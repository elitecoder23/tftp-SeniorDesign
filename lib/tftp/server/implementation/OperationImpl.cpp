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
 * @brief Definition of Class Tftp::Server::OperationImpl.
 **/

#include "OperationImpl.hpp"

#include <tftp/packets/ErrorCodeDescription.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>
#include <tftp/packets/PacketStatistic.hpp>
#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>

#include <tftp/Logger.hpp>
#include <tftp/TftpException.hpp>
#include <tftp/TftpOptionsConfiguration.hpp>

#include <helper/Exception.hpp>
#include <helper/SafeCast.hpp>

#include <boost/exception/all.hpp>

#include <boost/bind/bind.hpp>

namespace Tftp::Server {

void OperationImpl::gracefulAbort(
  const Packets::ErrorCode errorCode,
  std::string errorMessage )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::warning )
    << "Graceful abort requested: " << errorCode << " '" << errorMessage << "'";

  Packets::ErrorPacket errorPacket{
    errorCode,
    std::move( errorMessage ) };

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

const OperationImpl::ErrorInfo& OperationImpl::errorInfo() const
{
  return errorInfoV;
}

OperationImpl::OperationImpl(
  boost::asio::io_context &ioContext,
  const std::chrono::seconds tftpTimeout,
  const uint16_t tftpRetries,
  const uint16_t maxReceivePacketSize,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  const std::optional< boost::asio::ip::udp::endpoint > &local )
try:
  completionHandler{ std::move( completionHandler ) },
  receiveTimeoutV{ tftpTimeout },
  tftpRetries{ tftpRetries },
  socket{ ioContext },
  timer{ ioContext },
  receivePacket( maxReceivePacketSize )
{
  try
  {
    // Open the socket
    socket.open( remote.protocol() );

    // bind to local address
    if ( local )
    {
      socket.bind( *local );
    }
    // connect to client.
    socket.connect( remote );
  }
  catch ( const boost::system::system_error &err )
  {
    if ( socket.is_open() )
    {
      socket.close();
    }

    BOOST_THROW_EXCEPTION( CommunicationException()
      << Helper::AdditionalInfo{ err.what() } );
  }
}
catch ( const boost::system::system_error &err )
{
  BOOST_THROW_EXCEPTION( CommunicationException()
    << Helper::AdditionalInfo{ err.what() } );
}

OperationImpl::~OperationImpl() = default;

void OperationImpl::finished(
  const TransferStatus status,
  ErrorInfo &&errorInfo )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::info )
    << "TFTP Server Operation finished";

  errorInfoV = std::move( errorInfo );

  timer.cancel();
  socket.cancel();

  if ( completionHandler )
  {
    completionHandler( shared_from_this(), status );
  }
}

void OperationImpl::send( const Packets::Packet &packet )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::trace )
    << "TX: " << static_cast< std::string>( packet );

  try
  {
    // Reset the transmit-counter
    transmitCounter = 1U;

    // Encode raw packet
    transmitPacket = static_cast< Packets::RawTftpPacket>( packet );

    // Update statistic
    Packets::PacketStatistic::globalTransmit().packet(
      packet.packetType(),
      transmitPacket.size() );

    socket.send( boost::asio::buffer( transmitPacket ) );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "TX ERROR: " << err.what();

    // Operation completed
    finished( TransferStatus::CommunicationError );
    return;
  }
}

void OperationImpl::receive()
{
  BOOST_LOG_FUNCTION()

  try
  {
    socket.async_receive(
      boost::asio::buffer( receivePacket),
      std::bind_front( &OperationImpl::receiveHandler, this ) );

    timer.expires_after( receiveTimeoutV );

    timer.async_wait( std::bind_front( &OperationImpl::timeoutHandler, this ) );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "RX ERROR: " << err.what();

    // Operation completed
    finished( TransferStatus::CommunicationError );
    return;
  }
}

void OperationImpl::receiveDally()
{
  BOOST_LOG_FUNCTION()

  try
  {
    socket.async_receive(
      boost::asio::buffer( receivePacket ),
      std::bind_front( &OperationImpl::receiveHandler, this ) );

    timer.expires_after( 2U * receiveTimeoutV );

    timer.async_wait(
      std::bind_front( &OperationImpl::timeoutDallyHandler, this ) );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "RX ERROR: " << err.what();

    // Operation completed
    finished( TransferStatus::CommunicationError );
    return;
  }
}

void OperationImpl::receiveTimeout(
  const std::chrono::seconds receiveTimeout ) noexcept
{
  receiveTimeoutV = receiveTimeout;
}

void OperationImpl::readRequestPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::ReadRequestPacket &readRequestPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
    << "RX ERROR: " << static_cast< std::string>( readRequestPacket );

  Packets::ErrorPacket errorPacket{
    Packets::ErrorCode::IllegalTftpOperation,
    "RRQ not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void OperationImpl::writeRequestPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::WriteRequestPacket &writeRequestPacket)
{
  BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
    << "RX ERROR: " << static_cast< std::string>( writeRequestPacket );

  Packets::ErrorPacket errorPacket{
    Packets::ErrorCode::IllegalTftpOperation,
    "WRQ not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket) );
}

void OperationImpl::errorPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::ErrorPacket &errorPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
    << "RX ERROR: " << static_cast< std::string>( errorPacket );

  // Operation completed
  switch ( Packets::Packet::packetType( transmitPacket ) )
  {
    case Packets::PacketType::OptionsAcknowledgement:
      switch ( errorPacket.errorCode() )
      {
        case Packets::ErrorCode::TftpOptionRefused:
          finished( TransferStatus::OptionNegotiationError, errorPacket );
          break;

        default:
          finished( TransferStatus::TransferError,  errorPacket );
          break;
      }
      break;

    default:
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
    << "RX ERROR: " << static_cast< std::string>( optionsAcknowledgementPacket );

  Packets::ErrorPacket errorPacket{
    Packets::ErrorCode::IllegalTftpOperation,
    "OACK not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void OperationImpl::invalidPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  [[maybe_unused]] Packets::ConstRawTftpPacketSpan rawPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
    << "RX: UNKNOWN";

  Packets::ErrorPacket errorPacket{
    Packets::ErrorCode::IllegalTftpOperation,
    "Invalid packet not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void OperationImpl::receiveHandler(
  const boost::system::error_code& errorCode,
  std::size_t bytesTransferred )
{
  BOOST_LOG_FUNCTION()

  // handle abort
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    return;
  }

  // Check error
  if ( errorCode )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "receive error: " << errorCode.message();

    // Operation completed
    finished( TransferStatus::CommunicationError );
    return;
  }

  // handle the received packet
  packet(
    socket.remote_endpoint(),
    Packets::ConstRawTftpPacketSpan{ receivePacket.begin(), bytesTransferred } );
}

void OperationImpl::timeoutHandler( const boost::system::error_code& errorCode )
{
  BOOST_LOG_FUNCTION()

  // handle abort
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    return;
  }

  if ( errorCode )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "timer error: " << errorCode.message();

    // Operation completed
    finished( TransferStatus::CommunicationError );
    return;
  }

  if ( transmitCounter > tftpRetries )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "Retry counter exceeded ABORT";

    // Operation completed
    finished( TransferStatus::CommunicationError );
    return;
  }

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::warning )
    << "retransmit last packet";

  try
  {
    socket.send( boost::asio::buffer( transmitPacket ) );

    ++transmitCounter;

    timer.expires_after( receiveTimeoutV );

    timer.async_wait( std::bind_front( &OperationImpl::timeoutHandler, this ) );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "TX error: " << err.what();

    // Operation completed
    finished( TransferStatus::CommunicationError );
    return;
  }
}

void OperationImpl::timeoutDallyHandler(
  const boost::system::error_code& errorCode )
{
  BOOST_LOG_FUNCTION()

  // handle abort
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    return;
  }

  if ( errorCode )
  {
    BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
      << "timer error: " << errorCode.message();

    // Operation completed
    finished( TransferStatus::CommunicationError );
    return;
  }

  BOOST_LOG_SEV( Logger::get(), Helper::Severity::info )
    << "Dally Timeout Completed - Finish";

  finished( TransferStatus::Successful );
}

}
