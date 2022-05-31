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

#include <tftp/server/implementation/ReadOperationImpl.hpp>
#include <tftp/server/implementation/WriteOperationImpl.hpp>

#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>

#include <boost/bind/bind.hpp>

#include <cstdint>

namespace Tftp::Server {

TftpServerImpl::TftpServerImpl(
  boost::asio::io_context &ioContext,
  ServerConfiguration configuration )
try :
  ioContext{ ioContext },
  socket{ ioContext },
  configurationV{ std::move( configuration ) },
  receivePacket( DefaultMaxPacketSize )
{
  try
  {
    // open the socket
    socket.open(
      configurationV.serverAddress.value_or( DefaultLocalEndpoint ).protocol() );

    // bind to the local address
    socket.bind( configurationV.serverAddress.value_or( DefaultLocalEndpoint ) );
  }
  catch ( const boost::system::system_error &err )
  {
    // close socket when opened
    if ( socket.is_open() )
    {
      socket.close();
    }

    BOOST_THROW_EXCEPTION( CommunicationException()
      << Helper::AdditionalInfo{ err.what() }
      << TransferPhaseInfo{ TransferPhase::Initialisation } );
  }
}
catch ( const boost::system::system_error &err )
{
  BOOST_THROW_EXCEPTION( CommunicationException()
    << Helper::AdditionalInfo{ err.what() }
    << TransferPhaseInfo{ TransferPhase::Initialisation } );
}

TftpServerImpl::~TftpServerImpl() noexcept
{
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
}

OperationPtr TftpServerImpl::readOperation(
  ReadOperationConfiguration configuration )
{
  return std::make_shared< ReadOperationImpl >(
    ioContext,
    configurationV.tftpTimeout,
    configurationV.tftpRetries,
    std::move( configuration ) );
}

OperationPtr TftpServerImpl::writeOperation(
  WriteOperationConfiguration configuration )
{
  return std::make_shared< WriteOperationImpl >(
    ioContext,
    configurationV.tftpTimeout,
    configurationV.tftpRetries,
    configurationV.dally,
    std::move( configuration ) );
}

void TftpServerImpl::errorOperation(
  const boost::asio::ip::udp::endpoint &remote,
  const ErrorCode errorCode,
  std::string_view errorMessage )
{
  BOOST_LOG_FUNCTION()

  Packets::ErrorPacket errorPacket{ errorCode, errorMessage };

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "TX: " << static_cast< std::string>( errorPacket );

  try
  {
    boost::asio::ip::udp::socket errSocket{ ioContext };

    errSocket.open( remote.protocol() );

    errSocket.connect( remote);

    errSocket.send( boost::asio::buffer(
      static_cast< Packets::RawTftpPacket>( errorPacket ) ) );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << err.what();
  }
}

void TftpServerImpl::errorOperation(
  const boost::asio::ip::udp::endpoint &remote,
  const boost::asio::ip::udp::endpoint &local,
  const ErrorCode errorCode,
  std::string_view errorMessage )
{
  BOOST_LOG_FUNCTION()

  Packets::ErrorPacket errorPacket{ errorCode, errorMessage };

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "TX: " << static_cast< std::string>( errorPacket );

  try
  {
    boost::asio::ip::udp::socket errSocket{ ioContext };

    errSocket.open( remote.protocol() );

    errSocket.bind( local );

    errSocket.connect( remote );

    errSocket.send( boost::asio::buffer(
      static_cast< Packets::RawTftpPacket>( errorPacket ) ) );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << err.what();
  }
}

void TftpServerImpl::receive()
{
  try
  {
    // wait for incoming packet
    socket.async_receive_from(
      boost::asio::buffer( receivePacket ),
      remoteEndpoint,
      boost::bind(
        &TftpServerImpl::receiveHandler,
        this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred ) );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_THROW_EXCEPTION( CommunicationException()
      << Helper::AdditionalInfo{ err.what() }
      << TransferPhaseInfo{ TransferPhase::Initialisation } );
  }
}

void TftpServerImpl::receiveHandler(
  const boost::system::error_code& errorCode,
  const std::size_t bytesTransferred )
{
  // handle abort
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    return;
  }

  // Check error
  if ( errorCode)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "receive error: " + errorCode.message();

    BOOST_THROW_EXCEPTION( CommunicationException()
      << Helper::AdditionalInfo{ errorCode.message() } );
  }

  try
  {
    // handle the received packet (decode it and call the appropriate handler)
    packet(
      remoteEndpoint,
      Packets::ConstRawTftpPacketSpan{ receivePacket.begin(), bytesTransferred } );
  }
  catch ( const TftpException &e )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "TFTP exception: " << e.what();
  }

  receive();
}

void TftpServerImpl::readRequestPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::ReadRequestPacket &readRequestPacket )
{
  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "RX: " << static_cast< std::string>( readRequestPacket );

  // check handler
  if ( !configurationV.handler )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::warning )
      << "No registered handler - reject";

    // execute error operation
    errorOperation(
      remote,
      ErrorCode::FileNotFound,
      "RRQ not accepted" );
  }

  // extract known TFTP Options
  auto receivedOptions{ readRequestPacket.options() };
  Options clientOptions{};
  clientOptions.insert( receivedOptions.extract(
    std::string{ Packets::TftpOptions_name( KnownOptions::BlockSize ) } ) ) ;
  clientOptions.insert( receivedOptions.extract(
    std::string{ Packets::TftpOptions_name( KnownOptions::Timeout ) } ) ) ;
  clientOptions.insert( receivedOptions.extract(
    std::string{ Packets::TftpOptions_name( KnownOptions::TransferSize ) } ) ) ;

  // call the handler, which handles the received request
  configurationV.handler(
    remote,
    RequestType::Read,
    readRequestPacket.filename(),
    readRequestPacket.mode(),
    clientOptions,
    receivedOptions );
}

void TftpServerImpl::writeRequestPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::WriteRequestPacket &writeRequestPacket )
{
  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info)
    << "RX: " << static_cast< std::string>( writeRequestPacket );

  // check handler
  if ( !configurationV.handler)
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::warning )
      << "No registered handler - reject";

    // execute error operation
    errorOperation(
      remote,
      ErrorCode::FileNotFound,
      "WRQ" );
  }

  // extract known TFTP Options
  auto receivedOptions{ writeRequestPacket.options() };
  Options clientOptions{};
  clientOptions.insert( receivedOptions.extract(
    std::string{ Packets::TftpOptions_name( KnownOptions::BlockSize ) } ) ) ;
  clientOptions.insert( receivedOptions.extract(
    std::string{ Packets::TftpOptions_name( KnownOptions::Timeout ) } ) ) ;
  clientOptions.insert( receivedOptions.extract(
    std::string{ Packets::TftpOptions_name( KnownOptions::TransferSize ) } ) ) ;

  // call the handler, which handles the received request
  configurationV.handler(
    remote,
    RequestType::Write,
    writeRequestPacket.filename(),
    writeRequestPacket.mode(),
    clientOptions,
    receivedOptions );
}

void TftpServerImpl::dataPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::DataPacket &dataPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::warning )
    << "RX ERROR: " << static_cast< std::string>( dataPacket );

  // execute error operation
  errorOperation(
    remote,
    ErrorCode::IllegalTftpOperation,
    "DATA not expected" );
}

void TftpServerImpl::acknowledgementPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::warning )
    << "RX ERROR: " << static_cast< std::string>( acknowledgementPacket );

  // execute error operation
  errorOperation(
    remote,
    ErrorCode::IllegalTftpOperation,
    "ACK not expected" );
}

void TftpServerImpl::errorPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::ErrorPacket &errorPacket )
{
  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::warning )
    << "RX ERROR: " << static_cast< std::string>( errorPacket );

  // execute error operation
  errorOperation(
    remote,
    ErrorCode::IllegalTftpOperation,
    "ERROR not expected" );
}

void TftpServerImpl::optionsAcknowledgementPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket )
{
  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::warning )
    << "RX ERROR: " << static_cast< std::string>( optionsAcknowledgementPacket );

  // execute error operation
  errorOperation(
    remote,
    ErrorCode::IllegalTftpOperation,
    "OACK not expected" );
}

void TftpServerImpl::invalidPacket(
  const boost::asio::ip::udp::endpoint &,
  Packets::ConstRawTftpPacketSpan )
{
  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::warning )
    << "RX: UNKNOWN: *ERROR* - IGNORE";
}

}
