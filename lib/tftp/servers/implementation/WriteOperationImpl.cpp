// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Servers::WriteOperationImpl.
 **/

#include "WriteOperationImpl.hpp"

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>

#include <tftp/ReceiveDataHandler.hpp>
#include <tftp/TftpException.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <utility>

namespace Tftp::Servers {

WriteOperationImpl::WriteOperationImpl( boost::asio::io_context &ioContext ) :
  OperationImpl{ ioContext }
{
}

WriteOperation& WriteOperationImpl::tftpTimeout(
  const std::chrono::seconds timeout )
{
  OperationImpl::tftpTimeout( timeout );
  return *this;
}

WriteOperation& WriteOperationImpl::tftpRetries( const uint16_t retries )
{
  OperationImpl::tftpRetries( retries );
  return *this;
}

WriteOperation& WriteOperationImpl::dally( const bool dally )
{
  dallyV = dally;
  return *this;
}

WriteOperation& WriteOperationImpl::optionsConfiguration(
  TftpOptionsConfiguration optionsConfiguration )
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

WriteOperation& WriteOperationImpl::completionHandler( OperationCompletedHandler handler )
{
  OperationImpl::completionHandler( std::move( handler ) );
  return *this;
}

WriteOperation& WriteOperationImpl::dataHandler( ReceiveDataHandlerPtr handler )
{
  dataHandlerV = std::move( handler );
  return *this;
}

WriteOperation& WriteOperationImpl::remote( boost::asio::ip::udp::endpoint remote )
{
  OperationImpl::remote( std::move( remote ) );
  return *this;
}

WriteOperation& WriteOperationImpl::local( boost::asio::ip::udp::endpoint local )
{
  OperationImpl::local( std::move( local ) );
  return *this;
}

WriteOperation& WriteOperationImpl::clientOptions( Packets::TftpOptions clientOptions )
{
  clientOptionsV = std::move( clientOptions );
  return *this;
}

WriteOperation& WriteOperationImpl::additionalNegotiatedOptions( Packets::Options additionalNegotiatedOptions )
{
  additionalNegotiatedOptionsV = std::move( additionalNegotiatedOptions );
  return *this;
}

void WriteOperationImpl::start()
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

    // option negotiation leads to an empty option list
    if ( !clientOptionsV && additionalNegotiatedOptionsV.empty() )
    {
      // Then no OACK is sent back - a simple ACK is sent.
      send( Packets::AcknowledgementPacket{ Packets::BlockNumber{ 0U } } );
    }
    else
    {
      // initialise server options with additional negotiated options
      Packets::Options serverOptions{ additionalNegotiatedOptionsV };

      // check block size option - if set use it
      if ( optionsConfigurationV.blockSizeOption && clientOptionsV.blockSize )
      {
        receiveDataSize = std::min( *clientOptionsV.blockSize, *optionsConfigurationV.blockSizeOption );

        // respond option string
        serverOptions.try_emplace(
          std::string{ Packets::TftpOptions_name( Packets::KnownOptions::BlockSize ) },
          std::to_string( receiveDataSize ) );
      }

      // check timeout option - if set use it
      if ( optionsConfigurationV.timeoutOption
        && clientOptionsV.timeout
        && ( std::chrono::seconds{ *clientOptionsV.timeout } <= *optionsConfigurationV.timeoutOption ) )
      {
        receiveTimeout( std::chrono::seconds{ *clientOptionsV.timeout } );

        // respond with timeout option set
        serverOptions.try_emplace(
          std::string{ Packets::TftpOptions_name( Packets::KnownOptions::Timeout ) },
          std::to_string( *clientOptionsV.timeout ) );
      }

      // check transfer size option
      if ( optionsConfigurationV.handleTransferSizeOption && clientOptionsV.transferSize )
      {
        if ( !dataHandlerV->receivedTransferSize( *clientOptionsV.transferSize ) )
        {
          Packets::ErrorPacket errorPacket{ Packets::ErrorCode::DiskFullOrAllocationExceeds, "FILE TO BIG" };

          send( errorPacket );

          // Operation completed
          finished( TransferStatus::TransferError, std::move( errorPacket ) );

          return;
        }

        // respond option string
        serverOptions.try_emplace(
          std::string{ Packets::TftpOptions_name( Packets::KnownOptions::TransferSize ) },
          std::to_string( *clientOptionsV.transferSize ) );
      }

      if ( !serverOptions.empty() )
      {
        // Send OACK
        send( Packets::OptionsAcknowledgementPacket{ serverOptions } );
      }
      else
      {
        // Send ACK instead of OACK
        send( Packets::AcknowledgementPacket{ Packets::BlockNumber{ 0U } } );
      }
    }

    // start receive loop
    receive();
  }
  catch ( const TftpException &e )
  {
    SPDLOG_ERROR( "Error during Operation: {}", e.what() );
  }
  catch ( ... )
  {
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

void WriteOperationImpl::finished( const TransferStatus status, Packets::ErrorInfo &&errorInfo ) noexcept
{
  // Complete data handler
  dataHandlerV->finished();

  // Inform base class
  OperationImpl::finished( status, std::move( errorInfo ) );
}

void WriteOperationImpl::dataPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::DataPacket &dataPacket )
{
  SPDLOG_TRACE( "RX: {}", static_cast< std::string>( dataPacket ) );

  // Check retransmission of last packet
  if ( dataPacket.blockNumber() == lastReceivedBlockNumber )
  {
    SPDLOG_INFO( "Retransmission of last packet - only send ACK" );

    // Retransmit last ACK packet
    send( Packets::AcknowledgementPacket{ lastReceivedBlockNumber } );

    // if the received data size is smaller than the expected
    if ( dataPacket.dataSize() < receiveDataSize )
    {
      // last packet has been received and the operation is finished
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
      // otherwise, wait for the next data package
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

  // pass data
  dataHandlerV->receivedData( dataPacket.data() );

  // increment received block number
  ++lastReceivedBlockNumber;

  // send ACK
  send( Packets::AcknowledgementPacket{ lastReceivedBlockNumber } );

  // if the received data size is smaller than the expected
  if ( dataPacket.dataSize() < receiveDataSize )
  {
    // last packet has been received and the operation is finished
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
    // otherwise, wait for the next data package
    receive();
  }
}

void WriteOperationImpl::acknowledgementPacket(
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

}
