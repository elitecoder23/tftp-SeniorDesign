/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Server::ReadOperationImpl.
 **/

#include "ReadOperationImpl.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>
#include <tftp/TransmitDataHandler.hpp>

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/ErrorPacket.hpp>
#include <tftp/packets/TftpOptions.hpp>

#include <utility>

namespace Tftp::Server {

ReadOperationImpl::ReadOperationImpl(
  boost::asio::io_context &ioContext,
  ReadOperationConfiguration configuration ) :
  OperationImpl{
    ioContext,
    configuration.tftpTimeout,
    configuration.tftpRetries,
    Packets::DefaultMaxPacketSize,
    configuration.completionHandler,
    configuration.remote,
    configuration.local },
  configurationV{ std::move( configuration ) }
{
}

void ReadOperationImpl::start()
{
  BOOST_LOG_FUNCTION()

  try
  {
    // Reset data handler
    configurationV.dataHandler->reset();

    // option negotiation leads to empty option list
    if ( configurationV.clientOptions.empty()
      && configurationV.additionalNegotiatedOptions.empty() )
    {
      sendData();
    }
    else
    {
      // initialise server options with additional negotiated options
      Packets::Options serverOptions{ configurationV.additionalNegotiatedOptions };

      // check block size option - if set use it
      if ( configurationV.optionsConfiguration.blockSizeOption )
      {
        const auto [ blockSizeValid, blockSize ] =
          Packets::TftpOptions_getOption< uint16_t >(
            configurationV.clientOptions,
            Packets::TftpOptions_name( Packets::KnownOptions::BlockSize ),
            Packets::BlockSizeOptionMin,
            Packets::BlockSizeOptionMax );

        if ( blockSize )
        {
          transmitDataSize = std::min(
            *blockSize,
            *configurationV.optionsConfiguration.blockSizeOption );

          // respond option string
          serverOptions.try_emplace(
            Packets::TftpOptions_name( Packets::KnownOptions::BlockSize ),
            std::to_string( transmitDataSize ) );
        }
      }

      // check timeout option - if set use it
      if ( configurationV.optionsConfiguration.timeoutOption )
      {
        const auto [ timeoutValid, timeout ] =
          Packets::TftpOptions_getOption< uint8_t >(
            configurationV.clientOptions,
            Packets::TftpOptions_name( Packets::KnownOptions::Timeout ),
            Packets::TimeoutOptionMin,
            Packets::TimeoutOptionMax );

        if ( timeoutValid && timeout
          && ( std::chrono::seconds{ *timeout }
            <= *configurationV.optionsConfiguration.timeoutOption ) )
        {
          receiveTimeout( std::chrono::seconds{ *timeout } );

          // respond with timeout option set
          serverOptions.try_emplace(
            Packets::TftpOptions_name( Packets::KnownOptions::Timeout ),
            std::to_string( *timeout ) );
        }
      }

      // check transfer size option
      if ( configurationV.optionsConfiguration.handleTransferSizeOption )
      {
        const auto [ transferSizeValid, transferSize ] =
          Packets::TftpOptions_getOption< uint64_t >(
            configurationV.clientOptions,
            Packets::TftpOptions_name( Packets::KnownOptions::TransferSize ) );

        if ( transferSize )
        {
          if ( 0U != *transferSize )
          {
            BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
              << "Received transfer size must be 0";

            Packets::ErrorPacket errorPacket{
              Packets::ErrorCode::TftpOptionRefused,
              "transfer size must be 0" };
            send( errorPacket );

            // Operation completed
            finished( TransferStatus::TransferError, std::move( errorPacket ) );

            return;
          }

          if (
            auto newTransferSize = configurationV.dataHandler->requestedTransferSize();
            newTransferSize )
          {
            // respond option string
            serverOptions.try_emplace(
              Packets::TftpOptions_name( Packets::KnownOptions::TransferSize ),
              std::to_string( *newTransferSize ) );
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
  catch ( const TftpException &e )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Error during Operation: " << e.what();
  }
  catch ( ... )
  {
    finished( TransferStatus::CommunicationError );
  }
}

void ReadOperationImpl::finished(
  const TransferStatus status,
  ErrorInfo &&errorInfo) noexcept
{
  BOOST_LOG_FUNCTION()

  OperationImpl::finished( status, std::move( errorInfo ) );
  configurationV.dataHandler->finished();
}

void ReadOperationImpl::sendData()
{
  BOOST_LOG_FUNCTION()

  lastTransmittedBlockNumber++;

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "Send Data: " << static_cast< uint16_t >( lastTransmittedBlockNumber );

  Packets::DataPacket data{
    lastTransmittedBlockNumber,
    configurationV.dataHandler->sendData( transmitDataSize ) };

  if ( data.dataSize() < transmitDataSize )
  {
    lastDataPacketTransmitted = true;
  }

  // send data
  send( data );
}

void ReadOperationImpl::dataPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::DataPacket &dataPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
    << "RX ERROR: " << static_cast< std::string>( dataPacket );

  Packets::ErrorPacket errorPacket{
    Packets::ErrorCode::IllegalTftpOperation,
    "DATA not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void ReadOperationImpl::acknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::AcknowledgementPacket &acknowledgementPacket )
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
  if ( acknowledgementPacket.blockNumber() != lastTransmittedBlockNumber )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Invalid block number received";

    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::IllegalTftpOperation,
      "Block number not expected" };

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
