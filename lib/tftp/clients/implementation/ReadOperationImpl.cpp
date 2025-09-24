// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Clients::ReadOperationImpl.
 **/

#include "ReadOperationImpl.hpp"

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/Options.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>
#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/TftpOptions.hpp>

#include <tftp/ReceiveDataHandler.hpp>
#include <tftp/TftpException.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <utility>

namespace Tftp::Clients {

ReadOperationImpl::ReadOperationImpl( boost::asio::io_context &ioContext ):
  OperationImpl{ ioContext }
{
}

void ReadOperationImpl::request()
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

    receiveDataSize = Packets::DefaultDataSize;
    lastReceivedBlockNumber = 0U;

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

    // Add the transfer size option with size '0' if requested.
    if ( optionsConfigurationV.handleTransferSizeOption )
    {
      // assure that the transfer size is set to zero for read request
      options.try_emplace( std::string{ Packets::TftpOptions_name( Packets::KnownOptions::TransferSize ) }, "0" );
    }

    // send the read request packet
    sendFirst( Packets::ReadRequestPacket{ filenameV, modeV, std::move( options ) } );

    // wait for answers
    receiveFirst();
  }
  catch ( const boost::exception &e )
  {
    SPDLOG_ERROR( "Exception during request {}", boost::diagnostic_information( e ) );

    finished( TransferStatus::CommunicationError );
  }
}

void ReadOperationImpl::gracefulAbort( Packets::ErrorCode errorCode, std::string errorMessage )
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

ReadOperation& ReadOperationImpl::tftpTimeout( const std::chrono::seconds timeout )
{
  OperationImpl::tftpTimeout( timeout );
  return *this;
}

ReadOperation& ReadOperationImpl::tftpRetries( const uint16_t retries )
{
  OperationImpl::tftpRetries( retries );
  return *this;
}

ReadOperation& ReadOperationImpl::dally( const bool dally )
{
  dallyV = dally;
  return *this;
}

ReadOperation& ReadOperationImpl::optionsConfiguration( TftpOptionsConfiguration optionsConfiguration )
{
  optionsConfigurationV = std::move( optionsConfiguration );

  maxReceivePacketSize(
    static_cast< uint16_t >(
      Packets::DefaultTftpDataPacketHeaderSize
      + std::max(
        Packets::DefaultDataSize,
        optionsConfigurationV.blockSizeOption.get_value_or( Packets::DefaultDataSize ) ) ) );

  return *this;
}

ReadOperation& ReadOperationImpl::additionalOptions( Packets::Options additionalOptions )
{
  additionalOptionsV = std::move( additionalOptions );
  return *this;
}

ReadOperation& ReadOperationImpl::optionNegotiationHandler( OptionNegotiationHandler handler )
{
  optionNegotiationHandlerV = std::move( handler );
  return *this;
}

ReadOperation& ReadOperationImpl::completionHandler( OperationCompletedHandler handler )
{
  OperationImpl::completionHandler( std::move( handler ) );
  return *this;
}

ReadOperation& ReadOperationImpl::dataHandler( ReceiveDataHandlerPtr handler )
{
  dataHandlerV = std::move( handler );
  return *this;
}

ReadOperation& ReadOperationImpl::filename( std::string filename )
{
  filenameV = std::move( filename );
  return *this;
}

ReadOperation& ReadOperationImpl::mode( const Packets::TransferMode mode )
{
  modeV = mode;
  return *this;
}

ReadOperation& ReadOperationImpl::remote( const boost::asio::ip::udp::endpoint remote )
{
  OperationImpl::remote( remote );
  return *this;
}

ReadOperation& ReadOperationImpl::local( const boost::asio::ip::udp::endpoint local )
{
  OperationImpl::local( local );
  return *this;
}

void ReadOperationImpl::finished( const TransferStatus status, Packets::ErrorInfo &&errorInfo ) noexcept
{
  // Complete data handler
  dataHandlerV->finished();

  // Inform base class
  OperationImpl::finished( status, std::move( errorInfo ) );
}

void ReadOperationImpl::dataPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::DataPacket &dataPacket )
{
  SPDLOG_TRACE( "RX: {}", static_cast< std::string>( dataPacket ) );

  // Check retransmission of last packet
  if ( dataPacket.blockNumber() == lastReceivedBlockNumber )
  {
    SPDLOG_WARN( "Received last data package again. Re-ACK them" );

    // Retransmit last ACK packet
    send( Packets::AcknowledgementPacket{ lastReceivedBlockNumber } );

    // if the received data size is smaller than the expected
    if ( dataPacket.dataSize() < receiveDataSize )
    {
      // last packet has been received and operation is finished
      if ( dallyV )
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
      // otherwise, wait for next data package
      receive();
    }

    return;
  }

  // check unexpected block number
  if ( dataPacket.blockNumber() != lastReceivedBlockNumber.next() )
  {
    SPDLOG_ERROR( "Wrong Data packet block number" );

    // send error packet
    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "Block Number not expected" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );
    return;
  }

  // check for too much data
  if ( dataPacket.dataSize() > receiveDataSize )
  {
    SPDLOG_ERROR( "Too much data received" );

    // send error packet
    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "Too much data" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );
    return;
  }

  // if the block number is 1 -> DATA of write without Options
  if ( ( dataPacket.blockNumber() == static_cast< uint16_t >( 1U ) ) && ( !oackReceived ) )
  {
    // Call Option Negotiation Handler with an empty options list.
    // If no Handler is registered - Continue Operation.
    // If options negotiation is aborted by Option Negotiation Handler - Abort Operation
    if ( Packets::Options options; optionNegotiationHandlerV && !optionNegotiationHandlerV( options ) )
    {
      SPDLOG_ERROR( "Option Negotiation failed" );

      Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Option Negotiation Failed" };

      send( errorPacket );

      finished( TransferStatus::TransferError, std::move( errorPacket ) );
      return;
    }
  }

  // pass data
  dataHandlerV->receivedData( dataPacket.data() );

  // increment received block number
  ++lastReceivedBlockNumber;

  // send ACK
  send( Packets::AcknowledgementPacket{ lastReceivedBlockNumber } );

  // if received data size is smaller than the expected
  if ( dataPacket.dataSize() < receiveDataSize )
  {
    // last packet has been received and operation is finished
    if ( dallyV )
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
    // otherwise, wait for the next data packet
    receive();
  }
}

void ReadOperationImpl::acknowledgementPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::AcknowledgementPacket &acknowledgementPacket )
{
  SPDLOG_ERROR( "RX Error: {}", static_cast< std::string>( acknowledgementPacket ) );

  // send Error
  Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "ACK not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void ReadOperationImpl::optionsAcknowledgementPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket )
{
  SPDLOG_TRACE( "RX: {}", static_cast< std::string>( optionsAcknowledgementPacket ) );

  if ( lastReceivedBlockNumber != Packets::BlockNumber{ 0U } )
  {
    SPDLOG_ERROR( "OACK must occur after RRQ" );

    // send error packet
    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "OACK must occur after RRQ" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );
    return;
  }

  auto remoteOptions{ optionsAcknowledgementPacket.options() };

  // check empty options - OACK with no option is not allowed
  if ( remoteOptions.empty() )
  {
    SPDLOG_ERROR( "Received option list is empty" );

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
    SPDLOG_ERROR( "Block Size Option not expected" );

    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Block Size Option not expected" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  if ( !blockSizeValid )
  {
    SPDLOG_ERROR( "Block Size Option decoding failed" );

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
      SPDLOG_ERROR( "Received Block Size Option bigger than negotiated" );

      Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Block size Option negotiation failed" };

      send( errorPacket );

      // Operation completed
      finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
      return;
    }

    receiveDataSize = *blockSizeValue;
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

    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Timeout Option not expected" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  if ( !timeoutValid )
  {
    SPDLOG_ERROR( "Timeout Option decoding failed" );

    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Timeout Option decoding failed" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  if ( timeoutValue )
  {
    // Timeout Option Response from Server must be equal to Client Value
    if ( std::chrono::seconds{ *timeoutValue } != *optionsConfigurationV.timeoutOption )
    {
      SPDLOG_ERROR( "Timeout option not equal to requested" );

      Packets::ErrorPacket errorPacket{
        Packets::ErrorCode::TftpOptionRefused,
        "Timeout option not equal to requested" };

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

  if ( !optionsConfigurationV.handleTransferSizeOption && transferSizeValue )
  {
    SPDLOG_ERROR( "Transfer Size Option not expected" );

    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Transfer Size Option not expected" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  if ( !transferSizeValid )
  {
    SPDLOG_ERROR( "Transfer Size Option decoding failed" );

    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Transfer Size Option decoding failed" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  if ( transferSizeValue )
  {
    if ( !dataHandlerV->receivedTransferSize( *transferSizeValue ) )
    {
      Packets::ErrorPacket errorPacket{ Packets::ErrorCode::DiskFullOrAllocationExceeds, "File to big" };

      send( errorPacket );

      // Operation completed
      finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
      return;
    }
  }

  // Perform additional option negotiation.
  // If no handler is registered - Accept options and continue operation
  if ( optionNegotiationHandlerV && !optionNegotiationHandlerV( remoteOptions ) )
  {
    SPDLOG_ERROR( "Option negotiation failed" );

    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Option negotiation failed" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  // check that remaining remote options are empty
  if ( !remoteOptions.empty() )
  {
    SPDLOG_ERROR( "Option negotiation failed - unexpected options" );

    Packets::ErrorPacket errorPacket{ Packets::ErrorCode::TftpOptionRefused, "Unexpected options" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  // indicate Options acknowledgement
  oackReceived = true;

  // send Acknowledgment with block number set to 0
  send( Packets::AcknowledgementPacket{ Packets::BlockNumber{ 0 } } );

  // receive next packet
  receive();
}

}
