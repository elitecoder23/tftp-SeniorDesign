/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Server::WriteOperationImpl.
 **/

#include "WriteOperationImpl.hpp"

#include <tftp/TftpLogger.hpp>
#include <tftp/TftpException.hpp>
#include <tftp/ReceiveDataHandler.hpp>

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/ErrorPacket.hpp>
#include <tftp/packets/TftpOptions.hpp>

#include <utility>

namespace Tftp::Server {

WriteOperationImpl::WriteOperationImpl(
  boost::asio::io_context &ioContext,
  WriteOperationConfiguration configuration ) :
  OperationImpl{
    ioContext,
    configuration.tftpTimeout,
    configuration.tftpRetries,
    static_cast< uint16_t >( Packets::DefaultTftpDataPacketHeaderSize
      + std::max(
        Packets::DefaultDataSize,
        configuration.optionsConfiguration.blockSizeOption.get_value_or(
          Packets::DefaultDataSize ) ) ),
    configuration.completionHandler,
    configuration.remote,
    configuration.local },
  configurationV{ std::move( configuration ) }
{
}

void WriteOperationImpl::start()
{
  BOOST_LOG_FUNCTION()

  try
  {
    // Reset data handler
    configurationV.dataHandler->reset();

    // option negotiation leads to empty option list
    if ( !configurationV.clientOptions
      && configurationV.additionalNegotiatedOptions.empty() )
    {
      // Then no OACK is sent back - a simple ACK is sent.
      send( Packets::AcknowledgementPacket{ Packets::BlockNumber{ 0U } } );
    }
    else
    {
      // initialise server options with additional negotiated options
      Packets::Options serverOptions{ configurationV.additionalNegotiatedOptions };

      // check block size option - if set use it
      if ( configurationV.optionsConfiguration.blockSizeOption
        && configurationV.clientOptions.blockSize )
      {
        receiveDataSize = std::min(
          *configurationV.clientOptions.blockSize,
          *configurationV.optionsConfiguration.blockSizeOption );

        // respond option string
        serverOptions.try_emplace(
          Packets::TftpOptions_name( Packets::KnownOptions::BlockSize ),
          std::to_string( receiveDataSize ) );
      }

      // check timeout option - if set use it
      if ( configurationV.optionsConfiguration.timeoutOption
        && configurationV.clientOptions.timeout
        && ( std::chrono::seconds{ *configurationV.clientOptions.timeout }
          <= *configurationV.optionsConfiguration.timeoutOption ) )
      {
        receiveTimeout(
          std::chrono::seconds{ *configurationV.clientOptions.timeout } );

        // respond with timeout option set
        serverOptions.try_emplace(
          Packets::TftpOptions_name( Packets::KnownOptions::Timeout ),
          std::to_string( *configurationV.clientOptions.timeout ) );
      }

      // check transfer size option
      if ( configurationV.optionsConfiguration.handleTransferSizeOption
        && configurationV.clientOptions.transferSize )
      {
        if ( !configurationV.dataHandler->receivedTransferSize(
          *configurationV.clientOptions.transferSize ) )
        {
          Packets::ErrorPacket errorPacket{
            Packets::ErrorCode::DiskFullOrAllocationExceeds,
            "FILE TO BIG" };

          send( errorPacket );

          // Operation completed
          finished( TransferStatus::TransferError, std::move( errorPacket ) );

          return;
        }

        // respond option string
        serverOptions.try_emplace(
          Packets::TftpOptions_name( Packets::KnownOptions::TransferSize ),
          std::to_string( *configurationV.clientOptions.transferSize ) );
      }

      if ( !serverOptions.empty() )
      {
        // send OACK
        send( Packets::OptionsAcknowledgementPacket{ serverOptions } );
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

void WriteOperationImpl::finished(
  const TransferStatus status,
  ErrorInfo &&errorInfo ) noexcept
{
  BOOST_LOG_FUNCTION()

  // Complete data handler
  configurationV.dataHandler->finished();

  // Inform base class
  OperationImpl::finished( status, std::move( errorInfo ) );
}

void WriteOperationImpl::dataPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::DataPacket &dataPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "RX: " << static_cast< std::string>( dataPacket );

  // Check retransmission
  if ( dataPacket.blockNumber() == lastReceivedBlockNumber )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
      << "Retransmission of last packet - only send ACK";

    send( Packets::AcknowledgementPacket{ lastReceivedBlockNumber } );

    // if received data size is smaller than the expected -> last packet has been
    // received
    if ( dataPacket.dataSize() < receiveDataSize )
    {
      if ( configurationV.dally )
      {
        // wait for potential retry of Data.
        receiveDally();
      }
      else
      {
        finished( TransferStatus::Successful );
      }
    }
    else
    {
      // receive next packet
      receive();
    }

    return;
  }

  // check not expected block
  if ( dataPacket.blockNumber() != lastReceivedBlockNumber.next() )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Unexpected packet";

    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::IllegalTftpOperation,
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

    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::IllegalTftpOperation,
      "Too much data" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );

    return;
  }

  // call data handler
  configurationV.dataHandler->receivedData( dataPacket.data() );

  // increment block number
  lastReceivedBlockNumber++;

  send( Packets::AcknowledgementPacket{ lastReceivedBlockNumber } );

  // if received data size is smaller than the expected -> last packet has been
  // received
  if ( dataPacket.dataSize() < receiveDataSize )
  {
    if ( configurationV.dally )
    {
      // wait for potential retry of Data.
      receiveDally();
    }
    else
    {
      finished( TransferStatus::Successful );
    }
  }
  else
  {
    // receive next packet
    receive();
  }
}

void WriteOperationImpl::acknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::AcknowledgementPacket &acknowledgementPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
    << "RX ERROR: " << static_cast< std::string>( acknowledgementPacket );

  Packets::ErrorPacket errorPacket{
    Packets::ErrorCode::IllegalTftpOperation,
    "ACK not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

}
