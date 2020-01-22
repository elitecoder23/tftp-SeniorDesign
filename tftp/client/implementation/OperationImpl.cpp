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

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>
#include <tftp/ErrorCodeDescription.hpp>

#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>
#include <tftp/packets/ErrorPacket.hpp>

#include <boost/bind.hpp>

namespace Tftp::Client {

OperationImpl::~OperationImpl() noexcept
{
  BOOST_LOG_FUNCTION()
}

void OperationImpl::gracefulAbort(
  const ErrorCode errorCode,
  std::string_view errorMessage)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::warning)
    << "Graceful abort requested: " << errorCode << " '" << errorMessage << "'";

  Packets::ErrorPacket errorPacket{ errorCode, errorMessage};

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::Aborted, std::move( errorPacket));
}

void OperationImpl::abort()
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::warning)
    << "Abort requested";

  // Operation completed
  finished( TransferStatus::Aborted);
}

const OperationImpl::ErrorInfo& OperationImpl::errorInfo() const
{
  return errorInfoV;
}

OperationImpl::OperationImpl(
  boost::asio::io_context &ioContext,
  const uint8_t tftpTimeout,
  const uint16_t tftpRetries,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote)
try :
  completionHandler{ completionHandler},
  remoteEndpoint{ remote},
  maxReceivePacketSizeV{ DefaultMaxPacketSize},
  receiveTimeoutV{ tftpTimeout},
  tftpRetries{ tftpRetries},
  socket{ ioContext},
  timer{ ioContext},
  transmitPacketType{ PacketType::Invalid},
  transmitCounter{ 0U}
{
  BOOST_LOG_FUNCTION()

  try
  {
    // Open the socket
    socket.open( remote.protocol());
  }
  catch ( boost::system::system_error &err)
  {
    // On error and if socket is opened - close it.
    if ( socket.is_open())
    {
      socket.close();
    }

    BOOST_THROW_EXCEPTION( CommunicationException()
      << Helper::AdditionalInfo( err.what()));
  }
}
catch ( boost::system::system_error &err)
{
  BOOST_THROW_EXCEPTION( CommunicationException()
    << Helper::AdditionalInfo( err.what()));
}

OperationImpl::OperationImpl(
  boost::asio::io_context &ioContext,
  const uint8_t tftpTimeout,
  const uint16_t tftpRetries,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  const boost::asio::ip::udp::endpoint &local)
try :
  completionHandler{ completionHandler},
  remoteEndpoint{ remote},
  maxReceivePacketSizeV{ DefaultMaxPacketSize},
  receiveTimeoutV{ tftpTimeout},
  tftpRetries{ tftpRetries},
  socket{ ioContext},
  timer{ ioContext},
  transmitPacketType{ PacketType::Invalid},
  transmitCounter{ 0U}
{
  BOOST_LOG_FUNCTION()

  try
  {
    // Open the socket
    socket.open( remote.protocol());

    // Bind socket to source address (from)
    socket.bind( local);
  }
  catch ( boost::system::system_error &err)
  {
    // On error and if socket is opened - close it.
    if ( socket.is_open())
    {
      socket.close();
    }

    BOOST_THROW_EXCEPTION( CommunicationException()
      << Helper::AdditionalInfo( err.what()));
  }
}
catch ( boost::system::system_error &err)
{
  BOOST_THROW_EXCEPTION( CommunicationException()
   << Helper::AdditionalInfo( err.what()));
}

void OperationImpl::sendFirst( const Packets::Packet &packet)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info)
    << "TX: " << static_cast< std::string>( packet);

  try
  {
    // Reset the transmit counter
    transmitCounter = 1U;

    // Store packet type
    transmitPacketType = packet.packetType();

    // Encode raw packet
    transmitPacket = static_cast< RawTftpPacket>( packet);

    // Send the packet to the remote server
    socket.send_to(
      boost::asio::buffer( transmitPacket),
      remoteEndpoint);
  }
  catch (boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "TX Error: " << err.what();

    // Operation finished
    finished( TransferStatus::CommunicationError);
  }
}

void OperationImpl::send( const Packets::Packet &packet)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info)
    << "TX: " << static_cast< std::string>( packet);

  try
  {
    // Reset the transmit counter
    transmitCounter = 1U;

    // Store packet type
    transmitPacketType = packet.packetType();

    // Encode raw packet
    transmitPacket = static_cast< RawTftpPacket>( packet);

    // Send the packet to the remote server
    socket.send( boost::asio::buffer( transmitPacket));
  }
  catch (boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "TX Error: " << err.what();

    finished( TransferStatus::CommunicationError);
  }
}

void OperationImpl::receiveFirst()
{
  BOOST_LOG_FUNCTION()

  try
  {
    // Resize the receive buffer to the allowed packet size
    receivePacket.resize( maxReceivePacketSizeV);

    // the first time, we make receive_from (answer is not sent from destination)
    // Start the receive operation
    socket.async_receive_from(
      boost::asio::buffer( receivePacket),
      receiveEndpoint,
      boost::bind(
        &OperationImpl::receiveFirstHandler,
        shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));

    // Set receive timeout
    timer.expires_from_now( boost::posix_time::seconds{ receiveTimeoutV});

    // start waiting for receive timeout
    timer.async_wait( boost::bind(
      &OperationImpl::timeoutFirstHandler,
      shared_from_this(),
      boost::asio::placeholders::error));
  }
  catch (boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "RX Error: " << err.what();

    finished( TransferStatus::CommunicationError);
  }
}

void OperationImpl::receive()
{
  BOOST_LOG_FUNCTION()

  try
  {
    receivePacket.resize( maxReceivePacketSizeV);

    // start receive operation
    socket.async_receive(
      boost::asio::buffer( receivePacket),
      boost::bind(
        &OperationImpl::receiveHandler,
        shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));

    // set receive timeout
    timer.expires_from_now( boost::posix_time::seconds{ receiveTimeoutV});

    // start waiting for receive timeout
    timer.async_wait( boost::bind(
      &OperationImpl::timeoutHandler,
      shared_from_this(),
      boost::asio::placeholders::error));
  }
  catch (boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "RX Error: " << err.what();

    finished( TransferStatus::CommunicationError);
  }
}

void OperationImpl::maxReceivePacketSize(
  const uint16_t maxReceivePacketSize) noexcept
{
  maxReceivePacketSizeV = maxReceivePacketSize;
}

void OperationImpl::receiveTimeout( const uint8_t receiveTimeout) noexcept
{
  receiveTimeoutV = receiveTimeout;
}

void OperationImpl::finished(
  const TransferStatus status,
  ErrorInfo &&errorInfo) noexcept
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info)
    << "Operation finished";

  errorInfoV = std::move( errorInfo);

  timer.cancel();
  socket.cancel();

  if (completionHandler)
  {
    completionHandler( status);
  }
}

void OperationImpl::readRequestPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::ReadRequestPacket &readRequestPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
    << "RX ERROR: " << static_cast< std::string>( readRequestPacket);

  // send error packet
  Packets::ErrorPacket errorPacket{
    ErrorCode::IllegalTftpOperation,
    "RRQ not expected"};

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket));
}

void OperationImpl::writeRequestPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::WriteRequestPacket &writeRequestPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
    << "RX ERROR: " << static_cast< std::string>( writeRequestPacket);

  // send error packet
  Packets::ErrorPacket errorPacket{
    ErrorCode::IllegalTftpOperation,
    "WRQ not expected"};

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket));
}

void OperationImpl::errorPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::ErrorPacket &errorPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
    << "RX ERROR: " << static_cast< std::string>( errorPacket);

  // Operation completed
  switch ( transmitPacketType)
  {
    case PacketType::ReadRequest:
    case PacketType::WriteRequest:
      switch (errorPacket.errorCode())
      {
        case ErrorCode::TftpOptionRefused:
          finished( TransferStatus::OptionNegotiationError, errorPacket);
          break;

        default:
          finished( TransferStatus::RequestError, errorPacket);
          break;
      }
      break;

    default:
      finished( TransferStatus::TransferError, errorPacket);
      break;
  }
}

void OperationImpl::invalidPacket(
  const boost::asio::ip::udp::endpoint &,
  const RawTftpPacket &)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
    << "RX ERROR: INVALID Packet";

  // send error packet
  Packets::ErrorPacket errorPacket{
    ErrorCode::IllegalTftpOperation,
    "Invalid packet not expected"};

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, errorPacket);
}

void OperationImpl::receiveFirstHandler(
  const boost::system::error_code& errorCode,
  std::size_t bytesTransferred)
{
  BOOST_LOG_FUNCTION()

  // operation has been aborted (maybe timeout)
  if (boost::asio::error::operation_aborted == errorCode)
  {
    return;
  }

  // (internal) receive error occurred
  if (errorCode)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "Error when receiving message: " << errorCode.message();

    finished( TransferStatus::CommunicationError);
    return;
  }

  // check, if packet has been received from not expected source
  // send error packet and ignore it.
  if (remoteEndpoint.address() != receiveEndpoint.address())
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "Received packed from wrong source: "
      << receiveEndpoint.address().to_string();

    // sent Error packet to unknown partner
    try
    {
      // send error packet
      Packets::ErrorPacket err{
        ErrorCode::UnknownTransferId,
        "Packet from wrong source"};

      socket.send_to(
        boost::asio::buffer( static_cast< RawTftpPacket>( err)),
        receiveEndpoint);
    }
    catch ( boost::system::system_error &err)
    {
      // ignore send error to unknown partner
      BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
        << "Error sending ERR packet: " << err.what();
    }

    // restart receive operation
    try
    {
      socket.async_receive_from(
        boost::asio::buffer( receivePacket),
        receiveEndpoint,
        boost::bind(
          &OperationImpl::receiveFirstHandler,
          shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));

      return;
    }
    catch ( boost::system::system_error &err)
    {
      BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
        << "Start Receive: " << err.what();

      finished( TransferStatus::CommunicationError);
      return;
    }
  }

  // store real end point
  remoteEndpoint = receiveEndpoint;

  try
  {
    // connect to the server port
    socket.connect( receiveEndpoint);
  }
  catch ( boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "Connect: " << err.what();

    finished( TransferStatus::CommunicationError);
    return;
  }

  receivePacket.resize( bytesTransferred);

  packet( receiveEndpoint, receivePacket);
}

void OperationImpl::receiveHandler(
  const boost::system::error_code& errorCode,
  const std::size_t bytesTransferred)
{
  BOOST_LOG_FUNCTION()

  // operation has been aborted (maybe timeout)
  // error is not handled here.
  if (boost::asio::error::operation_aborted == errorCode)
  {
    return;
  }

  // (internal) receive error occurred
  if (errorCode)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "Error when receiving message: " << errorCode.message();

    finished( TransferStatus::CommunicationError);
    return;
  }

  receivePacket.resize( bytesTransferred);

  packet( remoteEndpoint, receivePacket);
}

void OperationImpl::timeoutFirstHandler(
  const boost::system::error_code& errorCode)
{
  BOOST_LOG_FUNCTION()

  // operation aborted (packet received)
  if (boost::asio::error::operation_aborted == errorCode)
  {
    return;
  }

  // internal (timer) error occurred
  if (errorCode)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "timer error: " + errorCode.message();

    finished( TransferStatus::CommunicationError);
    return;
  }

  // if maximum retries exceeded -> abort receive operation
  if ( transmitCounter > tftpRetries)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "Retry counter exceeded ABORT";

    finished( TransferStatus::CommunicationError);
    return;
  }

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info)
    << "retransmit last packet";

  try
  {
    // resent stored packet
    socket.send_to( boost::asio::buffer( transmitPacket), remoteEndpoint);

    // increment transmit counter
    ++transmitCounter;

    timer.expires_from_now( boost::posix_time::seconds( receiveTimeoutV));

    timer.async_wait( boost::bind(
      &OperationImpl::timeoutFirstHandler,
      shared_from_this(),
      boost::asio::placeholders::error));
  }
  catch (boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "Re-TX error: " << err.what();

    finished( TransferStatus::CommunicationError);
  }
}

void OperationImpl::timeoutHandler(
  const boost::system::error_code& errorCode)
{
  BOOST_LOG_FUNCTION()

  // operation aborted (packet received)
  if (boost::asio::error::operation_aborted == errorCode)
  {
    return;
  }

  // internal (timer) error occurred
  if (errorCode)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "timer error: " << errorCode.message();

    finished( TransferStatus::CommunicationError);
    return;
  }

  // if maximum retries exceeded -> abort receive operation
  if (transmitCounter > tftpRetries)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "Retry counter exceeded ABORT";

    finished( TransferStatus::CommunicationError);
    return;
  }

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info)
    << "retransmit last packet";

  try
  {
    socket.send( boost::asio::buffer( transmitPacket));

    ++transmitCounter;

    timer.expires_from_now( boost::posix_time::seconds( receiveTimeoutV));

    timer.async_wait( boost::bind(
      &OperationImpl::timeoutHandler,
      shared_from_this(),
      boost::asio::placeholders::error));
  }
  catch (boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "Re-TX error: " << err.what();

    finished( TransferStatus::CommunicationError);
  }
}

}
