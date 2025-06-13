// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Servers::ServerImpl.
 **/

#include "ServerImpl.hpp"

#include <tftp/servers/implementation/ReadOperationImpl.hpp>
#include <tftp/servers/implementation/WriteOperationImpl.hpp>

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/Options.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>
#include <tftp/packets/PacketStatistic.hpp>
#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>

#include <tftp/TftpException.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

#include <boost/bind/bind.hpp>

namespace Tftp::Servers {

ServerImpl::ServerImpl( boost::asio::io_context &ioContext ) :
  ioContextV{ ioContext },
  socketV{ ioContextV },
  receivePacketV( Packets::DefaultMaxPacketSize )
{
}

ServerImpl::~ServerImpl() = default;

Server& ServerImpl::requestHandler( ReceivedTftpRequestHandler handler )
{
  requestHandlerV = std::move( handler );
  return *this;
}

Server& ServerImpl::serverAddress( boost::asio::ip::udp::endpoint serverAddress )
{
  serverAddressV = std::move( serverAddress );
  return *this;
}

boost::asio::ip::udp::endpoint ServerImpl::localEndpoint() const
{
  return socketV.local_endpoint();
}

Server& ServerImpl::tftpTimeoutDefault( const std::chrono::seconds timeout )
{
  tftpTimeoutDefaultV = timeout;
  return *this;
}

Server& ServerImpl::tftpRetriesDefault( const uint16_t retries )
{
  tftpRetriesDefaultV = retries;
  return *this;
}

Server& ServerImpl::dallyDefault( const bool dally )
{
  dallyDefaultV = dally;
  return *this;
}

Server& ServerImpl::optionsConfigurationDefault( TftpOptionsConfiguration optionsConfiguration )
{
  optionsConfigurationDefaultV = std::move( optionsConfiguration );
  return *this;
}

Server& ServerImpl::localDefault( boost::asio::ip::address local )
{
  localV = std::move( local );
  return *this;
}

void ServerImpl::start()
{
  SPDLOG_INFO( "Start TFTP Server on {}:{}", serverAddressV.address().to_string(), serverAddressV.port() );

  try
  {
    socketV.open( serverAddressV.protocol() );
    socketV.bind( serverAddressV );

    // start receive
    receive();
  }
  catch ( const boost::system::system_error &err )
  {
    // close socket when opened
    if ( socketV.is_open() )
    {
      socketV.close();
    }

    BOOST_THROW_EXCEPTION( CommunicationException()
      << Helper::AdditionalInfo{ err.what() }
      << TransferPhaseInfo{ TransferPhase::Initialisation } );
  }
}

void ServerImpl::stop()
{
  SPDLOG_INFO( "Stop TFTP Server" );

  socketV.cancel();
  socketV.close();
}

ReadOperationPtr ServerImpl::readOperation()
{
  auto operation{ std::make_shared< ReadOperationImpl >( ioContextV ) };

  if ( tftpTimeoutDefaultV )
  {
    operation->tftpTimeout( *tftpTimeoutDefaultV );
  }

  if ( tftpRetriesDefaultV )
  {
    operation->tftpRetries( *tftpRetriesDefaultV );
  }

  if ( optionsConfigurationDefaultV )
  {
    operation->optionsConfiguration( *optionsConfigurationDefaultV );
  }

  if ( !localV.is_unspecified() )
  {
    operation->local( { localV, 0 } );
  }

  return operation;
}

WriteOperationPtr ServerImpl::writeOperation()
{
  auto operation{ std::make_shared< WriteOperationImpl >( ioContextV ) };

  if ( tftpTimeoutDefaultV )
  {
    operation->tftpTimeout( *tftpTimeoutDefaultV );
  }

  if ( tftpRetriesDefaultV )
  {
    operation->tftpRetries( *tftpRetriesDefaultV );
  }

  if ( dallyDefaultV )
  {
    operation->dally( *dallyDefaultV );
  }

  if ( optionsConfigurationDefaultV )
  {
    operation->optionsConfiguration( *optionsConfigurationDefaultV );
  }

  if ( !localV.is_unspecified() )
  {
    operation->local( { localV, 0 } );
  }

  return operation;
}

void ServerImpl::errorOperation(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::ErrorCode errorCode,
  std::string errorMessage )
{
  const Packets::ErrorPacket errorPacket{ errorCode, std::move( errorMessage ) };

  SPDLOG_INFO( "TX: {}", static_cast< std::string>( errorPacket ) );

  try
  {
    boost::asio::ip::udp::socket errSocket{ ioContextV };

    errSocket.open( remote.protocol() );

    errSocket.connect( remote );

    auto rawPacket{ static_cast< Helper::RawData >( errorPacket ) };

    // Update statistic
    Packets::PacketStatistic::globalTransmit().packet( errorPacket.packetType(), rawPacket.size() );

    errSocket.send( boost::asio::buffer( rawPacket ) );
  }
  catch ( const boost::system::system_error &err )
  {
    SPDLOG_ERROR( err.what() );
  }
}

void ServerImpl::errorOperation(
  const boost::asio::ip::udp::endpoint &remote,
  const boost::asio::ip::udp::endpoint &local,
  const Packets::ErrorCode errorCode,
  std::string errorMessage )
{
  const Packets::ErrorPacket errorPacket{ errorCode, std::move( errorMessage ) };

  SPDLOG_INFO( "TX: {}", static_cast< std::string >( errorPacket ) );

  try
  {
    boost::asio::ip::udp::socket errSocket{ ioContextV };

    errSocket.open( remote.protocol() );

    errSocket.bind( local );

    errSocket.connect( remote );

    auto rawPacket{ static_cast< Helper::RawData >( errorPacket ) };

    // Update statistic
    Packets::PacketStatistic::globalTransmit().packet( errorPacket.packetType(), rawPacket.size() );

    errSocket.send( boost::asio::buffer( rawPacket ) );
  }
  catch ( const boost::system::system_error &err )
  {
    SPDLOG_ERROR( err.what() );
  }
}

void ServerImpl::receive()
{
  try
  {
    // wait for incoming packet
    socketV.async_receive_from(
      boost::asio::buffer( receivePacketV ),
      remoteEndpointV,
      std::bind_front( &ServerImpl::receiveHandler, this ) );
  }
  catch ( const boost::system::system_error &err )
  {
    BOOST_THROW_EXCEPTION( CommunicationException()
      << Helper::AdditionalInfo{ err.what() }
      << TransferPhaseInfo{ TransferPhase::Initialisation } );
  }
}

void ServerImpl::receiveHandler( const boost::system::error_code &errorCode, const std::size_t bytesTransferred )
{
  // handle abort
  if ( boost::asio::error::operation_aborted == errorCode )
  {
    return;
  }

  // Check error
  if ( errorCode )
  {
    SPDLOG_ERROR( "receive error: " + errorCode.message() );

    BOOST_THROW_EXCEPTION( CommunicationException()
      << Helper::AdditionalInfo{ errorCode.message() } );
  }

  try
  {
    // handle the received packet (decode it and call the appropriate handler)
    packet( remoteEndpointV, Helper::ConstRawDataSpan{ receivePacketV.begin(), bytesTransferred } );
  }
  catch ( const TftpException &e )
  {
    SPDLOG_ERROR( "TFTP exception: {}", e.what() );
  }

  receive();
}

void ServerImpl::readRequestPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::ReadRequestPacket &readRequestPacket )
{
  SPDLOG_TRACE( "RX: {}", static_cast< std::string>( readRequestPacket ) );

  // check handler
  if ( !requestHandlerV )
  {
    SPDLOG_WARN( "No registered handler - reject" );

    // execute error operation
    errorOperation( remote, Packets::ErrorCode::FileNotFound, "RRQ not accepted" );
  }

  // extract known TFTP Options
  auto receivedOptions{ readRequestPacket.options() };
  auto decodedOptions{ tftpOptions( receivedOptions ) };

  // call the handler, which handles the received request
  requestHandlerV(
    remote,
    RequestType::Read,
    readRequestPacket.filename(),
    readRequestPacket.mode(),
    decodedOptions,
    receivedOptions );
}

void ServerImpl::writeRequestPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::WriteRequestPacket &writeRequestPacket )
{
  SPDLOG_TRACE( "RX: {}", static_cast< std::string>( writeRequestPacket ) );

  // check handler
  if ( !requestHandlerV )
  {
    SPDLOG_WARN( "No registered handler - reject" );

    // execute error operation
    errorOperation( remote, Packets::ErrorCode::FileNotFound, "WRQ" );
  }

  // extract known TFTP Options
  auto receivedOptions{ writeRequestPacket.options() };
  auto decodedOptions{ tftpOptions( receivedOptions ) };

  // call the handler, which handles the received request
  requestHandlerV(
    remote,
    RequestType::Write,
    writeRequestPacket.filename(),
    writeRequestPacket.mode(),
    decodedOptions,
    receivedOptions );
}

void ServerImpl::dataPacket( const boost::asio::ip::udp::endpoint &remote, const Packets::DataPacket &dataPacket )
{
  SPDLOG_WARN( "RX Error: {}", static_cast< std::string >( dataPacket ) );

  // execute error operation
  errorOperation( remote, Packets::ErrorCode::IllegalTftpOperation, "DATA packet not expected" );
}

void ServerImpl::acknowledgementPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  SPDLOG_WARN( "RX Error: {}", static_cast< std::string >( acknowledgementPacket ) );

  // execute error operation
  errorOperation( remote, Packets::ErrorCode::IllegalTftpOperation, "ACK packet not expected" );
}

void ServerImpl::errorPacket( const boost::asio::ip::udp::endpoint &remote, const Packets::ErrorPacket &errorPacket )
{
  SPDLOG_WARN( "RX Error: {}", static_cast< std::string>( errorPacket ) );

  // execute error operation
  errorOperation( remote, Packets::ErrorCode::IllegalTftpOperation, "ERR packet not expected" );
}

void ServerImpl::optionsAcknowledgementPacket(
  const boost::asio::ip::udp::endpoint &remote,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket )
{
  SPDLOG_WARN( "RX Error: {}", static_cast< std::string >( optionsAcknowledgementPacket ) );

  // execute error operation
  errorOperation( remote, Packets::ErrorCode::IllegalTftpOperation, "OACK packet not expected" );
}

void ServerImpl::invalidPacket(
  [[maybe_unused]] const boost::asio::ip::udp::endpoint &remote,
  [[maybe_unused]] Helper::ConstRawDataSpan rawPacket )
{
  SPDLOG_WARN( "RX: UNKNOWN: *Error* - IGNORE" );
}

Packets::TftpOptions ServerImpl::tftpOptions( Packets::Options &clientOptions ) const
{
  Packets::TftpOptions decodedOptions;

  // check block size option - if set use it
  const auto [ blockSizeValid, blockSize ] =
    Packets::Options_getOption< uint16_t >(
      clientOptions,
      Packets::TftpOptions_name( Packets::KnownOptions::BlockSize ),
      Packets::BlockSizeOptionMin,
      Packets::BlockSizeOptionMax );

  decodedOptions.blockSize = blockSize;
  // remove block size option
  // TODO remove std::string generation if P2077R3 is implemented within stdlibc++ (GCC)
  clientOptions.erase( std::string{ Packets::TftpOptions_name( Packets::KnownOptions::BlockSize ) } );

  // check timeout option - if set use it
  const auto [ timeoutValid, timeout ] =
    Packets::Options_getOption< uint8_t >(
      clientOptions,
      Packets::TftpOptions_name( Packets::KnownOptions::Timeout ),
      Packets::TimeoutOptionMin,
      Packets::TimeoutOptionMax );

  decodedOptions.timeout = timeout;
  // remove timeout option
  // TODO remove std::string generation if P2077R3 is implemented within stdlibc++ (GCC)
  clientOptions.erase( std::string{ Packets::TftpOptions_name( Packets::KnownOptions::Timeout ) } );

  // check transfer size option
  const auto [ transferSizeValid, transferSize ] = Packets::Options_getOption< uint64_t >(
    clientOptions,
    Packets::TftpOptions_name( Packets::KnownOptions::TransferSize ) );

  decodedOptions.transferSize = transferSize;
  // remove timeout option
  // TODO remove std::string generation if P2077R3 is implemented within stdlibc++ (GCC)
  clientOptions.erase( std::string{ Packets::TftpOptions_name( Packets::KnownOptions::TransferSize ) } );

  return decodedOptions;
}

}
