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
    BOOST_THROW_EXCEPTION( TftpException{}
      << Helper::AdditionalInfo{ "Parameter invalid" }
      << TransferPhaseInfo{ TransferPhase::Initialisation } );
  }

  try
  {
    // initialise socket
    initialise();

    // Reset data handler
    dataHandlerV->start();

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

    // Add the transfer size option if requested.
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

    // send the write request packet
    sendFirst( Packets::WriteRequestPacket{ filenameV, modeV, std::move( options ) } );

    // wait for answers
    receiveFirst();
  }
  catch ( const boost::exception &e )
  {
    SPDLOG_ERROR( "Exception during request {}", boost::diagnostic_information( e ) );

    finished( TransferStatus::CommunicationError );
  }
}

void WriteOperationImpl::gracefulAbort( const Packets::ErrorCode errorCode, std::string errorMessage )
{
  OperationImpl::gracefulAbort( errorCode, std::move( errorMessage ) );
}

void WriteOperationImpl::abort()
{
  OperationImpl::abort();
}

const Packets::ErrorInformation& WriteOperationImpl::errorInformation() const
{
  return OperationImpl::errorInformation();
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

WriteOperation& WriteOperationImpl::mode( const Packets::TransferMode mode )
{
  modeV = mode;
  return *this;
}

WriteOperation &WriteOperationImpl::remote( const boost::asio::ip::udp::endpoint remote )
{
  OperationImpl::remote( remote );
  return *this;
}

WriteOperation &WriteOperationImpl::local( const boost::asio::ip::udp::endpoint local )
{
  OperationImpl::local( local );
  return *this;
}

void WriteOperationImpl::finished( const TransferStatus status, Packets::ErrorInformation errorInformation ) noexcept
{
  // Complete data handler
  dataHandlerV->finished();

  // Inform base class
  OperationImpl::finished( status, std::move( errorInformation ) );
}

void WriteOperationImpl::sendData()
{
  ++lastTransmittedBlockNumber;

  SPDLOG_TRACE( "Send Data #{}", static_cast< uint16_t >( lastTransmittedBlockNumber ) );

  const Packets::DataPacket data{ lastTransmittedBlockNumber, dataHandlerV->sendData( transmitDataSize ) };

  if ( data.dataSize() < transmitDataSize )
  {
    lastDataPacketTransmitted = true;
  }

  // send the packet
  send( data );
}

void WriteOperationImpl::dataPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::DataPacket &dataPacket )
{
  SPDLOG_ERROR( "RX Error: {}", static_cast< std::string>( dataPacket ) );

  const Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "DATA not expected" };
  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, errorPacket.errorInformation() );
}

void WriteOperationImpl::acknowledgementPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::AcknowledgementPacket &acknowledgementPacket )
{
  SPDLOG_TRACE( "RX: {}", static_cast< std::string>( acknowledgementPacket ) );

  // check retransmission
  if ( acknowledgementPacket.blockNumber() == lastReceivedBlockNumber )
  {
    SPDLOG_WARN(
      "Received previous ACK packet: retry of last data package - "
      "IGNORE it due to Sorcerer's Apprentice Syndrome" );

    // receive next packet
    receive();

    return;
  }

  // check invalid block number
  if ( acknowledgementPacket.blockNumber() != lastTransmittedBlockNumber )
  {
    SPDLOG_ERROR( "Invalid block number received" );

    // send error packet
    const Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "Wrong block number" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, errorPacket.errorInformation() );
    return;
  }

  lastReceivedBlockNumber = acknowledgementPacket.blockNumber();

  // if the block number is 0 -> ACK of write without Options
  if ( acknowledgementPacket.blockNumber() == Packets::BlockNumber{ 0U } )
  {
    // Call Option Negotiation Handler with an empty options list.
    // If no Handler is registered - Continue Operation.
    // If options negotiation is aborted by Option Negotiation Handler - Abort Operation
    if ( Packets::Options options; optionNegotiationHandlerV && !optionNegotiationHandlerV( options ) )
    {
      SPDLOG_ERROR( "Option Negotiation failed" );

      const Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Option Negotiation Failed" };
      send( errorPacket );

      finished( TransferStatus::TransferError, errorPacket.errorInformation() );
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
  SPDLOG_TRACE( "RX: {}", static_cast< std::string>( optionsAcknowledgementPacket ) );

  if ( lastReceivedBlockNumber != Packets::BlockNumber{ 0xFFFFU } )
  {
    SPDLOG_ERROR( "OACK must occur after WRQ" );

    // send error packet
    const Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "OACK must occur after WRQ" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, errorPacket.errorInformation() );
    return;
  }

  auto remoteOptions{ optionsAcknowledgementPacket.options() };

  // check empty options - OACK with no option is not allowed
  if ( remoteOptions.empty() )
  {
    SPDLOG_ERROR( "Received option list is empty" );

    // send error packet
    const Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "Empty OACK not allowed" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, errorPacket.errorInformation() );
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
    SPDLOG_ERROR( "Block Size Option isn't expected" );

    const Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Block Size Option not expected" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, errorPacket.errorInformation() );
    return;
  }

  if ( !blockSizeValid )
  {
    SPDLOG_ERROR( "Block Size Option decoding failed" );

    const Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Block Size Option decoding failed" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, errorPacket.errorInformation() );
    return;
  }

  if ( blockSizeValue )
  {
    if ( *blockSizeValue > *optionsConfigurationV.blockSizeOption )
    {
      SPDLOG_ERROR( "Received Block Size Option bigger than negotiated" );

      const Packets::ErrorPacket errorPacket{
        Packets::ErrorCode::TftpOptionRefused,
        "Block size Option negotiation failed" };
      send( errorPacket );

      // Operation completed
      finished( TransferStatus::OptionNegotiationError, errorPacket.errorInformation() );
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
    SPDLOG_ERROR( "Timeout Option not expected" );

    const Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Timeout Option isn't expected" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, errorPacket.errorInformation() );
    return;
  }

  if ( !timeoutValid )
  {
    SPDLOG_ERROR( "Timeout Option decoding failed" );

    const Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Timeout Option decoding failed" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, errorPacket.errorInformation() );
    return;
  }

  if ( timeoutValue )
  {
    // Timeout Option Response from Server must be equal to Client Value
    if ( std::chrono::seconds{ *timeoutValue } != *optionsConfigurationV.timeoutOption )
    {
      SPDLOG_ERROR( "Timeout option not equal to requested" );

      const Packets::ErrorPacket errorPacket{
        Packets::ErrorCode::TftpOptionRefused,
        "Timeout option not equal to requested" };
      send( errorPacket );

      // Operation completed
      finished( TransferStatus::OptionNegotiationError, errorPacket.errorInformation() );
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
    SPDLOG_ERROR( "Transfer Size Option isn't expected" );

    const Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::TftpOptionRefused,
      "Transfer Size Option isn't expected" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, errorPacket.errorInformation() );
    return;
  }

  if ( !transferSizeValid )
  {
    SPDLOG_ERROR( "Transfer Size Option decoding failed" );

    const Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::TftpOptionRefused,
      "Transfer Size Option decoding failed" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, errorPacket.errorInformation() );
    return;
  }

  if ( transferSizeValue && *transferSizeValue != *transferSize )
  {
    SPDLOG_ERROR( "Transfer size value is not equal to sent value" );

    const Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "transfer size invalid" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, errorPacket.errorInformation() );
    return;
  }

  // Perform additional option negotiation.
  // If no handler is registered - Accept options and continue operation
  if ( optionNegotiationHandlerV && !optionNegotiationHandlerV( remoteOptions ) )
  {
    SPDLOG_ERROR( "Option negotiation failed" );

    const Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Option negotiation failed" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, errorPacket.errorInformation() );
    return;
  }

  // check that the remaining remote options are empty
  if ( !remoteOptions.empty() )
  {
    SPDLOG_ERROR( "Option negotiation failed - unexpected options" );

    const Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Unexpected options" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, errorPacket.errorInformation() );
    return;
  }

  // send data
  sendData();

  // receive next packet
  receive();
}

}
