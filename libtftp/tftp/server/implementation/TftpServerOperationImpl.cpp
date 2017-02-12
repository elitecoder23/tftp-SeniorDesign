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

#include "TftpServerOperationImpl.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/TftpConfiguration.hpp>

#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>
#include <tftp/packets/ErrorPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>

#include <tftp/server/implementation/TftpServerInternal.hpp>

#include <helper/Logger.hpp>

#include <boost/bind.hpp>

namespace Tftp {
namespace Server {

TftpServerOperationImpl::TftpServerOperationImpl(
  const TftpServerInternal &tftpServerInternal,
  const UdpAddressType &clientAddress,
  const Options::OptionList &clientOptions,
  const UdpAddressType &serverAddress)
try:
  tftpServerInternal( tftpServerInternal),
  clientAddress( clientAddress),
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

TftpServerOperationImpl::TftpServerOperationImpl(
  const TftpServerInternal &tftpServerInternal,
  const UdpAddressType &clientAddress,
  const Options::OptionList &clientOptions)
try:
  tftpServerInternal( tftpServerInternal),
  clientAddress( clientAddress),
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

TftpServerOperationImpl::~TftpServerOperationImpl() noexcept
{
  try
  {
    finished();

    // Close the socket.
    socket.close();
  }
  catch (boost::system::system_error &err)
  {
    BOOST_LOG_TRIVIAL( error) << err.what();
  }
}

void TftpServerOperationImpl::operator()()
{
  // start first receive operation
  receive();

  // start the event loop
  ioService.run();
}

void TftpServerOperationImpl::finished() noexcept
{
  ioService.stop();
}

void TftpServerOperationImpl::send( const Packets::TftpPacket &packet)
{
  BOOST_LOG_TRIVIAL( info) << "TX: " << packet.toString();

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
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( err.what()));
  }
}

void TftpServerOperationImpl::receive()
{
  try
  {
    receivePacket.resize( maxReceivePacketSize);

    ioService.reset();

    socket.async_receive(
      boost::asio::buffer( receivePacket),
      boost::bind(
        &TftpServerOperationImpl::receiveHandler,
        this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));

    timer.expires_from_now( boost::posix_time::seconds( receiveTimeout));

    timer.async_wait( boost::bind(
      &TftpServerOperationImpl::timeoutHandler,
      this,
      boost::asio::placeholders::error));
  }
  catch (boost::system::system_error &err)
  {
    //! @throw CommunicationException On communication failure.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( err.what()));
  }
}


Options::OptionList& TftpServerOperationImpl::getOptions()
{
  return options;
}

void TftpServerOperationImpl::setMaxReceivePacketSize(
  const uint16_t maxReceivePacketSize)
{
  this->maxReceivePacketSize = maxReceivePacketSize;
}

void TftpServerOperationImpl::setReceiveTimeout( const uint8_t receiveTimeout)
{
  this->receiveTimeout = receiveTimeout;
}

void TftpServerOperationImpl::handleReadRequestPacket(
  const UdpAddressType &,
  const Packets::ReadRequestPacket &readRequestPacket)
{
  BOOST_LOG_TRIVIAL( info) << "RX ERROR: " << readRequestPacket.toString();

  send( Packets::ErrorPacket(
    ErrorCode::ILLEGAL_TFTP_OPERATION,
    "RRQ not expected"));

  // Operation completed
  finished();

  //! @throw CommunicationException Always, because this packet is invalid.
  BOOST_THROW_EXCEPTION( CommunicationException() <<
    AdditionalInfo( "Unexpected packet received") <<
   PacketTypeInfo( PacketType::ReadRequest));
}

void TftpServerOperationImpl::handleWriteRequestPacket(
  const UdpAddressType &,
  const Packets::WriteRequestPacket &writeRequestPacket)
{
  BOOST_LOG_TRIVIAL( info) << "RX ERROR: " << writeRequestPacket.toString();

  send( Packets::ErrorPacket(
    ErrorCode::ILLEGAL_TFTP_OPERATION,
    "WRQ not expected"));

  // Operation completed
  finished();

  //! @throw CommunicationException Always, because this packet is invalid.
  BOOST_THROW_EXCEPTION( CommunicationException() <<
    AdditionalInfo( "Unexpected packet received") <<
   PacketTypeInfo( PacketType::WriteRequest));
}

void TftpServerOperationImpl::handleErrorPacket(
  const UdpAddressType &,
  const Packets::ErrorPacket &errorPacket)
{
  BOOST_LOG_TRIVIAL( info) << "RX ERROR: " << errorPacket.toString();

  //! @throw ErrorReceivedException Always, because this is an error.
  BOOST_THROW_EXCEPTION(
    ErrorReceivedException() <<
    AdditionalInfo( "ERR not expected") <<
    PacketTypeInfo( transmitPacketType) <<
    ErrorPacketInfo( errorPacket));
}

void TftpServerOperationImpl::handleOptionsAcknowledgementPacket(
  const UdpAddressType &,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket)
{
  BOOST_LOG_TRIVIAL( info) <<
    "RX ERROR: " << optionsAcknowledgementPacket.toString();

  send( Packets::ErrorPacket(
    ErrorCode::ILLEGAL_TFTP_OPERATION,
    "OACK not expected"));

  // Operation completed
  finished();

  //! @throw CommunicationException Always, because this packet is invalid.
  BOOST_THROW_EXCEPTION( CommunicationException() <<
    AdditionalInfo( "Unexpected packet received") <<
   PacketTypeInfo( PacketType::OptionsAcknowledgement));
}

void TftpServerOperationImpl::handleInvalidPacket(
  const UdpAddressType &,
  const RawTftpPacketType &)
{
  BOOST_LOG_TRIVIAL( info) << "RX: UNKNOWN";

  send( Packets::ErrorPacket(
    ErrorCode::ILLEGAL_TFTP_OPERATION,
    "Invalid packet not expected"));

  //! @throw CommunicationException Always, because this packet is invalid.
  BOOST_THROW_EXCEPTION( CommunicationException() <<
    AdditionalInfo( "Invalid TFTP packet received"));
}

void TftpServerOperationImpl::receiveHandler(
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
    BOOST_LOG_TRIVIAL( error) << "receive error: " << errorCode.message();

    //! @throw CommunicationException On communication failure.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( errorCode.message()));
  }

  // resize the received packet
  receivePacket.resize( bytesTransferred);

  // handle the received packet
  handlePacket( clientAddress, receivePacket);
}

void TftpServerOperationImpl::timeoutHandler(
  const boost::system::error_code& errorCode)
{
  // handle abort
  if (boost::asio::error::operation_aborted == errorCode)
  {
    return;
  }

  if (errorCode)
  {
    BOOST_LOG_TRIVIAL( error) << "timer error: " << errorCode.message();

    //! @throw CommunicationException On communication failure.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( errorCode.message()));
  }

  if (transmitCounter > tftpServerInternal.getConfiguration().tftpRetries)
  {
    //! @throw CommunicationException When retry counter reached.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( "Retry counter exceeded ABORT"));
  }

  BOOST_LOG_TRIVIAL( info) << "retransmit last packet";

  try
  {
    socket.send( boost::asio::buffer( transmitPacket));

    ++transmitCounter;

    timer.expires_from_now( boost::posix_time::seconds( receiveTimeout));

    timer.async_wait( boost::bind(
      &TftpServerOperationImpl::timeoutHandler,
      this,
      boost::asio::placeholders::error));
  }
  catch (boost::system::system_error &err)
  {
    //! @throw CommunicationException On communication failure.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( err.what()));
  }
}

}
}
