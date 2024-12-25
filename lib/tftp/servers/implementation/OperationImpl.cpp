// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Servers::OperationImpl.
 **/

#include "OperationImpl.hpp"

#include <tftp/packets/ErrorCodeDescription.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>
#include <tftp/packets/PacketStatistic.hpp>
#include <tftp/packets/PacketTypeDescription.hpp>
#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>

#include <tftp/Logger.hpp>
#include <tftp/TftpException.hpp>
#include <tftp/TftpOptionsConfiguration.hpp>

#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

#include <boost/bind/bind.hpp>

namespace Tftp::Servers {

OperationImpl::OperationImpl( boost::asio::io_context &ioContext ) :
  socket{ ioContext },
  timer{ ioContext },
  receivePacket( Packets::DefaultMaxPacketSize )
{
}

OperationImpl::~OperationImpl() = default;

void OperationImpl::initialise()
{
  try
  {
    // Open the socket
    socket.open( remoteV.protocol() );

    // bind to local address
    if ( !localV.address().is_unspecified() )
    {
      socket.bind( localV );
    }

    // connect to client.
    socket.connect( remoteV );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "Initialisation Error: " << err.what();

    // On error and if socket is opened - close it.
    if ( socket.is_open() )
    {
      socket.close();
    }

    // Operation finished
    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::gracefulAbort( const Packets::ErrorCode errorCode, std::string errorMessage )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::warning )
    << "Graceful abort requested: "
    << "'" << errorCode << "' '" << errorMessage << "'";

  Packets::ErrorPacket errorPacket{ errorCode, std::move( errorMessage ) };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::Aborted, std::move( errorPacket ) );
}

void OperationImpl::abort()
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::warning )
    << "Abort requested";

  // Operation completed
  finished( TransferStatus::Aborted );
}

const Packets::ErrorInfo& OperationImpl::errorInfo() const
{
  return errorInfoV;
}

void OperationImpl::tftpTimeout( std::chrono::seconds timeout )
{
  receiveTimeoutV = timeout;
}

void OperationImpl::tftpRetries( const uint16_t retries )
{
  tftpRetriesV = retries;
}

void OperationImpl::remote( boost::asio::ip::udp::endpoint remote )
{
  remoteV = std::move( remote );
}

void OperationImpl::local( boost::asio::ip::udp::endpoint local )
{
  localV = std::move( local );
}

void OperationImpl::completionHandler( OperationCompletedHandler handler )
{
  completionHandlerV = std::move( handler );
}

void OperationImpl::maxReceivePacketSize( const uint16_t maxReceivePacketSize )
{
  receivePacket.resize( maxReceivePacketSize );
}

void OperationImpl::receiveTimeout( const std::chrono::seconds receiveTimeout ) noexcept
{
  receiveTimeoutV = receiveTimeout;
}

void OperationImpl::send( const Packets::Packet &packet )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::trace )
    << "TX: " << static_cast< std::string>( packet );

  try
  {
    // Reset transmit counter
    transmitCounter = 1U;

    // Encode raw packet
    transmitPacket = static_cast< Packets::RawData >( packet );

    // Update statistic
    Packets::PacketStatistic::globalTransmit().packet( packet.packetType(), transmitPacket.size() );

    // Send the packet to the remote client
    socket.send( boost::asio::buffer( transmitPacket ) );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "TX Error: " << err.what();

    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::receive()
{
  BOOST_LOG_FUNCTION()

  try
  {
    // start receive operation
    socket.async_receive(
      boost::asio::buffer( receivePacket ),
      std::bind_front( &OperationImpl::receiveHandler, this ) );

    // set receive timeout
    timer.expires_after( receiveTimeoutV );

    // start waiting for receive timeout
    timer.async_wait( std::bind_front( &OperationImpl::timeoutHandler, this ) );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "RX Error: " << err.what();

    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::receiveDally()
{
  BOOST_LOG_FUNCTION()

  try
  {
    // start receive operation
    socket.async_receive(
      boost::asio::buffer( receivePacket ),
      std::bind_front( &OperationImpl::receiveHandler, this ) );

    // set receive timeout
    timer.expires_after( 2U * receiveTimeoutV );

    // start waiting for receive timeout
    timer.async_wait( std::bind_front( &OperationImpl::timeoutDallyHandler, this ) );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "RX Error: " << err.what();

    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::finished( const TransferStatus status, Packets::ErrorInfo &&errorInfo )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::info )
    << "TFTP Server Operation finished";

  errorInfoV = std::move( errorInfo );

  timer.cancel();
  socket.cancel();
  socket.close();

  if ( completionHandlerV )
  {
    completionHandlerV( status );
  }
}

void OperationImpl::readRequestPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::ReadRequestPacket &readRequestPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
    << "RX Error: " << static_cast< std::string>( readRequestPacket );

  // send error packet
  Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "RRQ not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void OperationImpl::writeRequestPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::WriteRequestPacket &writeRequestPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
    << "RX Error: " << static_cast< std::string>( writeRequestPacket );

  // send error packet
  Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "WRQ not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void OperationImpl::errorPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::ErrorPacket &errorPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
    << "RX Error: " << static_cast< std::string>( errorPacket );

  // Operation completed
  switch ( Packets::Packet::packetType( transmitPacket ) )
  {
    case Packets::PacketType::OptionsAcknowledgement:
      switch ( errorPacket.errorCode() )
      {
        case Packets::ErrorCode::TftpOptionRefused:
          // TFTP Option negotiation refused
          finished( TransferStatus::OptionNegotiationError, errorPacket );
          break;

        default:
          finished( TransferStatus::TransferError, errorPacket );
          break;
      }
      break;

    default:
      // error for other package
      finished( TransferStatus::TransferError, errorPacket );
      break;
  }
}

void OperationImpl::optionsAcknowledgementPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
    << "RX Error: " << static_cast< std::string>( optionsAcknowledgementPacket );

  // send error packet
  Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "OACK not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void OperationImpl::invalidPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  [[maybe_unused]] Packets::ConstRawDataSpan rawPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
    << "RX Error: INVALID Packet";

  // send error packet
  Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "Invalid packet not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void OperationImpl::receiveHandler( const boost::system::error_code &errorCode, const std::size_t bytesTransferred )
{
  BOOST_LOG_FUNCTION()

  // operation has been aborted (maybe timeout)
  // error is not handled here.
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    return;
  }

  // (internal) receive error occurred
  if ( errorCode )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "Error when receiving message: " << errorCode.message();

    finished( TransferStatus::CommunicationError );
    return;
  }

  // handle the received packet
  packet( socket.remote_endpoint(), Packets::ConstRawDataSpan{ receivePacket.begin(), bytesTransferred } );
}

void OperationImpl::timeoutHandler( const boost::system::error_code& errorCode )
{
  BOOST_LOG_FUNCTION()

  // wait aborted (packet received)
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    return;
  }

  // internal (timer) error occurred
  if ( errorCode )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "Timer error: " << errorCode.message();

    finished( TransferStatus::CommunicationError );
    return;
  }

  // if maximum retries exceeded -> abort receive operation
  if ( transmitCounter > tftpRetriesV )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "TFTP Retry counter exceeded";

    finished( TransferStatus::CommunicationError );
    return;
  }

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::info )
    << "Retransmit last TFTP packet: "
    << Packets::Packet::packetType( transmitPacket );

  try
  {
    // Update statistic
    Packets::PacketStatistic::globalTransmit().packet(
      Packets::Packet::packetType( transmitPacket ),
      transmitPacket.size() );

    socket.send( boost::asio::buffer( transmitPacket ) );

    ++transmitCounter;

    timer.expires_after( receiveTimeoutV );

    timer.async_wait( std::bind_front( &OperationImpl::timeoutHandler, this ) );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "Re-TX error: " << err.what();

    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::timeoutDallyHandler( const boost::system::error_code &errorCode )
{
  BOOST_LOG_FUNCTION()

  // operation aborted (packet received)
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    return;
  }

  // internal (timer) error occurred
  if ( errorCode )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "Timer error: " << errorCode.message();

    finished( TransferStatus::CommunicationError );
    return;
  }

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::info )
    << "Dally Timeout Completed - Finish";

  finished( TransferStatus::Successful );
}

}
