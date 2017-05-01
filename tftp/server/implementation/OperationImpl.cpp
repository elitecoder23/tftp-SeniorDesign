/*
 * $Date$
 * $Revision$
 */
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class Tftp::Server::TftpServerOperation.
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

namespace Tftp {
namespace Server {

void OperationImpl::start()
{
  // start first receive operation
  receive();
}

void OperationImpl::gracefulAbort(
  const ErrorCode errorCode,
  const string &errorMessage)
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning) <<
    "Graceful abort requested: " << errorCode << " '" << errorMessage << "'";

  Packets::ErrorPacket errorPacket(
    errorCode,
    errorMessage);

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

const OperationImpl::ErrorInfo& OperationImpl::getErrorInfo() const
{
  return errorInfo;
}

OperationImpl::OperationImpl(
  boost::asio::io_service &ioService,
  OperationCompletedHandler completionHandler,
  const TftpServerInternal &tftpServerInternal,
  const UdpAddressType &clientAddress,
  const Options::OptionList &clientOptions,
  const UdpAddressType &serverAddress)
try:
  completionHandler( completionHandler),
  tftpServerInternal( tftpServerInternal),
  options( tftpServerInternal.getOptionList().negotiateServer( clientOptions)),
  maxReceivePacketSize( DefaultMaxPacketSize),
  receiveTimeout( tftpServerInternal.getConfiguration().tftpTimeout),
  socket( ioService),
  timer( ioService),
  transmitPacketType( PacketType::Invalid),
  transmitCounter( 0)
{
  try
  {
    // Open the socket
    socket.open( clientAddress.protocol());

    // bind to local address
    socket.bind( serverAddress);

    // connect to client.
    socket.connect( clientAddress);
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

OperationImpl::OperationImpl(
  boost::asio::io_service &ioService,
  OperationCompletedHandler completionHandler,
  const TftpServerInternal &tftpServerInternal,
  const UdpAddressType &clientAddress,
  const Options::OptionList &clientOptions)
try:
  completionHandler( completionHandler),
  tftpServerInternal( tftpServerInternal),
  options( tftpServerInternal.getOptionList().negotiateServer( clientOptions)),
  maxReceivePacketSize( DefaultMaxPacketSize),
  receiveTimeout( tftpServerInternal.getConfiguration().tftpTimeout),
  socket( ioService),
  timer( ioService),
  transmitPacketType( PacketType::Invalid),
  transmitCounter( 0)
{
  BOOST_LOG_FUNCTION();

  try
  {
    // Open socket
    socket.open( clientAddress.protocol());

    // Connect to client
    socket.connect( clientAddress);
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
  BOOST_LOG_FUNCTION();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
    "Operation finished";

  this->errorInfo = errorInfo;

  timer.cancel();
  socket.cancel();

  if (completionHandler)
  {
    completionHandler( status);
  }
}

void OperationImpl::send( const Packets::Packet &packet)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "TX: "
    << static_cast< std::string>( packet);

  // Reset the transmit counter
  transmitCounter = 1;

  // Store packet type
  transmitPacketType = packet.getPacketType();

  // Encode raw packet
  transmitPacket = packet.encode();

  try
  {
    socket.send( boost::asio::buffer( transmitPacket));
  }
  catch (boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) << "TX ERROR: " <<
      err.what();

    // Operation completed
    finished( TransferStatus::CommunicationError);
    return;
  }
}

void OperationImpl::receive()
{
  BOOST_LOG_FUNCTION();

  try
  {
    receivePacket.resize( maxReceivePacketSize);

    socket.async_receive(
      boost::asio::buffer( receivePacket),
      boost::bind(
        &OperationImpl::receiveHandler,
        shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));

    timer.expires_from_now( boost::posix_time::seconds( receiveTimeout));

    timer.async_wait( boost::bind(
      &OperationImpl::timeoutHandler,
      shared_from_this(),
      boost::asio::placeholders::error));
  }
  catch (boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) << "RX ERROR: " <<
      err.what();

    // Operation completed
    finished( TransferStatus::CommunicationError);
    return;
  }
}

Options::OptionList& OperationImpl::getOptions()
{
  return options;
}

void OperationImpl::setMaxReceivePacketSize(
  const uint16_t maxReceivePacketSize)
{
  this->maxReceivePacketSize = maxReceivePacketSize;
}

void OperationImpl::setReceiveTimeout( const uint8_t receiveTimeout)
{
  this->receiveTimeout = receiveTimeout;
}

void OperationImpl::handleReadRequestPacket(
  const UdpAddressType &,
  const Packets::ReadRequestPacket &readRequestPacket)
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) << "RX ERROR: " <<
    static_cast< std::string>( readRequestPacket);

  Packets::ErrorPacket errorPacket(
    ErrorCode::IllegalTftpOperation,
    "RRQ not expected");

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket));
}

void OperationImpl::handleWriteRequestPacket(
  const UdpAddressType &,
  const Packets::WriteRequestPacket &writeRequestPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) << "RX ERROR: " <<
    static_cast< std::string>( writeRequestPacket);

  Packets::ErrorPacket errorPacket(
    ErrorCode::IllegalTftpOperation,
    "WRQ not expected");

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket));
}

void OperationImpl::handleErrorPacket(
  const UdpAddressType &,
  const Packets::ErrorPacket &errorPacket)
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) << "RX ERROR: " <<
    static_cast< std::string>( errorPacket);

  // Operation completed
  switch (transmitPacketType)
  {
    case PacketType::OptionsAcknowledgement:
      switch (errorPacket.getErrorCode())
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

void OperationImpl::handleOptionsAcknowledgementPacket(
  const UdpAddressType &,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket)
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) << "RX ERROR: " <<
    static_cast< std::string>( optionsAcknowledgementPacket);

  Packets::ErrorPacket errorPacket(
    ErrorCode::IllegalTftpOperation,
    "OACK not expected");

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket));
}

void OperationImpl::handleInvalidPacket(
  const UdpAddressType &,
  const RawTftpPacketType &)
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) << "RX: UNKNOWN";

  Packets::ErrorPacket errorPacket(
    ErrorCode::IllegalTftpOperation,
    "Invalid packet not expected");

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket));
}

void OperationImpl::receiveHandler(
  const boost::system::error_code& errorCode,
  std::size_t bytesTransferred)
{
  BOOST_LOG_FUNCTION();

  // handle abort
  if (boost::asio::error::operation_aborted == errorCode)
  {
    return;
  }

  // Check error
  if (errorCode)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "receive error: " << errorCode.message();

    // Operation completed
    finished( TransferStatus::CommunicationError);
    return;
  }

  // resize the received packet
  receivePacket.resize( bytesTransferred);

  // handle the received packet
  handlePacket( socket.remote_endpoint(), receivePacket);
}

void OperationImpl::timeoutHandler( const boost::system::error_code& errorCode)
{
  BOOST_LOG_FUNCTION();

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

  if (transmitCounter > tftpServerInternal.getConfiguration().tftpRetries)
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

    timer.expires_from_now( boost::posix_time::seconds( receiveTimeout));

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
}
