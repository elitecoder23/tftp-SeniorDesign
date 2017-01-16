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
 * @brief Definition of class Tftp::Server::TftpServerImpl.
 **/

#include "TftpServerImpl.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/packet/PacketFactory.hpp>
#include <tftp/server/implementation/TftpServerReadRequestOperationImpl.hpp>
#include <tftp/server/implementation/TftpServerWriteRequestOperationImpl.hpp>
#include <tftp/server/implementation/TftpServerErrorOperation.hpp>

#include <helper/Logger.hpp>

#include <boost/bind.hpp>

#include <vector>
#include <cstdint>

namespace Tftp {
namespace Server {

TftpServerImpl::TftpServerImpl(
	const TftpConfiguration &configuration,
  const OptionList& additionalOptions,
  const UdpAddressType &serverAddress)
try :
  configuration( configuration),
  options( configuration.getServerOptions( additionalOptions)),
  serverAddress( serverAddress),
  socket( ioService)
{
  try
  {
    // open the socket
    socket.open( serverAddress.protocol());

    // bind to the local address
    socket.bind( serverAddress);
  }
  catch ( boost::system::system_error &err)
  {
    // close socket when opened
    if ( socket.is_open())
    {
      socket.close();
    }

    //! @throw CommunicationException When socket operation fails.
    BOOST_THROW_EXCEPTION(
      CommunicationException() << AdditionalInfo( err.what())
        << TftpTransferPhaseInfo(
          TftpTransferPhase::TFTP_PHASE_INITIALIZATION));
  }
}
catch ( boost::system::system_error &err)
{
  //! @throw CommunicationException When socket operation fails.
  BOOST_THROW_EXCEPTION(
    CommunicationException() << AdditionalInfo( err.what())
      << TftpTransferPhaseInfo( TftpTransferPhase::TFTP_PHASE_INITIALIZATION));
}

TftpServerImpl::~TftpServerImpl() noexcept
{
  try
  {
    stop();

    socket.close();
  }
  catch ( boost::system::system_error &err)
  {
    // do not throw an exception within the constructor
    BOOST_LOG_TRIVIAL( error)<< err.what();
  }
}

void TftpServerImpl::registerRequestHandler( ReceivedTftpRequestHandler handler)
{
  this->handler = handler;
}

void TftpServerImpl::start()
{
  try
  {
    // start receive
    receive();

    // the server loop
    ioService.run();
  }
  catch (boost::system::system_error &err)
  {
    // only internal error are handled by this exception
    BOOST_THROW_EXCEPTION( TftpException() <<
      AdditionalInfo( err.what()));
  }

}

void TftpServerImpl::stop()
{
  // cancel receive operation
  socket.cancel();

  // stop handler
  ioService.stop();
}

TftpServerOperation TftpServerImpl::createReadRequestOperation(
  TftpTransmitDataOperationHandler &handler,
  const UdpAddressType &clientAddress,
  const OptionList &clientOptions,
  const UdpAddressType &serverAddress)
{
  auto operation = std::make_shared< TftpServerReadRequestOperationImpl>(
    handler,
    *this,
    clientAddress,
    clientOptions,
    serverAddress);

  return std::bind( &TftpServerReadRequestOperationImpl::operator(), operation);
}

TftpServerOperation TftpServerImpl::createReadRequestOperation(
  TftpTransmitDataOperationHandler &handler,
  const UdpAddressType &clientAddress,
  const OptionList &clientOptions)
{
  auto operation = std::make_shared< TftpServerReadRequestOperationImpl>(
    handler,
    *this,
    clientAddress,
    clientOptions);

  return std::bind( &TftpServerReadRequestOperationImpl::operator(), operation);
}

TftpServerOperation TftpServerImpl::createWriteRequestOperation(
  TftpReceiveDataOperationHandler &handler,
  const UdpAddressType &clientAddress,
  const OptionList &clientOptions,
  const UdpAddressType &serverAddress)
{
  auto operation = std::make_shared< TftpServerWriteRequestOperationImpl>(
    handler,
    *this,
    clientAddress,
    clientOptions,
    serverAddress);

  return std::bind(
    &TftpServerWriteRequestOperationImpl::operator(),
    operation);
}

TftpServerOperation TftpServerImpl::createWriteRequestOperation(
  TftpReceiveDataOperationHandler &handler,
  const UdpAddressType &clientAddress,
  const OptionList &clientOptions)
{
  auto operation = std::make_shared< TftpServerWriteRequestOperationImpl>(
    handler,
    *this,
    clientAddress,
    clientOptions);

  return std::bind(
    &TftpServerWriteRequestOperationImpl::operator(),
    operation);
}

TftpServerOperation TftpServerImpl::createErrorOperation(
  const UdpAddressType &clientAddress,
  const UdpAddressType &from,
  const ErrorCode errorCode,
  const string &errorMessage)
{
  auto operation = std::make_shared< TftpServerErrorOperation>(
    clientAddress,
    from,
    errorCode,
    errorMessage);

  return std::bind( &TftpServerErrorOperation::operator(), operation);
}

TftpServerOperation TftpServerImpl::createErrorOperation(
  const UdpAddressType &clientAddress,
  const ErrorCode errorCode,
  const string &errorMessage)
{
  auto operation = std::make_shared< TftpServerErrorOperation>(
    clientAddress,
    errorCode,
    errorMessage);

  return std::bind( &TftpServerErrorOperation::operator(), operation);
}

const Tftp::TftpConfiguration& TftpServerImpl::getConfiguration() const
{
  return configuration;
}

const OptionList& TftpServerImpl::getOptionList() const
{
  return options;
}

void TftpServerImpl::receive()
{
  try
  {
    packet.resize( DEFAULT_MAX_PACKET_SIZE);

    // wait for incoming packet
    socket.async_receive_from(
      boost::asio::buffer( packet),
      remoteEndpoint,
      boost::bind(
        &TftpServerImpl::receiveHandler,
        this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
  }
  catch ( boost::system::system_error &err)
  {
    ioService.stop();

    //! @throw CommunicationException on IO error.
    BOOST_THROW_EXCEPTION(
      CommunicationException() << AdditionalInfo( err.what()));
  }
}

void TftpServerImpl::receiveHandler(
  const boost::system::error_code& errorCode,
  std::size_t bytesTransferred)
{
  // handle abort
  if ( boost::asio::error::operation_aborted == errorCode)
  {
    return;
  }

  // Check error
  if ( errorCode)
  {
    BOOST_LOG_TRIVIAL( error)<< "receive error: " + errorCode.message();

    //! @throw CommunicationException On communication failure.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( errorCode.message()));
  }

  try
  {
    // resize buffer to actual size
    packet.resize( bytesTransferred);

    // handle the received packet (decode it and call the approbate handler)
    handlePacket( remoteEndpoint, packet);
  }
  catch ( TftpException &e)
  {
    BOOST_LOG_TRIVIAL( error)<< "TFTP exception: " << e.what();
  }

  receive();
}

void TftpServerImpl::handleReadRequestPacket(
  const UdpAddressType &from,
  const ReadRequestPacket &readRequestPacket)
{
  BOOST_LOG_TRIVIAL( info)<< "RX: " << readRequestPacket.toString();

  // check handler
  if (!handler)
  {
    BOOST_LOG_TRIVIAL( info) <<
    "No registered handler - reject";

    TftpServerOperation errOp = createErrorOperation(
      from,
      ErrorCode::FILE_NOT_FOUND,
      "RRQ not accepted");

    // execute error operation
    errOp();
  }

  // call the handler, which handles the received request
  handler(
    TftpRequestType::ReadRequest,
    from,
    readRequestPacket.getFilename(),
    readRequestPacket.getMode(),
    readRequestPacket.getOptions());
}

void TftpServerImpl::handleWriteRequestPacket(
  const UdpAddressType &from,
  const WriteRequestPacket &writeRequestPacket)
{
  BOOST_LOG_TRIVIAL( info)<< "RX: " << writeRequestPacket.toString();

  // check handler
  if (!handler)
  {
    BOOST_LOG_TRIVIAL( info) << "No registered handler - reject";

    TftpServerOperation errOp = createErrorOperation(
      from,
      ErrorCode::FILE_NOT_FOUND,
      "WRQ");

    // execute error operation
    errOp();
  }

  // call the handler, which handles the received request
  handler(
    TftpRequestType::WriteRequest,
    from,
    writeRequestPacket.getFilename(),
    writeRequestPacket.getMode(),
    writeRequestPacket.getOptions());
}

void TftpServerImpl::handleDataPacket(
  const UdpAddressType &from,
  const DataPacket &dataPacket)
{
  BOOST_LOG_TRIVIAL( info)<< "RX ERROR: " << dataPacket.toString();

  TftpServerOperation errOp = createErrorOperation(
    from,
    ErrorCode::ILLEGAL_TFTP_OPERATION,
    "DATA not expected");

  // execute error operation
  errOp();
}

void TftpServerImpl::handleAcknowledgementPacket(
  const UdpAddressType &from,
  const AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_TRIVIAL( info)<<
  "RX ERROR: " << acknowledgementPacket.toString();

  TftpServerOperation errOp = createErrorOperation(
    from,
    ErrorCode::ILLEGAL_TFTP_OPERATION,
    "ACK not expected");

  // execute error operation
  errOp();
}

void TftpServerImpl::handleErrorPacket(
  const UdpAddressType &from,
  const ErrorPacket &errorPacket)
{
  BOOST_LOG_TRIVIAL( info)<<
  "RX ERROR: " << errorPacket.toString();

  TftpServerOperation errOp = createErrorOperation(
    from,
    ErrorCode::ILLEGAL_TFTP_OPERATION,
    "ERROR not expected");

  // execute error operation
  errOp();
}

void TftpServerImpl::handleOptionsAcknowledgementPacket(
  const UdpAddressType &from,
  const OptionsAcknowledgementPacket &optionsAcknowledgementPacket)
{
  BOOST_LOG_TRIVIAL( info)<<
  "RX ERROR: " << optionsAcknowledgementPacket.toString();

  TftpServerOperation errOp = createErrorOperation(
    from,
    ErrorCode::ILLEGAL_TFTP_OPERATION,
    "OACK not expected");

  // execute error operation
  errOp();
}

void TftpServerImpl::handleInvalidPacket(
  const UdpAddressType &,
  const RawTftpPacketType &)
{
  BOOST_LOG_TRIVIAL( info)<<"RX: UNKNOWN: *ERROR* - IGNORE";
}

}
}
