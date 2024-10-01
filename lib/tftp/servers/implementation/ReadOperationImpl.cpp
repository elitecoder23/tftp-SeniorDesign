// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Servers::ReadOperationImpl.
 **/

#include "ReadOperationImpl.hpp"

#include "tftp/packets/AcknowledgementPacket.hpp"
#include "tftp/packets/DataPacket.hpp"
#include "tftp/packets/OptionsAcknowledgementPacket.hpp"

#include "tftp/Logger.hpp"
#include "tftp/TftpException.hpp"
#include "tftp/TransmitDataHandler.hpp"

#include "helper/Exception.hpp"

#include <boost/exception/all.hpp>

#include <utility>

namespace Tftp::Servers {

ReadOperationImpl::ReadOperationImpl( boost::asio::io_context &ioContext ) :
  OperationImpl{ ioContext }
{
}

ReadOperation& ReadOperationImpl::tftpTimeout(
  const std::chrono::seconds timeout )
{
  OperationImpl::tftpTimeout( timeout );
  return *this;
}

ReadOperation& ReadOperationImpl::tftpRetries( const uint16_t retries )
{
  OperationImpl::tftpRetries( retries );
  return *this;
}

ReadOperation& ReadOperationImpl::optionsConfiguration(
  TftpOptionsConfiguration optionsConfiguration )
{
  optionsConfigurationV = std::move( optionsConfiguration );
  return *this;
}

ReadOperation& ReadOperationImpl::completionHandler(
  OperationCompletedHandler handler )
{
  OperationImpl::completionHandler( std::move( handler ) );
  return *this;
}

ReadOperation& ReadOperationImpl::dataHandler( TransmitDataHandlerPtr handler )
{
  dataHandlerV = std::move( handler );
  return *this;
}

ReadOperation& ReadOperationImpl::remote(
  boost::asio::ip::udp::endpoint remote )
{
  OperationImpl::remote( std::move( remote ) );
  return *this;
}

ReadOperation& ReadOperationImpl::local( boost::asio::ip::udp::endpoint local )
{
  OperationImpl::local( std::move( local ) );
  return *this;
}

ReadOperation& ReadOperationImpl::clientOptions(
  Packets::TftpOptions clientOptions )
{
  clientOptionsV = std::move( clientOptions );
  return *this;
}

ReadOperation& ReadOperationImpl::additionalNegotiatedOptions(
  Packets::Options additionalNegotiatedOptions )
{
  additionalNegotiatedOptionsV = std::move( additionalNegotiatedOptions );
  return *this;
}

void ReadOperationImpl::start()
{
  BOOST_LOG_FUNCTION()

  if ( !dataHandlerV )
  {
    BOOST_THROW_EXCEPTION( TftpException()
      << Helper::AdditionalInfo{ "Parameter Invalid" }
      << TransferPhaseInfo{ TransferPhase::Initialisation } );
  }

  try
  {
    // initialise socket
    initialise();

    // Reset data handler
    dataHandlerV->reset();

    // option negotiation leads to empty option list
    if ( !clientOptionsV && additionalNegotiatedOptionsV.empty() )
    {
      // Then no OACK is sent back - data is sent immediately
      sendData();
    }
    else
    {
      // initialise server options with additional negotiated options
      Packets::Options serverOptions{ additionalNegotiatedOptionsV };

      // check block size option - if set use it
      if ( optionsConfigurationV.blockSizeOption && clientOptionsV.blockSize )
      {
        transmitDataSize = std::min(
          *clientOptionsV.blockSize,
          *optionsConfigurationV.blockSizeOption );

        // respond option string
        serverOptions.try_emplace(
          Packets::TftpOptions_name( Packets::KnownOptions::BlockSize ),
          std::to_string( transmitDataSize ) );
      }

      // check timeout option - if set use it
      if ( optionsConfigurationV.timeoutOption
        && clientOptionsV.timeout
        && ( std::chrono::seconds{ *clientOptionsV.timeout }
          <= *optionsConfigurationV.timeoutOption ) )
      {
        receiveTimeout(
          // NOLINTNEXTLINE(bugprone-unchecked-optional-access): false positive
          std::chrono::seconds{ *clientOptionsV.timeout } );

        // respond with timeout option set
        serverOptions.try_emplace(
          Packets::TftpOptions_name( Packets::KnownOptions::Timeout ),
          // NOLINTNEXTLINE(bugprone-unchecked-optional-access): false positive
          std::to_string( *clientOptionsV.timeout ) );
      }

      // check transfer size option
      if ( optionsConfigurationV.handleTransferSizeOption
        && clientOptionsV.transferSize )
      {
        if ( 0U != *clientOptionsV.transferSize )
        {
          BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
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
          auto newTransferSize = dataHandlerV->requestedTransferSize();
          newTransferSize )
        {
          // respond option string
          serverOptions.try_emplace(
            Packets::TftpOptions_name( Packets::KnownOptions::TransferSize ),
            std::to_string( *newTransferSize ) );
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
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "Error during Operation: " << e.what();
  }
  catch ( ... )
  {
    finished( TransferStatus::CommunicationError );
  }
}

void ReadOperationImpl::gracefulAbort(
  Packets::ErrorCode errorCode,
  std::string errorMessage )
{
  OperationImpl::gracefulAbort( errorCode, std::move( errorMessage ) );
}

void ReadOperationImpl::abort()
{
  OperationImpl::abort();
}

const Packets::ErrorInfo& ReadOperationImpl::errorInfo() const
{
  return OperationImpl::errorInfo();
}

void ReadOperationImpl::finished(
  const TransferStatus status,
  Packets::ErrorInfo &&errorInfo ) noexcept
{
  BOOST_LOG_FUNCTION()

  // Complete data handler
  dataHandlerV->finished();

  // Inform base class
  OperationImpl::finished( status, std::move( errorInfo ) );
}

void ReadOperationImpl::sendData()
{
  BOOST_LOG_FUNCTION()

  lastTransmittedBlockNumber++;

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::trace )
    << "Send Data #" << static_cast< uint16_t >( lastTransmittedBlockNumber );

  const Packets::DataPacket data{
    lastTransmittedBlockNumber,
    dataHandlerV->sendData( transmitDataSize ) };

  if ( data.dataSize() < transmitDataSize )
  {
    lastDataPacketTransmitted = true;
  }

  // send packet
  send( data );
}

void ReadOperationImpl::dataPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::DataPacket &dataPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
    << "RX Error: " << static_cast< std::string>( dataPacket );

  Packets::ErrorPacket errorPacket{
    Packets::ErrorCode::IllegalTftpOperation,
    "DATA not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void ReadOperationImpl::acknowledgementPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::AcknowledgementPacket &acknowledgementPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::trace )
    << "RX: " << static_cast< std::string>( acknowledgementPacket );

  // check retransmission
  if ( acknowledgementPacket.blockNumber() == lastReceivedBlockNumber )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::warning )
      << "Received previous ACK packet: retry of last data package - "
         "IGNORE it due to Sorcerer's Apprentice Syndrome";

    // receive next packet
    receive();

    return;
  }

  // check invalid block number
  if ( acknowledgementPacket.blockNumber() != lastTransmittedBlockNumber )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "Invalid block number received";

    // send error packet
    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::IllegalTftpOperation,
      "Wrong block number" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );
    return;
  }

  lastReceivedBlockNumber = acknowledgementPacket.blockNumber();

  // if it was the last ACK of the last data packet - we are finished.
  if ( lastDataPacketTransmitted )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::trace )
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
