// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Clients::WriteOperationImpl.
 **/

#include "WriteOperationImpl.hpp"

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/Options.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>
#include <tftp/packets/TftpOptions.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>

#include <tftp/TftpException.hpp>
#include <tftp/TransmitDataHandler.hpp>

#include <helper/Dump.hpp>
#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <utility>

namespace Tftp::Clients {

WriteOperationImpl::WriteOperationImpl( boost::asio::io_context &ioContext ) :
  OperationImpl{ ioContext }
{
}

void WriteOperationImpl::request()
{
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

    transmitDataSize = Packets::DefaultDataSize;
    lastDataPacketTransmitted = false;
    lastTransmittedBlockNumber = 0U;
    lastReceivedBlockNumber = 0xFFFFU;

    // initialise options with additional options
    Packets::Options options{ additionalOptionsV };

    // Block size Option
    if ( optionsConfigurationV.blockSizeOption )
    {
      options.try_emplace(
        std::string{ Packets::TftpOptions_name( Packets::KnownOptions::BlockSize ) },
        std::to_string( *optionsConfigurationV.blockSizeOption ) );
    }

    // Timeout Option
    if ( optionsConfigurationV.timeoutOption )
    {
      options.try_emplace(
        std::string{ Packets::TftpOptions_name( Packets::KnownOptions::Timeout ) },
        std::to_string( static_cast< uint16_t >( optionsConfigurationV.timeoutOption->count() ) ) );
    }

    // Add transfer size option if requested.
    if ( optionsConfigurationV.handleTransferSizeOption )
    {
      // If the handler supplies a transfer size
      transferSize = dataHandlerV->requestedTransferSize();
      if ( transferSize )
      {
        // set transfer size TFTP option
        options.try_emplace(
          std::string{ Packets::TftpOptions_name( Packets::KnownOptions::TransferSize ) },
          std::to_string( *transferSize ) );
      }
    }

    // send write request packet
    sendFirst( Packets::WriteRequestPacket{ filenameV, modeV, std::move( options ) } );

    // wait for answers
    receiveFirst();
  }
  catch ( const boost::exception &e )
  {
    spdlog::error( "Exception during request {}", boost::diagnostic_information( e ) );

    finished( TransferStatus::CommunicationError );
  }
}

void WriteOperationImpl::gracefulAbort( Packets::ErrorCode errorCode, std::string errorMessage )
{
  OperationImpl::gracefulAbort( errorCode, std::move( errorMessage ) );
}

void WriteOperationImpl::abort()
{
  OperationImpl::abort();
}

const Packets::ErrorInfo& WriteOperationImpl::errorInfo() const
{
  return OperationImpl::errorInfo();
}

WriteOperation& WriteOperationImpl::tftpTimeout( const std::chrono::seconds timeout )
{
  OperationImpl::tftpTimeout( timeout );
  return *this;
}

WriteOperation& WriteOperationImpl::tftpRetries( const uint16_t retries )
{
  OperationImpl::tftpRetries( retries );
  return *this;
}

WriteOperation& WriteOperationImpl::optionsConfiguration( TftpOptionsConfiguration optionsConfiguration )
{
  optionsConfigurationV = std::move( optionsConfiguration );
  return *this;
}

WriteOperation& WriteOperationImpl::additionalOptions( Packets::Options additionalOptions )
{
  additionalOptionsV = std::move( additionalOptions );
  return *this;
}

WriteOperation& WriteOperationImpl::optionNegotiationHandler( OptionNegotiationHandler handler )
{
  optionNegotiationHandlerV = std::move( handler );
  return *this;
}

WriteOperation& WriteOperationImpl::completionHandler( OperationCompletedHandler handler )
{
  OperationImpl::completionHandler( std::move( handler ) );
  return *this;
}

WriteOperation& WriteOperationImpl::dataHandler( TransmitDataHandlerPtr handler )
{
  dataHandlerV = std::move( handler );
  return *this;
}

WriteOperation& WriteOperationImpl::filename( std::string filename )
{
  filenameV = std::move( filename );
  return *this;
}

WriteOperation& WriteOperationImpl::mode( Packets::TransferMode mode )
{
  modeV = mode;
  return *this;
}

WriteOperation &WriteOperationImpl::remote( boost::asio::ip::udp::endpoint remote )
{
  OperationImpl::remote( remote );
  return *this;
}

WriteOperation &WriteOperationImpl::local( boost::asio::ip::udp::endpoint local )
{
  OperationImpl::local( local );
  return *this;
}

void WriteOperationImpl::finished( const TransferStatus status, Packets::ErrorInfo &&errorInfo ) noexcept
{
  // Complete data handler
  dataHandlerV->finished();

  // Inform base class
  OperationImpl::finished( status, std::move( errorInfo ) );
}

void WriteOperationImpl::sendData()
{
  lastTransmittedBlockNumber++;

  spdlog::trace( "Send Data #{}", static_cast< uint16_t >( lastTransmittedBlockNumber ) );

  const Packets::DataPacket data{ lastTransmittedBlockNumber, dataHandlerV->sendData( transmitDataSize ) };

  if ( data.dataSize() < transmitDataSize )
  {
    lastDataPacketTransmitted = true;
  }

  // send packet
  send( data );
}

void WriteOperationImpl::dataPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::DataPacket &dataPacket )
{
  spdlog::error( "RX Error: {}", static_cast< std::string>( dataPacket ) );

  Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "DATA not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void WriteOperationImpl::acknowledgementPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::AcknowledgementPacket &acknowledgementPacket )
{
  spdlog::trace( "RX: {}", static_cast< std::string>( acknowledgementPacket ) );

  // check retransmission
  if ( acknowledgementPacket.blockNumber() == lastReceivedBlockNumber )
  {
    spdlog::warn(
      "Received previous ACK packet: retry of last data package - "
      "IGNORE it due to Sorcerer's Apprentice Syndrome" );

    // receive next packet
    receive();

    return;
  }

  // check invalid block number
  if ( acknowledgementPacket.blockNumber() != lastTransmittedBlockNumber )
  {
    spdlog::error( "Invalid block number received" );

    // send error packet
    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "Wrong block number" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );
    return;
  }

  lastReceivedBlockNumber = acknowledgementPacket.blockNumber();

  // if block number is 0 -> ACK of write without Options
  if ( acknowledgementPacket.blockNumber() == Packets::BlockNumber{ 0U } )
  {
    // Call Option Negotiation Handler with empty options list.
    // If no Handler is registered - Continue Operation.
    // If options negotiation is aborted by Option Negotiation Handler - Abort
    //   Operation
    Packets::Options options{};
    if ( optionNegotiationHandlerV && !optionNegotiationHandlerV( options ) )
    {
      spdlog::error( "Option Negotiation failed" );

      Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Option Negotiation Failed" };

      send( errorPacket );

      finished( TransferStatus::TransferError, std::move( errorPacket ) );
      return;
    }
  }

  // if ACK for last data packet - QUIT
  if ( lastDataPacketTransmitted )
  {
    finished( TransferStatus::Successful );
    return;
  }

  // send data
  sendData();

  // wait for the next packet
  receive();
}

void WriteOperationImpl::optionsAcknowledgementPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket )
{
  spdlog::trace( "RX: {}", static_cast< std::string>( optionsAcknowledgementPacket ) );

  if ( lastReceivedBlockNumber != Packets::BlockNumber{ 0xFFFFU } )
  {
    spdlog::error( "OACK must occur after WRQ" );

    // send error packet
    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "OACK must occur after WRQ" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );
    return;
  }

  auto remoteOptions{ optionsAcknowledgementPacket.options() };

  // check empty options - OACK with no option is not allowed
  if ( remoteOptions.empty() )
  {
    spdlog::error( "Received option list is empty" );

    // send error packet
    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "Empty OACK not allowed" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );
    return;
  }

  // Block Size Option
  const auto [ blockSizeValid, blockSizeValue ] =
    Packets::Options_getOption< uint16_t >(
      remoteOptions,
      Packets::TftpOptions_name( Packets::KnownOptions::BlockSize ),
      Packets::BlockSizeOptionMin,
      Packets::BlockSizeOptionMax );

  if ( !optionsConfigurationV.blockSizeOption && blockSizeValue )
  {
    spdlog::error( "Block Size Option not expected" );

    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Block Size Option not expected" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  if ( !blockSizeValid )
  {
    spdlog::error( "Block Size Option decoding failed" );

    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Block Size Option decoding failed" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  if ( blockSizeValue )
  {
    if ( *blockSizeValue > *optionsConfigurationV.blockSizeOption )
    {
      spdlog::error( "Received Block Size Option bigger than negotiated" );

      Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Block size Option negotiation failed" };

      send( errorPacket );

      // Operation completed
      finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
      return;
    }

    transmitDataSize = *blockSizeValue;
  }

  // Timeout Option
  const auto [ timeoutValid, timeoutValue ] =
    Packets::Options_getOption< uint8_t>(
      remoteOptions,
      Packets::TftpOptions_name( Packets::KnownOptions::Timeout ),
      Packets::TimeoutOptionMin,
      Packets::TimeoutOptionMax );

  if ( !optionsConfigurationV.timeoutOption && timeoutValue )
  {
    spdlog::error( "Timeout Option not expected" );

    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Timeout Option not expected" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  if ( !timeoutValid )
  {
    spdlog::error( "Timeout Option decoding failed" );

    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Timeout Option decoding failed" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  if ( timeoutValue )
  {
    // Timeout Option Response from Server must be equal to Client Value
    if ( std::chrono::seconds{ *timeoutValue }
      != *optionsConfigurationV.timeoutOption )
    {
      spdlog::error( "Timeout Option not equal to requested" );

      Packets::ErrorPacket errorPacket{
        Packets::ErrorCode::TftpOptionRefused,
        "Timeout Option not equal to requested" };

      send( errorPacket );

      // Operation completed
      finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
      return;
    }

    receiveTimeout( std::chrono::seconds{ *timeoutValue } );
  }

  // Transfer Size Option
  const auto [ transferSizeValid, transferSizeValue ] = Packets::Options_getOption< uint64_t >(
    remoteOptions,
    Packets::TftpOptions_name( Packets::KnownOptions::TransferSize ) );

  if ( ( !optionsConfigurationV.handleTransferSizeOption || !transferSize ) && transferSizeValue )
  {
    spdlog::error( "Transfer Size Option not expected" );

    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Transfer Size Option not expected" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  if ( !transferSizeValid )
  {
    spdlog::error( "Transfer Size Option decoding failed" );

    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Transfer Size Option decoding failed" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  if ( transferSizeValue && *transferSizeValue != *transferSize )
  {
    spdlog::error( "Transfer size value not equal to sent value" );

    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "transfer size invalid" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  // Perform additional option negotiation.
  // If no handler is registered - Accept options and continue operation
  if ( optionNegotiationHandlerV && !optionNegotiationHandlerV( remoteOptions ) )
  {
    spdlog::error(  "Option negotiation failed" );

    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Option negotiation failed" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  // check that remaining remote options are empty
  if ( !remoteOptions.empty() )
  {
    spdlog::error( "Option negotiation failed - unexpected options" );

    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Unexpected options" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  // send data
  sendData();

  // receive next packet
  receive();
}

}
