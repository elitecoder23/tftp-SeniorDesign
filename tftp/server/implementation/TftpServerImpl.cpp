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
#include <tftp/TftpLogger.hpp>
#include <tftp/packets/PacketFactory.hpp>
#include <tftp/server/implementation/ReadRequestOperationImpl.hpp>
#include <tftp/server/implementation/WriteRequestOperationImpl.hpp>
#include <tftp/server/implementation/ErrorOperation.hpp>

#include <boost/bind.hpp>

#include <vector>
#include <cstdint>

namespace Tftp {
namespace Server {

TftpServerImpl::TftpServerImpl(
  ReceivedTftpRequestHandler handler,
  const TftpConfiguration &configuration,
  const Options::OptionList& additionalOptions,
  const UdpAddressType &serverAddress)
try :
  handler( handler),
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
        << TransferPhaseInfo(
          TransferPhase::Initialisation));
  }
}
catch ( boost::system::system_error &err)
{
  //! @throw CommunicationException When socket operation fails.
  BOOST_THROW_EXCEPTION(
    CommunicationException() << AdditionalInfo( err.what())
      << TransferPhaseInfo( TransferPhase::Initialisation));
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
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)<< err.what();
  }
}

void TftpServerImpl::operator()()
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

OperationPtr TftpServerImpl::createReadRequestOperation(
  TransmitDataHandler &handler,
  const UdpAddressType &clientAddress,
  const Options::OptionList &clientOptions,
  const UdpAddressType &serverAddress)
{
  auto operation = std::make_shared< ReadRequestOperationImpl>(
    handler,
    *this,
    clientAddress,
    clientOptions,
    serverAddress);

  return operation;
}

OperationPtr TftpServerImpl::createReadRequestOperation(
  TransmitDataHandler &handler,
  const UdpAddressType &clientAddress,
  const Options::OptionList &clientOptions)
{
  auto operation = std::make_shared< ReadRequestOperationImpl>(
    handler,
    *this,
    clientAddress,
    clientOptions);

  return operation;
}

OperationPtr TftpServerImpl::createWriteRequestOperation(
  ReceiveDataHandler &handler,
  const UdpAddressType &clientAddress,
  const Options::OptionList &clientOptions,
  const UdpAddressType &serverAddress)
{
  auto operation = std::make_shared< WriteRequestOperationImpl>(
    handler,
    *this,
    clientAddress,
    clientOptions,
    serverAddress);

  return operation;
}

OperationPtr TftpServerImpl::createWriteRequestOperation(
  ReceiveDataHandler &handler,
  const UdpAddressType &clientAddress,
  const Options::OptionList &clientOptions)
{
  auto operation = std::make_shared< WriteRequestOperationImpl>(
    handler,
    *this,
    clientAddress,
    clientOptions);

  return operation;
}

OperationPtr TftpServerImpl::createErrorOperation(
  const UdpAddressType &clientAddress,
  const UdpAddressType &from,
  const ErrorCode errorCode,
  const string &errorMessage)
{
  auto operation = std::make_shared< ErrorOperation>(
    clientAddress,
    from,
    errorCode,
    errorMessage);

  return operation;
}

OperationPtr TftpServerImpl::createErrorOperation(
  const UdpAddressType &clientAddress,
  const ErrorCode errorCode,
  const string &errorMessage)
{
  auto operation = std::make_shared< ErrorOperation>(
    clientAddress,
    errorCode,
    errorMessage);

  return operation;
}

const Tftp::TftpConfiguration& TftpServerImpl::getConfiguration() const
{
  return configuration;
}

const Options::OptionList& TftpServerImpl::getOptionList() const
{
  return options;
}

void TftpServerImpl::receive()
{
  try
  {
    packet.resize( DefaultMaxPacketSize);

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
  const std::size_t bytesTransferred)
{
  // handle abort
  if ( boost::asio::error::operation_aborted == errorCode)
  {
    return;
  }

  // Check error
  if ( errorCode)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "receive error: " + errorCode.message();

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
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
      << "TFTP exception: " << e.what();
  }

  receive();
}

void TftpServerImpl::handleReadRequestPacket(
  const UdpAddressType &from,
  const Packets::ReadRequestPacket &readRequestPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)<< "RX: " <<
    static_cast< std::string>( readRequestPacket);

  // check handler
  if (!handler)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
    "No registered handler - reject";

    auto operation( createErrorOperation(
      from,
      ErrorCode::FileNotFound,
      "RRQ not accepted"));

    // execute error operation
    (*operation)();
  }

  // call the handler, which handles the received request
  handler(
    RequestType::Read,
    readRequestPacket.getFilename(),
    readRequestPacket.getMode(),
    readRequestPacket.getOptions(),
    from);
}

void TftpServerImpl::handleWriteRequestPacket(
  const UdpAddressType &from,
  const Packets::WriteRequestPacket &writeRequestPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)<< "RX: " <<
    static_cast< std::string>( writeRequestPacket);

  // check handler
  if (!handler)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
      << "No registered handler - reject";

    auto operation( createErrorOperation(
      from,
      ErrorCode::FileNotFound,
      "WRQ"));

    // execute error operation
    (*operation)();
  }

  // call the handler, which handles the received request
  handler(
    RequestType::Write,
    writeRequestPacket.getFilename(),
    writeRequestPacket.getMode(),
    writeRequestPacket.getOptions(),
    from);
}

void TftpServerImpl::handleDataPacket(
  const UdpAddressType &from,
  const Packets::DataPacket &dataPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX ERROR: " <<
    static_cast< std::string>( dataPacket);

  auto operation( createErrorOperation(
    from,
    ErrorCode::IllegalTftpOperation,
    "DATA not expected"));

  // execute error operation
  (*operation)();
}

void TftpServerImpl::handleAcknowledgementPacket(
  const UdpAddressType &from,
  const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX ERROR: " <<
    static_cast< std::string>( acknowledgementPacket);

  auto operation( createErrorOperation(
    from,
    ErrorCode::IllegalTftpOperation,
    "ACK not expected"));

  // execute error operation
  (*operation)();
}

void TftpServerImpl::handleErrorPacket(
  const UdpAddressType &from,
  const Packets::ErrorPacket &errorPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX ERROR: " <<
    static_cast< std::string>( errorPacket);

  auto operation( createErrorOperation(
    from,
    ErrorCode::IllegalTftpOperation,
    "ERROR not expected"));

  // execute error operation
  (*operation)();
}

void TftpServerImpl::handleOptionsAcknowledgementPacket(
  const UdpAddressType &from,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX ERROR: " <<
    static_cast< std::string>( optionsAcknowledgementPacket);

  auto operation( createErrorOperation(
    from,
    ErrorCode::IllegalTftpOperation,
    "OACK not expected"));

  // execute error operation
  (*operation)();
}

void TftpServerImpl::handleInvalidPacket(
  const UdpAddressType &,
  const RawTftpPacketType &)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
    "RX: UNKNOWN: *ERROR* - IGNORE";
}

}
}
