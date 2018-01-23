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
  configurationV( configuration),
  optionsV( configuration.getServerOptions( additionalOptions)),
  serverAddress( serverAddress),
  work( ioService),
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
}

void TftpServerImpl::entry() noexcept
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
    "Start TFTP server IO service";

  try
  {
    ioService.run();
  }
  catch (boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::fatal)
      << "IO error: " << err.what();
  }

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
    "TFTP server IO service finished";
}

void TftpServerImpl::start()
{
  BOOST_LOG_FUNCTION();

  // start receive
  receive();
}

void TftpServerImpl::stop()
{
  // cancel receive operation
  socket.cancel();
  //! @todo cancel TFTP server operations.

  // stop handler
  ioService.stop();
}

OperationPtr TftpServerImpl::createReadRequestOperation(
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const UdpAddressType &remote,
  const Options::OptionList &clientOptions,
  const UdpAddressType &local)
{
  return std::make_shared< ReadRequestOperationImpl>(
    ioService,
    *this,
    dataHandler,
    completionHandler,
    remote,
    clientOptions,
    local);
}

OperationPtr TftpServerImpl::createWriteRequestOperation(
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const UdpAddressType &remote,
  const Options::OptionList &clientOptions,
  const UdpAddressType &local)
{
  return std::make_shared< WriteRequestOperationImpl>(
    ioService,
    *this,
    dataHandler,
    completionHandler,
    remote,
    clientOptions,
    local);
}

OperationPtr TftpServerImpl::createErrorOperation(
  OperationCompletedHandler completionHandler,
  const UdpAddressType &remote,
  const UdpAddressType &local,
  const ErrorCode errorCode,
  const string &errorMessage)
{
  return std::make_shared< ErrorOperation>(
    ioService,
    completionHandler,
    remote,
    local,
    errorCode,
    errorMessage);
}

OperationPtr TftpServerImpl::createErrorOperation(
  OperationCompletedHandler completionHandler,
  const UdpAddressType &remote,
  const UdpAddressType &local,
  const ErrorCode errorCode,
  string &&errorMessage)
{
  return std::make_shared< ErrorOperation>(
    ioService,
    completionHandler,
    remote,
    local,
    errorCode,
    std::move( errorMessage));
}

const Tftp::TftpConfiguration& TftpServerImpl::configuration() const
{
  return configurationV;
}

const Options::OptionList& TftpServerImpl::options() const
{
  return optionsV;
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
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning) <<
    "No registered handler - reject";

    auto operation( createErrorOperation(
      {},
      from,
      UdpAddressType{ serverAddress.address(), 0},
      ErrorCode::FileNotFound,
      "RRQ not accepted"));

    // execute error operation
    operation->start();
  }

  // call the handler, which handles the received request
  handler(
    RequestType::Read,
    readRequestPacket.filename(),
    readRequestPacket.mode(),
    readRequestPacket.options(),
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
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning)
      << "No registered handler - reject";

    auto operation( createErrorOperation(
      {},
      from,
      UdpAddressType{ serverAddress.address(), 0},
      ErrorCode::FileNotFound,
      "WRQ"));

    // execute error operation
    operation->start();
  }

  // call the handler, which handles the received request
  handler(
    RequestType::Write,
    writeRequestPacket.filename(),
    writeRequestPacket.mode(),
    writeRequestPacket.options(),
    from);
}

void TftpServerImpl::handleDataPacket(
  const UdpAddressType &from,
  const Packets::DataPacket &dataPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning) << "RX ERROR: " <<
    static_cast< std::string>( dataPacket);

  auto operation( createErrorOperation(
    {},
    from,
    UdpAddressType{ serverAddress.address(), 0},
    ErrorCode::IllegalTftpOperation,
    "DATA not expected"));

  // execute error operation
  operation->start();
}

void TftpServerImpl::handleAcknowledgementPacket(
  const UdpAddressType &from,
  const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning) << "RX ERROR: " <<
    static_cast< std::string>( acknowledgementPacket);

  auto operation( createErrorOperation(
    {},
    from,
    UdpAddressType{ serverAddress.address(), 0},
    ErrorCode::IllegalTftpOperation,
    "ACK not expected"));

  // execute error operation
  operation->start();
}

void TftpServerImpl::handleErrorPacket(
  const UdpAddressType &from,
  const Packets::ErrorPacket &errorPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning) << "RX ERROR: " <<
    static_cast< std::string>( errorPacket);

  auto operation( createErrorOperation(
    {},
    from,
    UdpAddressType{ serverAddress.address(), 0},
    ErrorCode::IllegalTftpOperation,
    "ERROR not expected"));

  // execute error operation
  operation->start();
}

void TftpServerImpl::handleOptionsAcknowledgementPacket(
  const UdpAddressType &from,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning) << "RX ERROR: " <<
    static_cast< std::string>( optionsAcknowledgementPacket);

  auto operation( createErrorOperation(
    {},
    from,
    UdpAddressType{ serverAddress.address(), 0},
    ErrorCode::IllegalTftpOperation,
    "OACK not expected"));

  // execute error operation
  operation->start();
}

void TftpServerImpl::handleInvalidPacket(
  const UdpAddressType &,
  const RawTftpPacket &)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning) <<
    "RX: UNKNOWN: *ERROR* - IGNORE";
}

}
}
