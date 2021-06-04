/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Server::WriteRequestOperationImpl.
 **/

#include "WriteRequestOperationImpl.hpp"

#include <tftp/TftpLogger.hpp>
#include <tftp/TftpException.hpp>
#include <tftp/ReceiveDataHandler.hpp>

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/ErrorPacket.hpp>

namespace Tftp::Server {

WriteRequestOperationImpl::WriteRequestOperationImpl(
  boost::asio::io_context &ioContext,
  const uint8_t tftpTimeout,
  const uint16_t tftpRetries,
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  const TftpOptionsConfiguration &optionsConfiguration,
  const Options &clientOptions,
  const Options &additionalNegotiatedOptions ) :
  OperationImpl{
    ioContext,
    tftpTimeout,
    tftpRetries,
    static_cast< uint16_t >( DefaultTftpDataPacketHeaderSize
      + std::max( DefaultDataSize, optionsConfiguration.blockSizeOption.get_value_or( DefaultDataSize ) ) ),
    completionHandler,
    remote },
  dataHandler{ dataHandler },
  optionsConfiguration{ optionsConfiguration },
  clientOptions{ clientOptions },
  additionalNegotiatedOptions{ additionalNegotiatedOptions },
  receiveDataSize{ DefaultDataSize },
  lastReceivedBlockNumber{ 0U }
{
}

WriteRequestOperationImpl::WriteRequestOperationImpl(
  boost::asio::io_context &ioContext,
  const uint8_t tftpTimeout,
  const uint16_t tftpRetries,
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  const TftpOptionsConfiguration &optionsConfiguration,
  const Options &clientOptions,
  const Options &additionalNegotiatedOptions,
  const boost::asio::ip::udp::endpoint &local ) :
  OperationImpl{
    ioContext,
    tftpTimeout,
    tftpRetries,
    static_cast< uint16_t >( DefaultTftpDataPacketHeaderSize
      + std::max( DefaultDataSize, optionsConfiguration.blockSizeOption.get_value_or( DefaultDataSize ) ) ),
    completionHandler,
    remote,
    local },
  dataHandler{ dataHandler},
  optionsConfiguration{ optionsConfiguration },
  clientOptions{ clientOptions },
  additionalNegotiatedOptions{ additionalNegotiatedOptions },
  receiveDataSize{ DefaultDataSize },
  lastReceivedBlockNumber{ 0U }
{
}

void WriteRequestOperationImpl::start()
{
  BOOST_LOG_FUNCTION()

  try
  {
    // Reset data handler
    dataHandler->reset();

    // option negotiation leads to empty option list
    if ( clientOptions.empty() && additionalNegotiatedOptions.empty() )
    {
      // Then no OACK is sent back - a simple ACK is sent.
      send( Packets::AcknowledgementPacket{ Packets::BlockNumber{ 0U } } );
    }
    else
    {
      Options serverOptions{ additionalNegotiatedOptions };

      // check block size option - if set use it
      if ( optionsConfiguration.blockSizeOption )
      {
        const auto [blockSizeValid, blockSize] =
          Packets::TftpOptions_getOption< uint16_t >(
            clientOptions,
            KnownOptions::BlockSize );

        if ( blockSize )
        {
          // respond option string
          serverOptions.emplace( Packets::TftpOptions_setOption(
            KnownOptions::BlockSize,
            std::min( *blockSize, *optionsConfiguration.blockSizeOption ) ) );

          receiveDataSize = *blockSize;
        }
      }

      // check timeout option - if set use it
      if ( optionsConfiguration.timeoutOption )
      {
        const auto [timeoutValid, timeout] =
        Packets::TftpOptions_getOption< uint8_t >(
          clientOptions,
          KnownOptions::Timeout );

        if ( timeout )
        {
          receiveTimeout( *timeout );
        }

        // respond option string
        serverOptions.emplace( Packets::TftpOptions_setOption(
          KnownOptions::Timeout,
          *timeout ) );
      }

      // check transfer size option
      if ( optionsConfiguration.handleTransferSizeOption )
      {
        const auto [transferSizeValid, transferSize] =
        Packets::TftpOptions_getOption< uint64_t >(
          clientOptions,
          KnownOptions::TransferSize );

        if ( transferSize )
        {
          if ( !dataHandler->receivedTransferSize( *transferSize ) )
          {
            Packets::ErrorPacket errorPacket{
              ErrorCode::DiskFullOrAllocationExceeds,
              "FILE TO BIG"};

            send( errorPacket);

            // Operation completed
            finished( TransferStatus::TransferError, std::move( errorPacket ) );

            return;
          }

          // respond option string
          serverOptions.emplace( Packets::TftpOptions_setOption(
            KnownOptions::TransferSize,
            *transferSize ) );
        }
      }

      if ( !serverOptions.empty() )
      {
        // send OACK
        send( Packets::OptionsAcknowledgementPacket{ clientOptions } );
      }
      else
      {
        // Send OACK instead of ACK
        send( Packets::AcknowledgementPacket{ Packets::BlockNumber{ 0U } } );
      }
    }

    // start receive loop
    receive();
  }
  catch ( TftpException &e)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Error during Operation: " << e.what();
  }
  catch ( ...)
  {
    finished( TransferStatus::CommunicationError );
  }
}

void WriteRequestOperationImpl::finished(
  const TransferStatus status,
  ErrorInfo &&errorInfo) noexcept
{
  BOOST_LOG_FUNCTION()

  OperationImpl::finished( status, std::move( errorInfo ) );
  dataHandler->finished();
}

void WriteRequestOperationImpl::dataPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::DataPacket &dataPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "RX: " << static_cast< std::string>( dataPacket );

  // Check retransmission
  if (dataPacket.blockNumber() == lastReceivedBlockNumber)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info)
      << "Retransmission of last packet - only send ACK";

    send( Packets::AcknowledgementPacket{ lastReceivedBlockNumber});

    // receive next packet
    receive();

    return;
  }

  // check not expected block
  if (dataPacket.blockNumber() != lastReceivedBlockNumber.next())
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "Unexpected packet";

    Packets::ErrorPacket errorPacket{
      ErrorCode::IllegalTftpOperation,
      "Wrong block number" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );

    return;
  }

  // check for too much data
  if ( dataPacket.dataSize() > receiveDataSize )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Too much data received";

    using namespace std::literals;
    Packets::ErrorPacket errorPacket{
      ErrorCode::IllegalTftpOperation,
      "Too much data"sv };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );

    return;
  }

  // call data handler
  dataHandler->receivedData( dataPacket.data());

  // increment block number
  lastReceivedBlockNumber++;

  send( Packets::AcknowledgementPacket( lastReceivedBlockNumber));

  // if received data size is smaller then the expected -> last packet has been
  // received
  if (dataPacket.dataSize() < receiveDataSize)
  {
    finished( TransferStatus::Successful);
  }
  else
  {
    // receive next packet
    receive();
  }
}

void WriteRequestOperationImpl::acknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
    << "RX ERROR: " << static_cast< std::string>( acknowledgementPacket);

  Packets::ErrorPacket errorPacket{
    ErrorCode::IllegalTftpOperation,
    "ACK not expected"};

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket));
}

}
