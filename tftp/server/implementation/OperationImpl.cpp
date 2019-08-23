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

#include <tftp/TftpLogger.hpp>
#include <tftp/TftpException.hpp>
#include <tftp/TftpConfiguration.hpp>
#include <tftp/ErrorCodeDescription.hpp>

#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>
#include <tftp/packets/ErrorPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>

#include <tftp/server/implementation/TftpServerInternal.hpp>

#include <boost/bind.hpp>

namespace Tftp::Server {

void OperationImpl::start()
{
  // start first receive operation
  receive();
}

void OperationImpl::gracefulAbort(
  const ErrorCode errorCode,
  std::string &&errorMessage)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning)
    << "Graceful abort requested: " << errorCode << " '" << errorMessage << "'";

  Packets::ErrorPacket errorPacket(
    errorCode,
    std::move( errorMessage));

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::Aborted, std::move( errorPacket));
}

void OperationImpl::abort()
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning) <<
    "Abort requested";

  // Operation completed
  finished( TransferStatus::Aborted);
}

const OperationImpl::ErrorInfo& OperationImpl::errorInfo() const
{
  return errorInfoV;
}

OperationImpl::OperationImpl(
  boost::asio::io_context &ioContext,
  const TftpServerInternal &tftpServer,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  const Options::OptionList &clientOptions,
  const boost::asio::ip::udp::endpoint &local)
try:
  completionHandler( completionHandler),
  tftpServer( tftpServer),
  optionsV( tftpServer.options().negotiateServer( clientOptions)),
  maxReceivePacketSizeV( DefaultMaxPacketSize),
  receiveTimeoutV( tftpServer.configuration().tftpTimeout),
  socket( ioContext),
  timer( ioContext),
  transmitPacketType( PacketType::Invalid),
  transmitCounter( 0)
{
  try
  {
    // Open the socket
    socket.open( remote.protocol());

    // bind to local address
    socket.bind( local);

    // connect to client.
    socket.connect( remote);
  }
  catch (boost::system::system_error &err)
  {
    if (socket.is_open())
    {
      socket.close();
    }

    //! @throw CommunicationException
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( err.what()));
  }
}
catch (boost::system::system_error &err)
{
  //! @throw CommunicationException
  BOOST_THROW_EXCEPTION( CommunicationException() <<
    AdditionalInfo( err.what()));
}

OperationImpl::~OperationImpl() noexcept
{
}

void OperationImpl::finished(
  const TransferStatus status,
  ErrorInfo &&errorInfo) noexcept
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
    << "Operation finished";

  errorInfoV = std::move( errorInfo);

  timer.cancel();
  socket.cancel();

  if (completionHandler)
  {
    completionHandler( status);
  }
}

void OperationImpl::send( const Packets::Packet &packet)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
    << "TX: " << static_cast< std::string>( packet);

  // Reset the transmit counter
  transmitCounter = 1;

  // Store packet type
  transmitPacketType = packet.packetType();

  // Encode raw packet
  transmitPacket = static_cast< RawTftpPacket>( packet);

  try
  {
    socket.send( boost::asio::buffer( transmitPacket));
  }
  catch (boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
      << "TX ERROR: " << err.what();

    // Operation completed
    finished( TransferStatus::CommunicationError);
    return;
  }
}

void OperationImpl::receive()
{
  BOOST_LOG_FUNCTION()

  try
  {
    receivePacket.resize( maxReceivePacketSizeV);

    socket.async_receive(
      boost::asio::buffer( receivePacket),
      boost::bind(
        &OperationImpl::receiveHandler,
        shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));

    timer.expires_from_now( boost::posix_time::seconds( receiveTimeoutV));

    timer.async_wait( boost::bind(
      &OperationImpl::timeoutHandler,
      shared_from_this(),
      boost::asio::placeholders::error));
  }
  catch (boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
      << "RX ERROR: " << err.what();

    // Operation completed
    finished( TransferStatus::CommunicationError);
    return;
  }
}

const TftpConfiguration& OperationImpl::configuration() const
{
  return tftpServer.configuration();
}

const Options::OptionList& OperationImpl::options() const
{
  return optionsV;
}

void OperationImpl::maxReceivePacketSize(
  const uint16_t maxReceivePacketSize)
{
  maxReceivePacketSizeV = maxReceivePacketSize;
}

void OperationImpl::receiveTimeout( const uint8_t receiveTimeout) noexcept
{
  receiveTimeoutV = receiveTimeout;
}

void OperationImpl::readRequestPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::ReadRequestPacket &readRequestPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
    << "RX ERROR: " << static_cast< std::string>( readRequestPacket);

  using namespace std::literals::string_view_literals;
  Packets::ErrorPacket errorPacket(
    ErrorCode::IllegalTftpOperation,
    "RRQ not expected"sv);

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket));
}

void OperationImpl::writeRequestPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::WriteRequestPacket &writeRequestPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
    << "RX ERROR: " << static_cast< std::string>( writeRequestPacket);

  using namespace std::literals::string_view_literals;
  Packets::ErrorPacket errorPacket(
    ErrorCode::IllegalTftpOperation,
    "WRQ not expected"sv);

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket));
}

void OperationImpl::errorPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::ErrorPacket &errorPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
    << "RX ERROR: " << static_cast< std::string>( errorPacket);

  // Operation completed
  switch (transmitPacketType)
  {
    case PacketType::OptionsAcknowledgement:
      switch (errorPacket.errorCode())
      {
        case ErrorCode::TftpOptionRefused:
          finished( TransferStatus::OptionNegotiationError, std::move( errorPacket));
          break;

        default:
          finished( TransferStatus::TransferError, std::move( errorPacket));
          break;
      }
      break;

    default:
      finished( TransferStatus::TransferError, std::move( errorPacket));
      break;
  }
}

void OperationImpl::optionsAcknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
    << "RX ERROR: " << static_cast< std::string>( optionsAcknowledgementPacket);

  using namespace std::literals::string_view_literals;
  Packets::ErrorPacket errorPacket(
    ErrorCode::IllegalTftpOperation,
    "OACK not expected"sv);

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket));
}

void OperationImpl::invalidPacket(
  const boost::asio::ip::udp::endpoint &,
  const RawTftpPacket &)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
    << "RX: UNKNOWN";

  using namespace std::literals::string_view_literals;
  Packets::ErrorPacket errorPacket(
    ErrorCode::IllegalTftpOperation,
    "Invalid packet not expected"sv);

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket));
}

void OperationImpl::receiveHandler(
  const boost::system::error_code& errorCode,
  std::size_t bytesTransferred)
{
  BOOST_LOG_FUNCTION()

  // handle abort
  if (boost::asio::error::operation_aborted == errorCode)
  {
    return;
  }

  // Check error
  if (errorCode)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
      << "receive error: " << errorCode.message();

    // Operation completed
    finished( TransferStatus::CommunicationError);
    return;
  }

  // resize the received packet
  receivePacket.resize( bytesTransferred);

  // handle the received packet
  packet( socket.remote_endpoint(), receivePacket);
}

void OperationImpl::timeoutHandler( const boost::system::error_code& errorCode)
{
  BOOST_LOG_FUNCTION()

  // handle abort
  if (boost::asio::error::operation_aborted == errorCode)
  {
    return;
  }

  if (errorCode)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "timer error: " << errorCode.message();

    // Operation completed
    finished( TransferStatus::CommunicationError);
    return;
  }

  if (transmitCounter > tftpServer.configuration().tftpRetries)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Retry counter exceeded ABORT";

    // Operation completed
    finished( TransferStatus::CommunicationError);
    return;
  }

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning) <<
    "retransmit last packet";

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
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "TX error: " << err.what();

    // Operation completed
    finished( TransferStatus::CommunicationError);
    return;
  }
}

}
