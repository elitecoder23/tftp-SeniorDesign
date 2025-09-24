// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Clients::OperationImpl.
 **/

#include "OperationImpl.hpp"

#include <tftp/packets/ErrorCodeDescription.hpp>
#include <tftp/packets/PacketStatistic.hpp>
#include <tftp/packets/PacketTypeDescription.hpp>
#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>

#include <tftp/TftpException.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <boost/bind/bind.hpp>

namespace Tftp::Clients {

OperationImpl::OperationImpl( boost::asio::io_context &ioContext ) :
  socketV{ ioContext },
  timerV{ ioContext },
  receivePacketV( Packets::DefaultMaxPacketSize )
{
}

OperationImpl::~OperationImpl() = default;

void OperationImpl::initialise()
{
  try
  {
    // reset remembered connected endpoint
    receiveEndpointV = boost::asio::ip::udp::endpoint{};

    // Open the socket
    socketV.open( remoteV.protocol() );

    // Bind socket to source address (from)
    if ( !localV.address().is_unspecified() )
    {
      socketV.bind( localV );
    }
  }
  catch ( const boost::system::system_error &err )
  {
    SPDLOG_ERROR( "Initialisation Error: {}", err.what() );

    // On error and if socket is opened - close it.
    if ( socketV.is_open() )
    {
      socketV.close();
    }

    // Operation finished
    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::gracefulAbort( const Packets::ErrorCode errorCode, std::string errorMessage )
{
  SPDLOG_WARN(
    "Graceful abort requested: '{}' '{}'",
    Packets::ErrorCodeDescription::instance().name( errorCode ),
    errorMessage );

  const Packets::ErrorPacket errorPacket{ errorCode, std::move( errorMessage ) };

  // skip transmission if not already received something from remote
  if ( receiveEndpointV != boost::asio::ip::udp::endpoint{} )
  {
    send( errorPacket );
  }

  // Operation completed
  finished( TransferStatus::Aborted, errorPacket.errorInformation() );
}

void OperationImpl::abort()
{
  SPDLOG_WARN( "Abort requested" );

  // Operation completed
  finished( TransferStatus::Aborted );
}

const Packets::ErrorInformation& OperationImpl::errorInformation() const
{
  return errorInformationV;
}

void OperationImpl::tftpTimeout( const std::chrono::seconds timeout )
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
  receivePacketV.resize( maxReceivePacketSize );
}

void OperationImpl::receiveTimeout( const std::chrono::seconds receiveTimeout ) noexcept
{
  receiveTimeoutV = receiveTimeout;
}

void OperationImpl::sendFirst( const Packets::Packet &packet )
{
  SPDLOG_TRACE( "TX: {}", static_cast< std::string>( packet ) );

  try
  {
    // Reset transmit counter
    transmitCounterV = 1U;

    // Encode the raw packet
    transmitPacketV = static_cast< Helper::RawData >( packet );

    // Update statistic
    Packets::PacketStatistic::globalTransmit().packet( packet.packetType(), transmitPacketV.size() );

    // Send the packet to the remote server
    socketV.send_to( boost::asio::buffer( transmitPacketV ), remoteV );
  }
  catch ( const boost::system::system_error &err )
  {
    SPDLOG_ERROR( "TX Error: {}", err.what() );

    // Operation finished
    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::send( const Packets::Packet &packet )
{
  SPDLOG_TRACE( "TX: {}", static_cast< std::string>( packet ) );

  try
  {
    // Reset transmit counter
    transmitCounterV = 1U;

    // Encode the raw packet
    transmitPacketV = static_cast< Helper::RawData >( packet );

    // Update statistic
    Packets::PacketStatistic::globalTransmit().packet( packet.packetType(), transmitPacketV.size() );

    // Send the packet to the remote server
    socketV.send( boost::asio::buffer( transmitPacketV ) );
  }
  catch ( const boost::system::system_error &err )
  {
    SPDLOG_ERROR( "TX Error: {}", err.what() );

    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::receiveFirst()
{
  try
  {
    // the first time, we make receive_from (answer is not sent from destination)
    // Start the reception operation
    socketV.async_receive_from(
      boost::asio::buffer( receivePacketV ),
      receiveEndpointV,
      std::bind_front( &OperationImpl::receiveFirstHandler, this ) );

    // Set receive timeout
    timerV.expires_after( receiveTimeoutV );

    // start waiting for receive timeout
    timerV.async_wait( std::bind_front( &OperationImpl::timeoutFirstHandler, this ) );
  }
  catch ( const boost::system::system_error &err )
  {
    SPDLOG_ERROR( "RX Error: {}", err.what() );

    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::receive()
{
  try
  {
    // start the receive operation
    socketV.async_receive(
      boost::asio::buffer( receivePacketV ),
      std::bind_front( &OperationImpl::receiveHandler, this ) );

    // set receive timeout
    timerV.expires_after( receiveTimeoutV );

    // start waiting for receive timeout
    timerV.async_wait( std::bind_front( &OperationImpl::timeoutHandler, this ) );
  }
  catch ( const boost::system::system_error &err )
  {
    SPDLOG_ERROR( "RX Error: {}", err.what() );

    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::receiveDally()
{
  try
  {
    // start the receive operation
    socketV.async_receive(
      boost::asio::buffer( receivePacketV ),
      std::bind_front( &OperationImpl::receiveHandler, this ) );

    // set receive timeout
    timerV.expires_after( 2U * receiveTimeoutV );

    // start waiting for receive timeout
    timerV.async_wait( std::bind_front( &OperationImpl::timeoutDallyHandler, this ) );
  }
  catch ( const boost::system::system_error &err )
  {
    SPDLOG_ERROR( "RX Error: {}", err.what() );

    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::finished( const TransferStatus status, Packets::ErrorInformation errorInformation )
{
  SPDLOG_INFO( "TFTP Client Operation finished" );

  errorInformationV = std::move( errorInformation );

  timerV.cancel();
  socketV.cancel();
  socketV.close();

  if ( completionHandlerV )
  {
    completionHandlerV( status );
  }
}

void OperationImpl::readRequestPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::ReadRequestPacket &readRequestPacket )
{
  SPDLOG_ERROR( "RX Error: {}", static_cast< std::string>( readRequestPacket ) );

  // send error packet
  const Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "RRQ not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, errorPacket.errorInformation() );
}

void OperationImpl::writeRequestPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::WriteRequestPacket &writeRequestPacket )
{
  SPDLOG_ERROR( "RX Error: {}", static_cast< std::string>( writeRequestPacket ) );

  // send error packet
  const Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "WRQ not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, errorPacket.errorInformation() );
}

void OperationImpl::errorPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  const Packets::ErrorPacket &errorPacket )
{
  SPDLOG_ERROR( "RX Error: {}", static_cast< std::string>( errorPacket ) );

  // Operation completed
  switch ( Packets::Packet::packetType( transmitPacketV ) )
  {
    case Packets::PacketType::ReadRequest:
    case Packets::PacketType::WriteRequest:
      switch ( errorPacket.errorCode() )
      {
        case Packets::ErrorCode::TftpOptionRefused:
          // TFTP Option negotiation refused
          finished( TransferStatus::OptionNegotiationError, errorPacket.errorInformation() );
          break;

        default:
          // RRQ/ WRQ response with error
          finished( TransferStatus::RequestError, errorPacket.errorInformation() );
          break;
      }
      break;

    default:
      // error for an other packet
      finished( TransferStatus::TransferError, errorPacket.errorInformation() );
      break;
  }
}

void OperationImpl::invalidPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  [[maybe_unused]] Helper::ConstRawDataSpan rawPacket )
{
  SPDLOG_ERROR( "RX Error: INVALID Packet" );

  // send error packet
  const Packets::ErrorPacket errorPacket{ Packets::ErrorCode::IllegalTftpOperation, "Invalid packet isn't expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, errorPacket.errorInformation() );
}

void OperationImpl::receiveFirstHandler(
  const boost::system::error_code &errorCode,
  const std::size_t bytesTransferred )
{
  // operation has been aborted (maybe timeout)
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    return;
  }

  // (internal) receive error occurred
  if ( errorCode )
  {
    SPDLOG_ERROR( "Error when receiving message: {}", errorCode.message() );

    finished( TransferStatus::CommunicationError );
    return;
  }

  // check if the packet has been received from a not expected source
  // send an error packet and ignore it.
  if ( remoteV.address() != receiveEndpointV.address() )
  {
    SPDLOG_ERROR( "Received packed from wrong source: {}", receiveEndpointV.address().to_string() );

    // sent an error packet to the unknown partner
    try
    {
      // send error packet
      const Packets::ErrorPacket err{ Packets::ErrorCode::UnknownTransferId, "Packet from wrong source" };

      auto rawPacket{ static_cast< Helper::RawData>( err ) };

      // Update statistic
      Packets::PacketStatistic::globalTransmit().packet( err.packetType(), rawPacket.size() );

      socketV.send_to( boost::asio::buffer( rawPacket ), receiveEndpointV );
    }
    catch ( const boost::system::system_error &err)
    {
      // ignore send error to unknown partner
      SPDLOG_ERROR( "Error sending ERR packet: {}", err.what() );
    }

    // restart receive operation
    try
    {
      socketV.async_receive_from(
        boost::asio::buffer( receivePacketV ),
        receiveEndpointV,
        std::bind_front( &OperationImpl::receiveFirstHandler, this ) );

      return;
    }
    catch ( const boost::system::system_error &err )
    {
      SPDLOG_ERROR( "Start Receive: {}", err.what() );

      finished( TransferStatus::CommunicationError );
      return;
    }
  }

  try
  {
    // connect to the server port
    socketV.connect( receiveEndpointV );
  }
  catch ( const boost::system::system_error &err )
  {
    SPDLOG_ERROR( "Connect: {}", err.what() );

    finished( TransferStatus::CommunicationError );
    return;
  }

  packet( receiveEndpointV, Helper::ConstRawDataSpan{ receivePacketV.begin(), bytesTransferred } );
}

void OperationImpl::receiveHandler( const boost::system::error_code &errorCode, const std::size_t bytesTransferred )
{
  // operation has been aborted (maybe timeout)
  // error is not handled here.
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    return;
  }

  // (internal) receive error occurred
  if ( errorCode )
  {
    SPDLOG_ERROR( "Error when receiving message: {}", errorCode.message() );

    finished( TransferStatus::CommunicationError );
    return;
  }

  // handle the received packet
  packet( receiveEndpointV, Helper::ConstRawDataSpan{ receivePacketV.begin(), bytesTransferred } );
}

void OperationImpl::timeoutFirstHandler( const boost::system::error_code &errorCode )
{
  // wait aborted (a packet has been received)
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    return;
  }

  // internal (timer) error occurred
  if ( errorCode )
  {
    SPDLOG_ERROR( "Timer error: {}", errorCode.message() );

    finished( TransferStatus::CommunicationError );
    return;
  }

  // if maximum retries exceeded -> abort receive operation
  if ( transmitCounterV > tftpRetriesV )
  {
    SPDLOG_ERROR( "TFTP Retry counter exceeded" );

    finished( TransferStatus::CommunicationError );
    return;
  }

  SPDLOG_WARN(
    "Retransmit last TFTP packet: {}",
    Packets::PacketTypeDescription::instance().name( Packets::Packet::packetType( transmitPacketV ) ) );

  try
  {
    // Update statistic
    Packets::PacketStatistic::globalTransmit().packet(
      Packets::Packet::packetType( transmitPacketV ),
      transmitPacketV.size() );

    // resent stored packet
    socketV.send_to( boost::asio::buffer( transmitPacketV ), remoteV );

    // increment transmit counter
    ++transmitCounterV;

    timerV.expires_after( receiveTimeoutV );

    timerV.async_wait( std::bind_front( &OperationImpl::timeoutFirstHandler, this ) );
  }
  catch ( const boost::system::system_error &err )
  {
    SPDLOG_ERROR( "Re-TX error: {}", err.what() );

    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::timeoutHandler( const boost::system::error_code& errorCode )
{
  // wait aborted (a packet has been received)
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    return;
  }

  // internal (timer) error occurred
  if ( errorCode )
  {
    SPDLOG_ERROR( "Timer error: {}", errorCode.message() );

    finished( TransferStatus::CommunicationError );
    return;
  }

  // if maximum retries exceeded -> abort receive operation
  if ( transmitCounterV > tftpRetriesV )
  {
    SPDLOG_ERROR( "TFTP Retry counter exceeded" );

    finished( TransferStatus::CommunicationError );
    return;
  }

  SPDLOG_INFO(
    "Retransmit last TFTP packet: {}",
    Packets::PacketTypeDescription::instance().name( Packets::Packet::packetType( transmitPacketV ) ) );

  try
  {
    // Update statistic
    Packets::PacketStatistic::globalTransmit().packet(
      Packets::Packet::packetType( transmitPacketV ),
      transmitPacketV.size() );

    socketV.send( boost::asio::buffer( transmitPacketV ) );

    ++transmitCounterV;

    timerV.expires_after( receiveTimeoutV );

    timerV.async_wait( std::bind_front( &OperationImpl::timeoutHandler, this ) );
  }
  catch ( const boost::system::system_error &err )
  {
    SPDLOG_ERROR( "Re-TX error: {}", err.what() );

    finished( TransferStatus::CommunicationError );
  }
}

void OperationImpl::timeoutDallyHandler( const boost::system::error_code &errorCode )
{
  // operation aborted (a packet has been received)
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    return;
  }

  // internal (timer) error occurred
  if ( errorCode )
  {
    SPDLOG_ERROR( "Timer error: {}", errorCode.message() );

    finished( TransferStatus::CommunicationError );
    return;
  }

  SPDLOG_INFO( "Dally Timeout Completed - Finish" );

  finished( TransferStatus::Successful );
}

}
