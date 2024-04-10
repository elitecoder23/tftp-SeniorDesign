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
 * @brief Definition of Class Tftp::Client::OperationImpl.
 **/

#include "OperationImpl.hpp"

#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>
#include <tftp/packets/ErrorCodeDescription.hpp>
#include <tftp/packets/PacketStatistic.hpp>
#include <tftp/packets/PacketTypeDescription.hpp>

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>

#include <helper/SafeCast.hpp>
#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

#include <boost/bind/bind.hpp>

namespace Tftp::Client {

OperationImpl::~OperationImpl() = default;

void OperationImpl::gracefulAbort(
  const Packets::ErrorCode errorCode,
  std::string errorMessage )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::warning )
    << "Graceful abort requested: "
    << "'" << errorCode << "' '" << errorMessage << "'";

  Packets::ErrorPacket errorPacket{ errorCode, std::move( errorMessage ) };

  // skip transmission if not already received something from remote
  if ( receiveEndpoint != boost::asio::ip::udp::endpoint{} )
  {
    send( errorPacket );
  }

  // Operation completed
  finished( TransferStatus::Aborted, std::move( errorPacket ) );
}

void OperationImpl::abort()
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::warning )
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
  boost::asio::ip::udp::endpoint remote,
  const std::optional< boost::asio::ip::udp::endpoint > &local )
try :
  receiveTimeoutV{ tftpTimeout },
  tftpRetries{ tftpRetries },
  completionHandler{ std::move( completionHandler ) },
  remoteEndpoint{ std::move( remote ) },
  socket{ ioContext },
  timer{ ioContext },
  receivePacket( maxReceivePacketSize )
{
  BOOST_LOG_FUNCTION()

  try
  {
    // Open the socket
    socket.open( remoteEndpoint.protocol() );

    // Bind socket to source address (from)
    if ( local )
    {
      socket.bind( *local );
    }
  }
  catch ( const boost::system::system_error &err )
  {
    // On error and if socket is opened - close it.
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

void OperationImpl::sendFirst( const Packets::Packet &packet )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::trace )
    << "TX: " << static_cast< std::string>( packet );

  try
  {
    // Reset transmit counter
    transmitCounter = 1U;

    // Encode raw packet
    transmitPacket = static_cast< Packets::RawTftpPacket >( packet );

    // Update statistic
    Packets::PacketStatistic::globalTransmit().packet(
      packet.packetType(),
      transmitPacket.size() );

    // Send the packet to the remote server
    socket.send_to(
      boost::asio::buffer( transmitPacket ),
      remoteEndpoint );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "TX Error: " << err.what();

    // Operation finished
    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::send( const Packets::Packet &packet )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::trace )
    << "TX: " << static_cast< std::string>( packet );

  try
  {
    // Reset transmit counter
    transmitCounter = 1U;

    // Encode raw packet
    transmitPacket = static_cast< Packets::RawTftpPacket>( packet );

    // Update statistic
    Packets::PacketStatistic::globalTransmit().packet(
      packet.packetType(),
      transmitPacket.size() );

    // Send the packet to the remote server
    socket.send( boost::asio::buffer( transmitPacket ) );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "TX Error: " << err.what();

    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::receiveFirst()
{
  BOOST_LOG_FUNCTION()

  try
  {
    // the first time, we make receive_from (answer is not sent from destination)
    // Start the reception operation
    socket.async_receive_from(
      boost::asio::buffer( receivePacket ),
      receiveEndpoint,
      std::bind_front( &OperationImpl::receiveFirstHandler, this ) );

    // Set receive timeout
    timer.expires_after( receiveTimeoutV );

    // start waiting for receive timeout
    timer.async_wait( std::bind_front(
      &OperationImpl::timeoutFirstHandler,
      this ) );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "RX Error: " << err.what();

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
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
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
    timer.async_wait(
      std::bind_front( &OperationImpl::timeoutDallyHandler, this ) );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "RX Error: " << err.what();

    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::receiveTimeout(
  const std::chrono::seconds receiveTimeout ) noexcept
{
  receiveTimeoutV = receiveTimeout;
}

void OperationImpl::finished(
  const TransferStatus status,
  ErrorInfo &&errorInfo )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "TFTP Client Operation finished";

  errorInfoV = std::move( errorInfo );

  timer.cancel();
  socket.cancel();

  if ( completionHandler )
  {
    completionHandler( shared_from_this(), status );
  }
}

void OperationImpl::readRequestPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::ReadRequestPacket &readRequestPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
    << "RX ERROR: " << static_cast< std::string>( readRequestPacket );

  // send error packet
  Packets::ErrorPacket errorPacket{
    Packets::ErrorCode::IllegalTftpOperation,
    "RRQ not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void OperationImpl::writeRequestPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::WriteRequestPacket &writeRequestPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
    << "RX ERROR: " << static_cast< std::string>( writeRequestPacket );

  // send error packet
  Packets::ErrorPacket errorPacket{
    Packets::ErrorCode::IllegalTftpOperation,
    "WRQ not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void OperationImpl::errorPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::ErrorPacket &errorPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
    << "RX ERROR: " << static_cast< std::string>( errorPacket );

  // Operation completed
  switch ( Packets::Packet::packetType( transmitPacket ) )
  {
    case Packets::PacketType::ReadRequest:
    case Packets::PacketType::WriteRequest:
      switch ( errorPacket.errorCode() )
      {
        case Packets::ErrorCode::TftpOptionRefused:
          // TFTP Option negotiation refused
          finished( TransferStatus::OptionNegotiationError, errorPacket );
          break;

        default:
          // RRQ/ WRQ response with error
          finished( TransferStatus::RequestError, errorPacket );
          break;
      }
      break;

    default:
      // error for other package
      finished( TransferStatus::TransferError, errorPacket );
      break;
  }
}

void OperationImpl::invalidPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  [[maybe_unused]] Packets::ConstRawTftpPacketSpan rawPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
    << "RX ERROR: INVALID Packet";

  // send error packet
  Packets::ErrorPacket errorPacket{
    Packets::ErrorCode::IllegalTftpOperation,
    "Invalid packet not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, errorPacket );
}

void OperationImpl::receiveFirstHandler(
  const boost::system::error_code& errorCode,
  std::size_t bytesTransferred )
{
  BOOST_LOG_FUNCTION()

  // operation has been aborted (maybe timeout)
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    return;
  }

  // (internal) receive error occurred
  if ( errorCode )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Error when receiving message: " << errorCode.message();

    finished( TransferStatus::CommunicationError );
    return;
  }

  // check, if packet has been received from not expected source
  // send error packet and ignore it.
  if ( remoteEndpoint.address() != receiveEndpoint.address() )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Received packed from wrong source: "
      << receiveEndpoint.address();

    // sent Error packet to unknown partner
    try
    {
      // send error packet
      const Packets::ErrorPacket err{
        Packets::ErrorCode::UnknownTransferId,
        "Packet from wrong source" };

      auto rawPacket{ static_cast< Packets::RawTftpPacket>( err ) };

      // Update statistic
      Packets::PacketStatistic::globalTransmit().packet(
        err.packetType(),
        rawPacket.size() );

      socket.send_to( boost::asio::buffer( rawPacket ), receiveEndpoint );
    }
    catch ( const boost::system::system_error &err)
    {
      // ignore send error to unknown partner
      BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
        << "Error sending ERR packet: " << err.what();
    }

    // restart receive operation
    try
    {
      socket.async_receive_from(
        boost::asio::buffer( receivePacket ),
        receiveEndpoint,
        std::bind_front( &OperationImpl::receiveFirstHandler, this ) );

      return;
    }
    catch ( const boost::system::system_error &err )
    {
      BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
        << "Start Receive: " << err.what();

      finished( TransferStatus::CommunicationError );
      return;
    }
  }

  try
  {
    // connect to the server port
    socket.connect( receiveEndpoint );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Connect: " << err.what();

    finished( TransferStatus::CommunicationError );
    return;
  }

  packet(
    receiveEndpoint,
    Packets::ConstRawTftpPacketSpan{ receivePacket.begin(), bytesTransferred } );
}

void OperationImpl::receiveHandler(
  const boost::system::error_code& errorCode,
  const std::size_t bytesTransferred )
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
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Error when receiving message: " << errorCode.message();

    finished( TransferStatus::CommunicationError );
    return;
  }

  packet(
    receiveEndpoint,
    Packets::ConstRawTftpPacketSpan{ receivePacket.begin(), bytesTransferred } );
}

void OperationImpl::timeoutFirstHandler(
  const boost::system::error_code& errorCode )
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
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Timer error: " + errorCode.message();

    finished( TransferStatus::CommunicationError );
    return;
  }

  // if maximum retries exceeded -> abort receive operation
  if ( transmitCounter > tftpRetries )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "TFTP Retry counter exceeded";

    finished( TransferStatus::CommunicationError );
    return;
  }

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::warning )
    << "Retransmit last TFTP packet: "
    << Packets::Packet::packetType( transmitPacket );

  try
  {
    // Update statistic
    Packets::PacketStatistic::globalTransmit().packet(
      Packets::Packet::packetType( transmitPacket ),
      transmitPacket.size() );

    // resent stored packet
    socket.send_to( boost::asio::buffer( transmitPacket ), remoteEndpoint );

    // increment transmit counter
    ++transmitCounter;

    timer.expires_after( receiveTimeoutV );

    timer.async_wait(
      std::bind_front( &OperationImpl::timeoutFirstHandler, this ) );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Re-TX error: " << err.what();

    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::timeoutHandler(
  const boost::system::error_code& errorCode )
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
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Timer error: " << errorCode.message();

    finished( TransferStatus::CommunicationError );
    return;
  }

  // if maximum retries exceeded -> abort receive operation
  if ( transmitCounter > tftpRetries )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "TFTP Retry counter exceeded";

    finished( TransferStatus::CommunicationError );
    return;
  }

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
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
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Re-TX error: " << err.what();

    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::timeoutDallyHandler(
  const boost::system::error_code &errorCode )
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
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Timer error: " << errorCode.message();

    finished( TransferStatus::CommunicationError );
    return;
  }

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "Dally Timeout Completed - Finish";

  finished( TransferStatus::Successful );
}

}
