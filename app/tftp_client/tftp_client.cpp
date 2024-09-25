// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief TFTP Client CLI Application.
 **/

#include "tftp/Tftp.hpp"
#include "tftp/TftpConfiguration.hpp"
#include "tftp/TftpOptionsConfiguration.hpp"
#include "tftp/TftpException.hpp"

#include "tftp/RequestTypeDescription.hpp"

#include "tftp/file/StreamFile.hpp"

#include "tftp/client/ReadOperation.hpp"
#include "tftp/client/TftpClient.hpp"
#include "tftp/client/WriteOperation.hpp"

#include "tftp/packets/PacketStatistic.hpp"

#include "helper/Logger.hpp"
#include "helper/BoostAsioProgramOptions.hpp"

#include <boost/asio.hpp>

#include <boost/exception/all.hpp>

#include <boost/program_options.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

/**
 * @brief Application Entry Point.
 *
 * @param[in] argc
 *   Number of arguments.
 * @param[in] argv
 *   Arguments
 *
 * @return Application exit status.
 **/
int main( int argc, char * argv[] );

/**
 * @brief Option Negotiation callback.
 *
 * Checks Server options.
 * As we don't send any additional options, the received options must be empty.
 *
 * @param[in] serverOptions
 *   Received Server Options.
 *
 * @return if @p serverOptions are empty.
 **/
static bool optionNegotiation(
  const Tftp::Packets::Options &serverOptions );

/**
 * @brief Operation Completed callback
 *
 * @param[in] ioContext
 *   IO Context
 * @param[in] transferStatus
 *   Transfer Status
 **/
static void operationCompleted(
  boost::asio::io_context &ioContext,
  Tftp::TransferStatus transferStatus );

/**
 * @brief Initiates an executes the TFTP Client Read Operation.
 *
 * @param tftpClient
 * @param tftpConfiguration
 * @param tftpOptionsConfiguration
 * @param localFile
 * @param remoteFile
 * @param address
 * @param ioContext
 **/
static Tftp::Client::OperationPtr readOperation(
  Tftp::Client::TftpClientPtr &tftpClient,
  Tftp::TftpConfiguration &tftpConfiguration,
  Tftp::TftpOptionsConfiguration &tftpOptionsConfiguration,
  std::filesystem::path &localFile,
  std::string &remoteFile,
  boost::asio::ip::address &address,
  boost::asio::io_context &ioContext );

/**
 * @brief Initiates an executes the TFTP Client Read Operation.
 *
 * @param tftpClient
 * @param tftpConfiguration
 * @param tftpOptionsConfiguration
 * @param localFile
 * @param remoteFile
 * @param address
 * @param ioContext
 **/
static Tftp::Client::OperationPtr writeOperation(
  Tftp::Client::TftpClientPtr &tftpClient,
  Tftp::TftpConfiguration &tftpConfiguration,
  Tftp::TftpOptionsConfiguration &tftpOptionsConfiguration,
  std::filesystem::path &localFile,
  std::string &remoteFile,
  boost::asio::ip::address &address,
  boost::asio::io_context &ioContext );

int main( int argc, char * argv[] )
{
  BOOST_LOG_FUNCTION()

  Helper::initLogging();

  try
  {
    Tftp::RequestType requestType{};
    std::filesystem::path localFile{};
    std::string remoteFile{};
    boost::asio::ip::address address{};
    Tftp::TftpConfiguration tftpConfiguration{};
    Tftp::TftpOptionsConfiguration tftpOptionsConfiguration{};

    boost::program_options::options_description optionsDescription{
      "TFTP Client Options" };

    optionsDescription.add_options()
    (
      "help",
      "print this help screen"
    )
    (
      "request-type",
      boost::program_options::value( &requestType )->required(),
      R"(the desired operation ("Read"|"Write" ))"
    )
    (
      "local-file",
      boost::program_options::value( &localFile ),
      "filename of local file"
    )
    (
      "remote-file",
      boost::program_options::value( &remoteFile )->required(),
      "filename of remote file"
    )
    (
      "address",
      boost::program_options::value( &address )->required(),
      "remote address"
    );

    // Add common TFTP options
    optionsDescription.add( tftpConfiguration.options() );
    optionsDescription.add( tftpOptionsConfiguration.options() );

    std::cout << "TFTP Client\n";

    boost::program_options::variables_map variablesMap{};
    boost::program_options::store(
      boost::program_options::parse_command_line(
        argc,
        argv,
        optionsDescription ),
      variablesMap );

    if ( 0U != variablesMap.count( "help" ) )
    {
      std::cout
        << "Performs TFTP Client transfer\n"
        << optionsDescription << "\n";
      return EXIT_FAILURE;
    }

    boost::program_options::notify( variablesMap );

    // Assemble TFTP configuration
    boost::asio::io_context ioContext;

    auto tftpClient{ Tftp::Client::TftpClient::instance( ioContext ) };

    Tftp::Client::OperationPtr tftpOperation{};

    if ( localFile.empty() )
    {
      localFile = std::filesystem::path{ remoteFile }.filename();
    }

    std::cout << std::format(
      "{} request to {} '{}'<->'{}'\n",
      Tftp::RequestTypeDescription::instance().name( requestType ),
      address.to_string(),
      remoteFile,
      localFile.string() );

    switch ( requestType )
    {
      case Tftp::RequestType::Read:
        tftpOperation = readOperation(
          tftpClient,
          tftpConfiguration,
          tftpOptionsConfiguration,
          localFile,
          remoteFile,
          address,
          ioContext );
        break;

      case Tftp::RequestType::Write:
        tftpOperation = writeOperation(
          tftpClient,
          tftpConfiguration,
          tftpOptionsConfiguration,
          localFile,
          remoteFile,
          address,
          ioContext );
        break;

      default:
        std::cerr << "Internal invalid operation\n";
        return EXIT_FAILURE;
    }

    assert( tftpOperation );

    // start request
    tftpOperation->request();

    // Start client and its operations
    ioContext.run();

    // Print Packet Statistic
    std::cout
      << "RX:\n" << Tftp::Packets::PacketStatistic::globalReceive() << "\n"
      << "TX:\n" << Tftp::Packets::PacketStatistic::globalTransmit() << "\n";

    return EXIT_SUCCESS;
  }
  catch ( const boost::program_options::error &e )
  {
    std::cerr
      << "Error parsing command line: " << e.what() << "\n"
      << "Enter " << argv[0]
      << " --help for command line description\n";
    return EXIT_FAILURE;
  }
  catch ( const boost::exception &e )
  {
    std::cerr
      << "Error: "
      << boost::diagnostic_information( e ) << "\n";
    return EXIT_FAILURE;
  }
  catch ( const std::exception &e )
  {
    std::cerr
      << "Error: "
      << boost::diagnostic_information( e ) << "\n";
    return EXIT_FAILURE;
  }
  catch ( ... )
  {
    std::cerr << "Unknown exception occurred\n";
    return EXIT_FAILURE;
  }
}

static bool optionNegotiation(
  const Tftp::Packets::Options &serverOptions )
{
  return serverOptions.empty();
}

static void operationCompleted(
  boost::asio::io_context &ioContext,
  [[maybe_unused]] Tftp::TransferStatus transferStatus )
{
  ioContext.stop();
}

static Tftp::Client::OperationPtr readOperation(
  Tftp::Client::TftpClientPtr &tftpClient,
  Tftp::TftpConfiguration &tftpConfiguration,
  Tftp::TftpOptionsConfiguration &tftpOptionsConfiguration,
  std::filesystem::path &localFile,
  std::string &remoteFile,
  boost::asio::ip::address &address,
  boost::asio::io_context &ioContext )
{
  auto tftpOperation{ tftpClient->readOperation() };

  tftpOperation
    ->tftpTimeout( tftpConfiguration.tftpTimeout )
    .tftpRetries( tftpConfiguration.tftpRetries )
    .dally( tftpConfiguration.dally )
    .optionsConfiguration( tftpOptionsConfiguration )
    .optionNegotiationHandler( std::bind_front( &optionNegotiation ) )
    .completionHandler( std::bind_front( &operationCompleted, std::ref( ioContext ) ) )
    .dataHandler( std::make_shared< Tftp::File::StreamFile >(
      Tftp::File::TftpFile::Operation::Receive,
      localFile ) )
    .filename( remoteFile )
    .mode( Tftp::Packets::TransferMode::OCTET )
    .remote( boost::asio::ip::udp::endpoint{address,tftpConfiguration.tftpServerPort } );

  return tftpOperation;
}

static  Tftp::Client::OperationPtr writeOperation(
  Tftp::Client::TftpClientPtr &tftpClient,
  Tftp::TftpConfiguration &tftpConfiguration,
  Tftp::TftpOptionsConfiguration &tftpOptionsConfiguration,
  std::filesystem::path &localFile,
  std::string &remoteFile,
  boost::asio::ip::address &address,
  boost::asio::io_context &ioContext )
{
  auto tftpOperation{ tftpClient->writeOperation() };

  tftpOperation
    ->tftpTimeout( tftpConfiguration.tftpTimeout )
    .tftpRetries( tftpConfiguration.tftpRetries )
    .optionsConfiguration( tftpOptionsConfiguration )
    .optionNegotiationHandler( std::bind_front( &optionNegotiation ) )
    .completionHandler( std::bind_front( &operationCompleted, std::ref( ioContext ) ) )
    .dataHandler( std::make_shared< Tftp::File::StreamFile >(
      Tftp::File::TftpFile::Operation::Transmit,
      localFile,
      std::filesystem::file_size( localFile ) ) )
    .filename( remoteFile )
    .mode( Tftp::Packets::TransferMode::OCTET )
    .remote( boost::asio::ip::udp::endpoint{address,tftpConfiguration.tftpServerPort } );

  return tftpOperation;
}
