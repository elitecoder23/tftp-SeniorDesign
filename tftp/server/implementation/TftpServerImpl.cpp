/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Server::TftpServerImpl.
 **/

#include "TftpServerImpl.hpp"

#include <tftp/server/implementation/ReadRequestOperationImpl.hpp>
#include <tftp/server/implementation/WriteRequestOperationImpl.hpp>
#include <tftp/server/implementation/ErrorOperation.hpp>

#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>

#include <boost/bind.hpp>

#include <vector>
#include <cstdint>

namespace Tftp::Server {

TftpServerImpl::TftpServerImpl(
  ReceivedTftpRequestHandler handler,
  const TftpConfiguration &configuration,
  const boost::asio::ip::udp::endpoint &serverAddress)
try :
  handler{ handler},
  configurationV{ configuration},
  serverAddress{ serverAddress},
  work{ ioContext},
  socket{ ioContext}
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

    BOOST_THROW_EXCEPTION( CommunicationException()
      << AdditionalInfo( err.what())
      << TransferPhaseInfo( TransferPhase::Initialisation));
  }
}
catch ( boost::system::system_error &err)
{
  BOOST_THROW_EXCEPTION( CommunicationException()
    << AdditionalInfo( err.what())
    << TransferPhaseInfo( TransferPhase::Initialisation));
}

TftpServerImpl::~TftpServerImpl() noexcept
{
}

void TftpServerImpl::entry() noexcept
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
    << "Start TFTP server I/O context";

  try
  {
    ioContext.run();
  }
  catch (boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::fatal)
      << "IO error: " << err.what();
  }

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
    << "TFTP server I/O context finished";
}

void TftpServerImpl::start()
{
  BOOST_LOG_FUNCTION()

  // start receive
  receive();
}

void TftpServerImpl::stop()
{
  // cancel receive operation
  socket.cancel();
  //! @todo cancel TFTP server operations.

  // stop handler
  ioContext.stop();
}

OperationPtr TftpServerImpl::readRequestOperation(
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  const Options::Options &clientOptions,
  const Options::OptionList& serverOptions)
{
  return std::make_shared< ReadRequestOperationImpl>(
    ioContext,
    *this,
    dataHandler,
    completionHandler,
    remote,
    clientOptions,
    serverOptions);
}

OperationPtr TftpServerImpl::readRequestOperation(
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  const Options::Options &clientOptions,
  const Options::OptionList& serverOptions,
  const boost::asio::ip::udp::endpoint &local)
{
  return std::make_shared< ReadRequestOperationImpl>(
    ioContext,
    *this,
    dataHandler,
    completionHandler,
    remote,
    clientOptions,
    serverOptions,
    local);
}

OperationPtr TftpServerImpl::writeRequestOperation(
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  const Options::Options &clientOptions,
  const Options::OptionList& serverOptions)
{
  return std::make_shared< WriteRequestOperationImpl>(
    ioContext,
    *this,
    dataHandler,
    completionHandler,
    remote,
    clientOptions,
    serverOptions);
}

OperationPtr TftpServerImpl::writeRequestOperation(
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  const Options::Options &clientOptions,
  const Options::OptionList& serverOptions,
  const boost::asio::ip::udp::endpoint &local)
{
  return std::make_shared< WriteRequestOperationImpl>(
    ioContext,
    *this,
    dataHandler,
    completionHandler,
    remote,
    clientOptions,
    serverOptions,
    local);
}

OperationPtr TftpServerImpl::errorOperation(
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  const boost::asio::ip::udp::endpoint &local,
  const ErrorCode errorCode,
  std::string_view errorMessage)
{
  return std::make_shared< ErrorOperation>(
    ioContext,
    completionHandler,
    remote,
    local,
    errorCode,
    errorMessage);
}

const Tftp::TftpConfiguration& TftpServerImpl::configuration() const
{
  return configurationV;
}

void TftpServerImpl::receive()
{
  try
  {
    receivePacket.resize( DefaultMaxPacketSize);

    // wait for incoming packet
    socket.async_receive_from(
      boost::asio::buffer( receivePacket),
      remoteEndpoint,
      boost::bind(
        &TftpServerImpl::receiveHandler,
        this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
  }
  catch ( boost::system::system_error &err)
  {
    ioContext.stop();

    BOOST_THROW_EXCEPTION(CommunicationException()
      << AdditionalInfo( err.what())
      << TransferPhaseInfo( TransferPhase::Initialisation));
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
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
      << "receive error: " + errorCode.message();

    BOOST_THROW_EXCEPTION( CommunicationException()
      << AdditionalInfo( errorCode.message()));
  }

  try
  {
    // resize buffer to actual size
    receivePacket.resize( bytesTransferred);

    // handle the received packet (decode it and call the approbate handler)
    packet( remoteEndpoint, receivePacket);
  }
  catch ( TftpException &e)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
      << "TFTP exception: " << e.what();
  }

  receive();
}

void TftpServerImpl::readRequestPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::ReadRequestPacket &readRequestPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
    << "RX: " << static_cast< std::string>( readRequestPacket);

  // check handler
  if (!handler)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning)
      << "No registered handler - reject";

    using namespace std::string_view_literals;
    auto operation( errorOperation(
      {},
      remote,
      boost::asio::ip::udp::endpoint{ serverAddress.address(), 0},
      ErrorCode::FileNotFound,
      "RRQ not accepted"sv));

    // execute error operation
    operation->start();
  }

  // call the handler, which handles the received request
  handler(
    remote,
    RequestType::Read,
    readRequestPacket.filename(),
    readRequestPacket.mode(),
    readRequestPacket.options());
}

void TftpServerImpl::writeRequestPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::WriteRequestPacket &writeRequestPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
    << "RX: " << static_cast< std::string>( writeRequestPacket);

  // check handler
  if (!handler)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning)
      << "No registered handler - reject";

    using namespace std::string_view_literals;
    auto operation( errorOperation(
      {},
      remote,
      boost::asio::ip::udp::endpoint{ serverAddress.address(), 0},
      ErrorCode::FileNotFound,
      "WRQ"sv));

    // execute error operation
    operation->start();
  }

  // call the handler, which handles the received request
  handler(
    remote,
    RequestType::Write,
    writeRequestPacket.filename(),
    writeRequestPacket.mode(),
    writeRequestPacket.options());
}

void TftpServerImpl::dataPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::DataPacket &dataPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning)
    << "RX ERROR: " << static_cast< std::string>( dataPacket);

  using namespace std::string_view_literals;
  auto operation( errorOperation(
    {},
    remote,
    boost::asio::ip::udp::endpoint{ serverAddress.address(), 0},
    ErrorCode::IllegalTftpOperation,
    "DATA not expected"sv));

  // execute error operation
  operation->start();
}

void TftpServerImpl::acknowledgementPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning)
    << "RX ERROR: " << static_cast< std::string>( acknowledgementPacket);

  using namespace std::string_view_literals;
  auto operation( errorOperation(
    {},
    remote,
    boost::asio::ip::udp::endpoint{ serverAddress.address(), 0},
    ErrorCode::IllegalTftpOperation,
    "ACK not expected"sv));

  // execute error operation
  operation->start();
}

void TftpServerImpl::errorPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::ErrorPacket &errorPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning)
    << "RX ERROR: " << static_cast< std::string>( errorPacket);

  using namespace std::string_view_literals;
  auto operation( errorOperation(
    {},
    remote,
    boost::asio::ip::udp::endpoint{ serverAddress.address(), 0},
    ErrorCode::IllegalTftpOperation,
    "ERROR not expected"sv));

  // execute error operation
  operation->start();
}

void TftpServerImpl::optionsAcknowledgementPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning)
    << "RX ERROR: " << static_cast< std::string>( optionsAcknowledgementPacket);

  using namespace std::string_view_literals;
  auto operation( errorOperation(
    {},
    remote,
    boost::asio::ip::udp::endpoint{ serverAddress.address(), 0},
    ErrorCode::IllegalTftpOperation,
    "OACK not expected"sv));

  // execute error operation
  operation->start();
}

void TftpServerImpl::invalidPacket(
  const boost::asio::ip::udp::endpoint &,
  const RawTftpPacket &)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::warning)
    << "RX: UNKNOWN: *ERROR* - IGNORE";
}

}
