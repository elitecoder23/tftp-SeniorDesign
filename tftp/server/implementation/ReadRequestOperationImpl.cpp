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
  const Options &clientOptions ) :
  OperationImpl{
    ioContext,
    tftpTimeout,
    tftpRetries,
    completionHandler,
    remote},
  dataHandler{ dataHandler },
  optionsConfiguration{ optionsConfiguration },
  clientOptions{ clientOptions },
  transmitDataSize{ DefaultDataSize },
  lastDataPacketTransmitted{ false },
  lastTransmittedBlockNumber{ 0U }
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
  const boost::asio::ip::udp::endpoint &local ) :
  OperationImpl{
    ioContext,
    tftpTimeout,
    tftpRetries,
    completionHandler,
    remote,
    local },
  dataHandler{ dataHandler },
  optionsConfiguration{ optionsConfiguration },
  clientOptions{ clientOptions },
  transmitDataSize{ DefaultDataSize },
  lastDataPacketTransmitted{ false },
  lastTransmittedBlockNumber{ 0U }
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
    if ( clientOptions.empty())
    {
      sendData();
    }
    else
    {
      auto serverOptions{ clientOptions };

      // check block size option - if set use it
      if ( optionsConfiguration.blockSizeOption )
      {
        const auto blockSize{
          blockSizeOption( clientOptions, BlockSizeOptionMin, *optionsConfiguration.blockSizeOption ) };

        if ( blockSize )
        {
          transmitDataSize = *blockSize;
        }

        // respond option string
        serverOptions.emplace(
          TftpOptionsConfiguration::optionName( KnownOptions::BlockSize),
          std::to_string( *blockSize ) );
      }

      // check timeout option - if set use it
      if ( optionsConfiguration.timeoutOption )
      {
        const auto timeoutOptionV{ timeoutOption( clientOptions ) };

        if ( timeoutOptionV )
        {
          receiveTimeout( *timeoutOptionV );
        }

        // respond option string
        serverOptions.emplace(
          TftpOptionsConfiguration::optionName( KnownOptions::Timeout ),
          std::to_string( *timeoutOptionV ) );
      }

      // check transfer size option
      if ( optionsConfiguration.handleTransferSizeOption )
      {
        auto transferSize{ transferSizeOption( clientOptions ) };

        if ( transferSize )
        {
          if ( 0U != *transferSize )
          {
            BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
              << "Received transfer size must be 0";

            Packets::ErrorPacket errorPacket{
              ErrorCode::TftpOptionRefused,
              "transfer size must be 0"};
            send( errorPacket);

            // Operation completed
            finished( TransferStatus::TransferError, std::move( errorPacket));

            return;
          }

          if ( transferSize = dataHandler->requestedTransferSize() ; transferSize )
          {
            // respond option string
            serverOptions.emplace(
              TftpOptionsConfiguration::optionName( KnownOptions::TransferSize ),
              std::to_string( *transferSize ) );
          }
        }
      }

      // if transfer size option is the only option requested, but the handler
      // does not supply it -> empty OACK is not sent but data directly
      if ( !serverOptions.empty())
      {
        // Send OACK
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
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "Error during Operation: " << e.what();
  }
  catch ( ...)
  {
    finished( TransferStatus::CommunicationError);
  }
}

void ReadRequestOperationImpl::finished(
  const TransferStatus status,
  ErrorInfo &&errorInfo) noexcept
{
  BOOST_LOG_FUNCTION()

  OperationImpl::finished( status, std::move( errorInfo));
  dataHandler->finished();
}

void ReadRequestOperationImpl::sendData()
{
  BOOST_LOG_FUNCTION()

  lastTransmittedBlockNumber++;

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info)
    << "Send Data: " << static_cast< uint16_t >( lastTransmittedBlockNumber);

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
  const Packets::DataPacket &dataPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
    << "RX ERROR: " << static_cast< std::string>( dataPacket);

  using namespace std::literals;
  Packets::ErrorPacket errorPacket{
    ErrorCode::IllegalTftpOperation,
    "DATA not expected"sv};

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket));
}

void ReadRequestOperationImpl::acknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info)
    << "RX: " << static_cast< std::string>( acknowledgementPacket);

  // check retransmission
  if (acknowledgementPacket.blockNumber() == lastTransmittedBlockNumber.previous())
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info)
      << "Received previous ACK packet: retry of last data package - "
         "IGNORE it due to Sorcerer's Apprentice Syndrome";

    // receive next packet
    receive();

    return;
  }

  // check invalid block number
  if (acknowledgementPacket.blockNumber() != lastTransmittedBlockNumber)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "Invalid block number received";

    using namespace std::literals;
    Packets::ErrorPacket errorPacket{
      ErrorCode::IllegalTftpOperation,
      "Block number not expected"sv};

    send( errorPacket);

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket));

    return;
  }

  // if it was the last ACK of the last data packet - we are finished.
  if ( lastDataPacketTransmitted)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info)
      << "Last acknowledgement received";

    finished( TransferStatus::Successful);

    return;
  }

  // send data
  sendData();

  // receive next packet
  receive();
}

}
