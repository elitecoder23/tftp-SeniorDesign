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

  send( Packets::ErrorPacket(
    errorCode,
    errorMessage));

  // Operation completed
  finished( false);
}

void OperationImpl::abort()
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning) <<
    "Abort requested";

  // Operation completed
  finished( false);
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
  try
  {
    // Close the socket.
    socket.close();
  }
  catch (boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) << err.what();
  }
}

void OperationImpl::finished( bool successful) noexcept
{
  timer.cancel();
  socket.cancel();

  if (completionHandler)
  {
    completionHandler( successful);
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
    finished( false);
    return;
  }
}

void OperationImpl::receive()
{
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
    finished( false);
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
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX ERROR: " <<
    static_cast< std::string>( readRequestPacket);

  send( Packets::ErrorPacket(
    ErrorCode::IllegalTftpOperation,
    "RRQ not expected"));

  // Operation completed
  finished( false);
}

void OperationImpl::handleWriteRequestPacket(
  const UdpAddressType &,
  const Packets::WriteRequestPacket &writeRequestPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX ERROR: " <<
    static_cast< std::string>( writeRequestPacket);

  send( Packets::ErrorPacket(
    ErrorCode::IllegalTftpOperation,
    "WRQ not expected"));

  // Operation completed
  finished( false);
}

void OperationImpl::handleErrorPacket(
  const UdpAddressType &,
  const Packets::ErrorPacket &errorPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX ERROR: " <<
    static_cast< std::string>( errorPacket);

  // Operation completed
  finished( false);
}

void OperationImpl::handleOptionsAcknowledgementPacket(
  const UdpAddressType &,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX ERROR: " <<
    static_cast< std::string>( optionsAcknowledgementPacket);

  send( Packets::ErrorPacket(
    ErrorCode::IllegalTftpOperation,
    "OACK not expected"));

  // Operation completed
  finished( false);
}

void OperationImpl::handleInvalidPacket(
  const UdpAddressType &,
  const RawTftpPacketType &)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX: UNKNOWN";

  send( Packets::ErrorPacket(
    ErrorCode::IllegalTftpOperation,
    "Invalid packet not expected"));

  // Operation completed
  finished( false);
}

void OperationImpl::receiveHandler(
  const boost::system::error_code& errorCode,
  std::size_t bytesTransferred)
{
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
    finished( false);
    return;
  }

  // resize the received packet
  receivePacket.resize( bytesTransferred);

  // handle the received packet
  handlePacket( socket.remote_endpoint(), receivePacket);
}

void OperationImpl::timeoutHandler( const boost::system::error_code& errorCode)
{
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
    finished( false);
    return;
  }

  if (transmitCounter > tftpServerInternal.getConfiguration().tftpRetries)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Retry counter exceeded ABORT";

    // Operation completed
    finished( false);
    return;
  }

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
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
    finished( false);
    return;
  }
}

}
}
