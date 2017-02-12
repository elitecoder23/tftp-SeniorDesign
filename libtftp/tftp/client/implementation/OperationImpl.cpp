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
 * @brief Definition of class Tftp::Client::OperationImpl.
 **/

#include "OperationImpl.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/TftpConfiguration.hpp>

#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>
#include <tftp/packets/ErrorPacket.hpp>

#include <tftp/client/implementation/TftpClientInternal.hpp>

#include <helper/Logger.hpp>

#include <boost/bind.hpp>

namespace Tftp {
namespace Client {

OperationImpl::~OperationImpl() noexcept
{
  BOOST_LOG_FUNCTION();

  try
  {
    abort();
  }
  catch (boost::system::system_error &err)
  {
    // On error, ignore it and continue.
    BOOST_LOG_TRIVIAL( error) << err.what();
  }
}

void OperationImpl::operator()()
{
  // start first receive operation
  receiveFirst();

  // start the event loop
  ioService.run();
}

void OperationImpl::gracefulAbort(
  const ErrorCode errorCode [[gnu::unused]],
  const string &errorMessage [[gnu::unused]])
{
  //! @todo not implemented
}

void OperationImpl::abort()
{
  ioService.reset();

  ioService.stop();
}

RequestType OperationImpl::getRequestType() const
{
  return requestType;
}

const UdpAddressType& OperationImpl::getServerAddress() const
{
  return remoteEndpoint;
}

const OperationImpl::string& OperationImpl::getFilename() const
{
  return filename;
}

TransferMode OperationImpl::getMode() const
{
  return mode;
}

OperationImpl::OperationImpl(
  const RequestType requestType,
  const TftpClientInternal &tftpClient,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode,
  const UdpAddressType &from)
try :
  requestType( requestType),
  tftpClient( tftpClient),
  remoteEndpoint( serverAddress),
  filename( filename),
  mode( mode),
  options( tftpClient.getOptionList()),
  maxReceivePacketSize( DefaultMaxPacketSize),
  receiveTimeout( tftpClient.getConfiguration().tftpTimeout),
  socket( ioService),
  timer( ioService),
  transmitPacketType( PacketType::Invalid),
  transmitCounter( 0)
{
  try
  {
    // Open the socket
    socket.open( serverAddress.protocol());

    // Bind socket to source address (from)
    socket.bind( from);
  }
  catch ( boost::system::system_error &err)
  {
    // On error and if socket is opened - close it.
    if ( socket.is_open())
    {
      socket.close();
    }

    //! @throw CommunicationException Socket cannot be created or binded.
    BOOST_THROW_EXCEPTION(
      CommunicationException() << AdditionalInfo( err.what()));
  }
}
catch ( boost::system::system_error &err)
{
  //! @throw CommunicationException Error during initialisation of communication objects.
  BOOST_THROW_EXCEPTION(
    CommunicationException() << AdditionalInfo( err.what()));
}

OperationImpl::OperationImpl(
  const RequestType requestType,
  const TftpClientInternal &tftpClient,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode)
try:
  requestType( requestType),
  tftpClient( tftpClient),
  remoteEndpoint( serverAddress),
  filename( filename),
  mode( mode),
  options( tftpClient.getOptionList()),
  maxReceivePacketSize( DefaultMaxPacketSize),
  receiveTimeout( tftpClient.getConfiguration().tftpTimeout),
  socket( ioService),
  timer( ioService),
  transmitPacketType( PacketType::Invalid),
  transmitCounter( 0)
{
  try
  {
    // Open the socket
    socket.open( serverAddress.protocol());
  }
  catch ( boost::system::system_error &err)
  {
    // On error and if socket is opened - close it.
    if ( socket.is_open())
    {
      socket.close();
    }

    //! @throw CommunicationException Socket cannot be created.
    BOOST_THROW_EXCEPTION(
      CommunicationException() << AdditionalInfo( err.what()));
  }
}
catch ( boost::system::system_error &err)
{
  //! @throw CommunicationException Error during initialisation of communication objects.
  BOOST_THROW_EXCEPTION(
    CommunicationException() << AdditionalInfo( err.what()));
}

OperationImpl::OptionList& OperationImpl::getOptions()
{
  return options;
}

void OperationImpl::finished() noexcept
{
  ioService.stop();
}

void OperationImpl::sendFirst( const Packets::Packet &packet)
{
  BOOST_LOG_FUNCTION();

  try
  {
    BOOST_LOG_TRIVIAL( info) << "TX: " << static_cast< std::string>( packet);

    // Reset the transmit counter
    transmitCounter = 1;

    // Store packet type
    transmitPacketType = packet.getPacketType();

    // Encode raw packet
    transmitPacket = packet.encode();

    // Send the packet to the remote server
    socket.send_to(
      boost::asio::buffer( transmitPacket),
      remoteEndpoint);
  }
  catch (boost::system::system_error &err)
  {
    finished();

    //! @throw CommunicationException On communication error.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( err.what()));
  }
}

void OperationImpl::send( const Packets::Packet &packet)
{
  BOOST_LOG_FUNCTION();

  try
  {
    BOOST_LOG_TRIVIAL( info) << "TX: " << static_cast< std::string>( packet);

    // Reset the transmit counter
    transmitCounter = 1;

    // Store packet type
    transmitPacketType = packet.getPacketType();

    // Encode raw packet
    transmitPacket = packet.encode();

    // Send the packet to the remote server
    socket.send( boost::asio::buffer( transmitPacket));
  }
  catch (boost::system::system_error &err)
  {
    finished();

    //! @throw CommunicationException On communication error.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( err.what()));
  }
}

void OperationImpl::receiveFirst()
{
  BOOST_LOG_FUNCTION();

  try
  {
    // Resize the receive buffer to the allowed packet size
    receivePacket.resize( maxReceivePacketSize);

    // the first time, we make receive_from (answer is not sent from destination)
    // Start the receive operation
    socket.async_receive_from(
      boost::asio::buffer( receivePacket),
      receiveEndpoint,
      boost::bind(
        &OperationImpl::receiveFirstHandler,
        this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));

    // Set receive timeout
    timer.expires_from_now( boost::posix_time::seconds( receiveTimeout));

    // start waiting for receive timeout
    timer.async_wait( boost::bind(
      &OperationImpl::timeoutFirstHandler,
      this,
      boost::asio::placeholders::error));
  }
  catch (boost::system::system_error &err)
  {
    finished();

    //! @throw CommunicationException On communication error.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( err.what()));
  }
}

void OperationImpl::receive()
{
  BOOST_LOG_FUNCTION();

  try
  {
    receivePacket.resize( maxReceivePacketSize);

    ioService.reset();

    // start receive operation
    socket.async_receive(
      boost::asio::buffer( receivePacket),
      boost::bind(
        &OperationImpl::receiveHandler,
        this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));

    // set receive timeout
    timer.expires_from_now( boost::posix_time::seconds( receiveTimeout));

    // start waiting for receive timeout
    timer.async_wait( boost::bind(
      &OperationImpl::timeoutHandler,
      this,
      boost::asio::placeholders::error));
  }
  catch (boost::system::system_error &err)
  {
    finished();

    //! @throw CommunicationException On communication error.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( err.what()));
  }
}

void OperationImpl::setMaxReceivePacketSize(
  const uint16_t maxReceivePacketSize) noexcept
{
  this->maxReceivePacketSize = maxReceivePacketSize;
}

void OperationImpl::setReceiveTimeout(
  const uint8_t receiveTimeout) noexcept
{
  this->receiveTimeout = receiveTimeout;
}

void OperationImpl::handleReadRequestPacket(
  const UdpAddressType &,
  const Packets::ReadRequestPacket &readRequestPacket)
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_TRIVIAL( info) << "RX ERROR: " <<
    static_cast< std::string>( readRequestPacket);

  send( Packets::ErrorPacket(
      ErrorCode::IllegalTftpOperation,
      "RRQ not expected"));

  // Operation completed
  finished();

  //! @throw CommunicationException Always, because this packet is invalid.
  BOOST_THROW_EXCEPTION( CommunicationException() <<
    AdditionalInfo( "Unexpected packet received") <<
    PacketTypeInfo( PacketType::ReadRequest));
}

void OperationImpl::handleWriteRequestPacket(
  const UdpAddressType &,
  const Packets::WriteRequestPacket &writeRequestPacket)
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_TRIVIAL( info) << "RX ERROR: " <<
    static_cast< std::string>( writeRequestPacket);

  send( Packets::ErrorPacket(
    ErrorCode::IllegalTftpOperation,
    "WRQ not expected"));

  // Operation completed
  finished();

  //! @throw CommunicationException Always, because this packet is invalid.
  BOOST_THROW_EXCEPTION( CommunicationException() <<
    AdditionalInfo( "Unexpected packet received") <<
    PacketTypeInfo( PacketType::WriteRequest));
}

void OperationImpl::handleErrorPacket(
  const UdpAddressType &,
  const Packets::ErrorPacket &errorPacket)
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_TRIVIAL( info) << "RX ERROR: " <<
    static_cast< std::string>( errorPacket);

  // Operation completed
  finished();

  //! @throw ErrorReceivedException Always, because this is an error.
  BOOST_THROW_EXCEPTION(
    ErrorReceivedException() <<
      AdditionalInfo( "ERR not expected") <<
      PacketTypeInfo( transmitPacketType) <<
      ErrorPacketInfo( errorPacket));
}

void OperationImpl::handleInvalidPacket(
  const UdpAddressType &,
  const RawTftpPacketType &)
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_TRIVIAL( error) << "RX ERROR: INVALID Packet";

  send( Packets::ErrorPacket(
    ErrorCode::IllegalTftpOperation,
    "Invalid packet not expected"));

  // Operation completed
  finished();

  //! @throw CommunicationException Always.
  BOOST_THROW_EXCEPTION( CommunicationException() <<
    AdditionalInfo( "Invalid packet received") <<
    PacketTypeInfo( PacketType::Invalid));
}

void OperationImpl::receiveFirstHandler(
  const boost::system::error_code& errorCode,
  std::size_t bytesTransferred)
{
  BOOST_LOG_FUNCTION();

  // operation has been aborted (maybe timeout)
  if (boost::asio::error::operation_aborted == errorCode)
  {
    return;
  }

  // (internal) receive error occurred
  if (errorCode)
  {
    BOOST_LOG_TRIVIAL( error) <<
      "Error when receiving message: " << errorCode.message();

    finished();

    //! @throw CommunicationException On communication error.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( errorCode.message()));
  }

  // check, if packet has been received from not expected source
  // send error packet and ignore it.
  if (remoteEndpoint.address() != receiveEndpoint.address())
  {
    BOOST_LOG_TRIVIAL( error) <<
      "Received packed from wrong source: " <<
      receiveEndpoint.address().to_string();

    // sent Error packet to unknown partner
    try
    {
      Packets::ErrorPacket err(
        ErrorCode::UnknownTransferId,
        "Packet from wrong source");

      socket.send_to( boost::asio::buffer( err.encode()), receiveEndpoint);
    }
    catch ( boost::system::system_error &err)
    {
      // ignore send error to unknown partner
      BOOST_LOG_TRIVIAL( error) << "Error sending ERR packet: " << err.what();
    }

    // restart receive operation
    try
    {
      socket.async_receive_from(
        boost::asio::buffer( receivePacket),
        receiveEndpoint,
        boost::bind(
          &OperationImpl::receiveFirstHandler,
          this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));

      return;
    }
    catch ( boost::system::system_error &err)
    {
      finished();

      //! @throw CommunicationException On communication error.
      BOOST_THROW_EXCEPTION( CommunicationException() <<
        AdditionalInfo( err.what()));
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
    finished();

    //! @throw CommunicationException On communication error.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( err.what()));
  }

  receivePacket.resize( bytesTransferred);

  handlePacket( receiveEndpoint, receivePacket);
}

void OperationImpl::receiveHandler(
  const boost::system::error_code& errorCode,
  const std::size_t bytesTransferred)
{
  BOOST_LOG_FUNCTION();

  // operation has been aborted (maybe timeout)
  // error is not handled here.
  if (boost::asio::error::operation_aborted == errorCode)
  {
    return;
  }

  // (internal) receive error occurred
  if (errorCode)
  {
    BOOST_LOG_TRIVIAL( error) <<
      "Error when receiving message: " << errorCode.message();

    finished();

    //! @throw CommunicationException On communication error.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( errorCode.message()));
  }

  receivePacket.resize( bytesTransferred);

  handlePacket( remoteEndpoint, receivePacket);
}

void OperationImpl::timeoutFirstHandler(
  const boost::system::error_code& errorCode)
{
  BOOST_LOG_FUNCTION();

  //! operation aborted (packet received)
  if (boost::asio::error::operation_aborted == errorCode)
  {
    return;
  }

  // internal (timer) error occurred
  if (errorCode)
  {
    BOOST_LOG_TRIVIAL( error) << "timer error: " + errorCode.message();

    finished();

    //! @throw TftpException On internal Timer error.
    BOOST_THROW_EXCEPTION( TftpException() <<
      AdditionalInfo( "internal timer error"));
  }

  // if maximum retries exceeded -> abort receive operation
  if (transmitCounter > tftpClient.getConfiguration().tftpRetries)
  {
    BOOST_LOG_TRIVIAL( error) << "Retry counter exceeded ABORT";

    finished();

    //! @throw CommunicationException On receive timeout
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( "Timeout when waiting for response from server"));
  }

  BOOST_LOG_TRIVIAL( info) << "retransmit last packet";

  try
  {
    socket.send_to( boost::asio::buffer( transmitPacket), remoteEndpoint);

    ++transmitCounter;

    timer.expires_from_now( boost::posix_time::seconds( receiveTimeout));

    timer.async_wait( boost::bind(
      &OperationImpl::timeoutFirstHandler,
      this,
      boost::asio::placeholders::error));
  }
  catch (boost::system::system_error &err)
  {
    finished();

    //! @throw CommunicationException On communication error.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( err.what()));
  }
}

void OperationImpl::timeoutHandler(
  const boost::system::error_code& errorCode)
{
  BOOST_LOG_FUNCTION();

  // operation aborted (packet received)
  if (boost::asio::error::operation_aborted == errorCode)
  {
    return;
  }

  // internal (timer) error occurred
  if (errorCode)
  {
    BOOST_LOG_TRIVIAL( error) << "timer error: " << errorCode.message();

    finished();

    //! @throw TftpException On internal Timer error.
    BOOST_THROW_EXCEPTION( TftpException() <<
      AdditionalInfo( "internal timer error"));
  }

  // if maximum retries exceeded -> abort receive operation
  if (transmitCounter > tftpClient.getConfiguration().tftpRetries)
  {
    BOOST_LOG_TRIVIAL( error) << "Retry counter exceeded ABORT";

    finished();

    //! @throw CommunicationException On receive timeout
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( "timeout"));
  }

  BOOST_LOG_TRIVIAL( info) << "retransmit last packet";

  try
  {
    socket.send( boost::asio::buffer( transmitPacket));

    ++transmitCounter;

    timer.expires_from_now( boost::posix_time::seconds( receiveTimeout));

    timer.async_wait( boost::bind(
      &OperationImpl::timeoutHandler,
      this,
      boost::asio::placeholders::error));
  }
  catch (boost::system::system_error &err)
  {
    finished();

    //! @throw CommunicationException On communication error.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( err.what()));
  }
}

}
}
