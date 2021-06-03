/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Server::ReadRequestOperationImpl.
 **/

#include "ReadRequestOperationImpl.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>
#include <tftp/TransmitDataHandler.hpp>

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/ErrorPacket.hpp>

namespace Tftp::Server {

ReadRequestOperationImpl::ReadRequestOperationImpl(
  boost::asio::io_context &ioContext,
  uint8_t tftpTimeout,
  uint16_t tftpRetries,
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  const TftpOptionsConfiguration &optionsConfiguration,
  const Options &clientOptions,
  const Options &additionalNegotiatedOptions ) :
  OperationImpl{
    ioContext,
    tftpTimeout,
    tftpRetries,
    DefaultMaxPacketSize,
    completionHandler,
    remote },
  dataHandler{ dataHandler },
  optionsConfiguration{ optionsConfiguration },
  clientOptions{ clientOptions },
  additionalNegotiatedOptions{ additionalNegotiatedOptions },
  transmitDataSize{ DefaultDataSize },
  lastDataPacketTransmitted{ false },
  lastTransmittedBlockNumber{ 0U },
  lastReceivedBlockNumber{ 0U }
{
}

ReadRequestOperationImpl::ReadRequestOperationImpl(
  boost::asio::io_context &ioContext,
  uint8_t tftpTimeout,
  uint16_t tftpRetries,
  TransmitDataHandlerPtr dataHandler,
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
    DefaultMaxPacketSize,
    completionHandler,
    remote,
    local },
  dataHandler{ dataHandler },
  optionsConfiguration{ optionsConfiguration },
  clientOptions{ clientOptions },
  additionalNegotiatedOptions{ additionalNegotiatedOptions },
  transmitDataSize{ DefaultDataSize },
  lastDataPacketTransmitted{ false },
  lastTransmittedBlockNumber{ 0U },
  lastReceivedBlockNumber{ 0U }
{
}

void ReadRequestOperationImpl::start()
{
  BOOST_LOG_FUNCTION()

  try
  {
    // Reset data handler
    dataHandler->reset();

    // option negotiation leads to empty option list
    if ( clientOptions.empty() && additionalNegotiatedOptions.empty() )
    {
      sendData();
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
          if ( 0U != *transferSize )
          {
            BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
              << "Received transfer size must be 0";

            Packets::ErrorPacket errorPacket{
              ErrorCode::TftpOptionRefused,
              "transfer size must be 0" };
            send( errorPacket);

            // Operation completed
            finished( TransferStatus::TransferError, std::move( errorPacket ) );

            return;
          }

          if (
            auto newTransferSize = dataHandler->requestedTransferSize();
            newTransferSize )
          {
            // respond option string
            serverOptions.emplace( Packets::TftpOptions_setOption(
              KnownOptions::TransferSize,
              *newTransferSize ) );
          }
        }
      }

      // if transfer size option is the only option requested, but the handler
      // does not supply it -> empty OACK is not sent but data directly
      if ( !serverOptions.empty() )
      {
        // Send OACK
        // Update last received block - number to handle OACK Acknowledgment
        // correctly
        lastReceivedBlockNumber = 0xFFFFU;
        send( Packets::OptionsAcknowledgementPacket{ serverOptions } );
      }
      else
      {
        // directly send data
        sendData();
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

void ReadRequestOperationImpl::finished(
  const TransferStatus status,
  ErrorInfo &&errorInfo) noexcept
{
  BOOST_LOG_FUNCTION()

  OperationImpl::finished( status, std::move( errorInfo ) );
  dataHandler->finished();
}

void ReadRequestOperationImpl::sendData()
{
  BOOST_LOG_FUNCTION()

  lastTransmittedBlockNumber++;

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info)
    << "Send Data: " << static_cast< uint16_t >( lastTransmittedBlockNumber );

  Packets::DataPacket data{
    lastTransmittedBlockNumber,
    dataHandler->sendData( transmitDataSize)};

  if ( data.dataSize() < transmitDataSize)
  {
    lastDataPacketTransmitted = true;
  }

  // send data
  send( data);
}

void ReadRequestOperationImpl::dataPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::DataPacket &dataPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
    << "RX ERROR: " << static_cast< std::string>( dataPacket );

  using namespace std::literals;
  Packets::ErrorPacket errorPacket{
    ErrorCode::IllegalTftpOperation,
    "DATA not expected"sv };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void ReadRequestOperationImpl::acknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "RX: " << static_cast< std::string>( acknowledgementPacket );

  // check retransmission
  if ( acknowledgementPacket.blockNumber() == lastReceivedBlockNumber )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
      << "Received previous ACK packet: retry of last data package - "
         "IGNORE it due to Sorcerer's Apprentice Syndrome";

    // receive next packet
    receive();

    return;
  }

  // check invalid block number
  if (acknowledgementPacket.blockNumber() != lastTransmittedBlockNumber )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Invalid block number received";

    using namespace std::literals;
    Packets::ErrorPacket errorPacket{
      ErrorCode::IllegalTftpOperation,
      "Block number not expected"sv };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );

    return;
  }

  lastReceivedBlockNumber = acknowledgementPacket.blockNumber();

  // if it was the last ACK of the last data packet - we are finished.
  if ( lastDataPacketTransmitted )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
      << "Last acknowledgement received";

    finished( TransferStatus::Successful );

    return;
  }

  // send data
  sendData();

  // receive next packet
  receive();
}

}
