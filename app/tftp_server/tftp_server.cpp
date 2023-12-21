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
 * @brief TFTP Server CLI Application.
 **/

#include <tftp/server/TftpServer.hpp>
#include <tftp/server/Operation.hpp>
#include <tftp/server/ServerConfiguration.hpp>
#include <tftp/server/ReadOperationConfiguration.hpp>
#include <tftp/server/WriteOperationConfiguration.hpp>

#include <tftp/file/StreamFile.hpp>

#include <tftp/packets/PacketStatistic.hpp>

#include <tftp/TftpException.hpp>
#include <tftp/TftpConfiguration.hpp>
#include <tftp/TftpOptionsConfiguration.hpp>
#include <tftp/TransferStatusDescription.hpp>
#include <tftp/Version.hpp>

#include <helper/Logger.hpp>

#include <boost/asio.hpp>

#include <boost/exception/all.hpp>

#include <boost/program_options.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <utility>

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
 * @brief Performs Validity Check of supplied Filename.
 *
 * @param[in] filename
 *   Filename to check.
 *
 * @return If filename is valid.
 **/
static bool checkFilename( const std::filesystem::path &filename );

/**
 * @brief Handler for Received TFTP Requests.
 *
 * @param[in] remote
 *   Remote Address.
 * @param[in] requestType
 *   TFTP Request Type (RRQ/ WRQ)
 * @param[in] filename
 *   Requested filename.
 * @param[in] mode
 *   Transfer Mode
 * @param[in] clientOptions
 *   TFTP Options.
 * @param[in] additionalClientOptions
 *   Additional Options.
 **/
static void receivedRequest(
  const boost::asio::ip::udp::endpoint &remote,
  Tftp::RequestType requestType,
  std::string_view filename,
  Tftp::Packets::TransferMode mode,
  const Tftp::Packets::TftpOptions &clientOptions,
  const Tftp::Packets::Options &additionalClientOptions );

/**
 * @brief Transmits a requested file (RRQ).
 *
 * @param[in] remote
 *   Remote address.
 * @param[in] filename
 *   Requested filename.
 * @param[in] clientOptions
 *   Received Options.
 **/
static void transmitFile(
  const boost::asio::ip::udp::endpoint &remote,
  const std::filesystem::path &filename,
  const Tftp::Packets::TftpOptions &clientOptions );

/**
 * @brief Receives a requested file (WRQ).
 *
 * @param[in] remote
 *   Remote address.
 * @param[in] filename
 *   Requested filename.
 * @param[in] clientOptions
 *   Received Options.
 **/
static void receiveFile(
  const boost::asio::ip::udp::endpoint &remote,
  const std::filesystem::path &filename,
  const Tftp::Packets::TftpOptions &clientOptions );

/**
 * @brief Operatrion Completed callback
 *
 * @param[in] operation
 *   TFTP Operation
 * @param[in] transferStatus
 *   Transfer status.
 **/
static void operationCompleted(
  const Tftp::Server::OperationPtr &operation,
  Tftp::TransferStatus transferStatus );

//! TFTP Server Base Directory
static std::filesystem::path baseDir{};

//! TFTP Server Configuration
static Tftp::TftpConfiguration tftpConfiguration{};

//! TFTP Server Options Configuration
static Tftp::TftpOptionsConfiguration tftpOptionsConfiguration{};

//! TFTP Server Instance
static Tftp::Server::TftpServerPtr server{};

int main( int argc, char * argv[] )
{
  BOOST_LOG_FUNCTION()

  Helper::initLogging();

  try
  {
    std::cout << "TFTP Server - " << Tftp::Version::VersionInformation << "\n";

    boost::program_options::options_description optionsDescription{
      "TFTP Server Options" };

    optionsDescription.add_options()
    (
      "help",
      "print this help screen"
    )
    (
      "server-root",
      boost::program_options::value( &baseDir )->default_value(
        std::filesystem::current_path()),
      "Directory path, where the server shall have its root"
    );

    // Add TFTP options
    optionsDescription.add( tftpConfiguration.options() );
    optionsDescription.add( tftpOptionsConfiguration.options() );

    boost::asio::io_context ioContext;
    boost::asio::signal_set signals{ ioContext, SIGINT, SIGTERM };

    boost::program_options::variables_map vm{};
    boost::program_options::store(
      boost::program_options::parse_command_line(
        argc,
        argv,
        optionsDescription ),
      vm );

    if ( 0U != vm.count( "help" ) )
    {
      std::cout
        << "TFTP Server\n"
        << optionsDescription << "\n";
      return EXIT_FAILURE;
    }

    boost::program_options::notify( vm );

    // make an absolute path
    baseDir = std::filesystem::canonical( baseDir );

    std::cout
      << "Starting TFTP server in " << baseDir.string() << "\n";

    // The TFTP server instance
    server = Tftp::Server::TftpServer::instance(
      ioContext,
      Tftp::Server::ServerConfiguration{
        std::bind_front( &receivedRequest ),
        boost::asio::ip::udp::endpoint{
          boost::asio::ip::address_v4::any(),
          tftpConfiguration.tftpServerPort } } );

    std::cout
      << "Listening on " << server->localEndpoint() << "\n";

    server->start();

    // connect to SIGINT and SIGTERM
    signals.async_wait( [](
      const boost::system::error_code& error [[maybe_unused]],
      int signal_number [[maybe_unused]])
    {
      std::cout << "Termination request\n";
      server->stop();
    } );

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

static bool checkFilename( const std::filesystem::path &filename )
{
  if ( filename.is_relative())
  {
    return false;
  }

  if ( std::filesystem::is_directory( filename ) )
  {
    return false;
  }

  auto fileIt{ filename.begin() };

  for ( const auto& pathItem : baseDir )
  {
    if ( fileIt == filename.end() )
    {
      return false;
    }

    if ( *fileIt != pathItem)
    {
      return false;
    }

    ++fileIt;
  }

  return true;
}

static void receivedRequest(
  const boost::asio::ip::udp::endpoint &remote,
  const Tftp::RequestType requestType,
  std::string_view filename,
  const Tftp::Packets::TransferMode mode,
  const Tftp::Packets::TftpOptions &clientOptions,
  const Tftp::Packets::Options &additionalClientOptions )
{
  // Check transfer mode
  if ( mode != Tftp::Packets::TransferMode::OCTET )
  {
    std::cerr << "Wrong transfer mode\n";

    server->errorOperation(
      remote,
      Tftp::Packets::ErrorCode::IllegalTftpOperation,
      "wrong transfer mode" );

    return;
  }

  if ( !checkFilename( ( baseDir / filename ).lexically_normal() ) )
  {
    std::cerr << "Error filename check\n";

    server->errorOperation(
      remote,
      Tftp::Packets::ErrorCode::AccessViolation,
      "Illegal filename" );

    return;
  }

  if ( !additionalClientOptions.empty() )
  {
    std::cout << "Unknown options ";
    for ( const auto &[ optionName, optionValue ] : additionalClientOptions )
    {
      std::cout << "[" << optionName << ":" << optionValue << "]";
    }
    std::cout << "\n";
  }

  switch ( requestType )
  {
    case Tftp::RequestType::Read:
      // we are on server side and transmit the data on RRQ
      transmitFile( remote, baseDir / filename, clientOptions );
      break;

    case Tftp::RequestType::Write:
      // we are on server side and receive the data on WRQ
      receiveFile( remote, baseDir /  filename, clientOptions );
      break;

    default:
      return;
  }
}

static void transmitFile(
  const boost::asio::ip::udp::endpoint &remote,
  const std::filesystem::path &filename,
  const Tftp::Packets::TftpOptions &clientOptions )
{
  std::cout
    << "RRQ: " << filename << " from: " << remote.address().to_string() << "\n";

  // open requested file
  std::fstream fileStream{ filename.c_str(), std::fstream::in };

  // check that file was opened successfully
  if ( !fileStream.good() )
  {
    std::cerr << "Error opening file\n";

    server->errorOperation(
      remote,
      Tftp::Packets::ErrorCode::FileNotFound,
      "file not found" );

    return;
  }

  // initiate TFTP operation
  auto operation{ server->readOperation(
    Tftp::Server::ReadOperationConfiguration{
      tftpConfiguration,
      tftpOptionsConfiguration,
      std::bind_front( &operationCompleted ),
      std::make_shared< Tftp::File::StreamFile >(
        Tftp::File::TftpFile::Operation::Transmit,
        filename,
        std::filesystem::file_size( filename ) ),
      remote,
      clientOptions,
      {} /* no additional options */ } ) };

  operation->start();
}

static void receiveFile(
  const boost::asio::ip::udp::endpoint &remote,
  const std::filesystem::path &filename,
  const Tftp::Packets::TftpOptions &clientOptions )
{
  std::cout
    << "WRQ: " << filename << " from: " << remote.address().to_string() << "\n";

  // open requested file
  std::fstream fileStream(
    filename.c_str(),
    std::fstream::out | std::fstream::trunc );

  // check that file was opened successfully
  if ( !fileStream.good() )
  {
    std::cerr << "Error opening file\n";

    server->errorOperation(
      remote,
      Tftp::Packets::ErrorCode::AccessViolation );

    return;
  }

  // initiate TFTP operation
  auto operation{ server->writeOperation(
    Tftp::Server::WriteOperationConfiguration{
      tftpConfiguration,
      tftpOptionsConfiguration,
      std::bind_front( &operationCompleted ),
      std::make_shared< Tftp::File::StreamFile >(
        Tftp::File::TftpFile::Operation::Receive,
        filename ),
      remote,
      clientOptions,
      {} /* no additional options */ } ) };

  operation->start();
}

static void operationCompleted(
  [[maybe_unused]] const Tftp::Server::OperationPtr &operation,
  Tftp::TransferStatus transferStatus )
{
  std::cout << "Transfer Completed: " << transferStatus << "\n";
  // Print Packet Statistic
  std::cout
    << "RX:\n" << Tftp::Packets::PacketStatistic::globalReceive() << "\n"
    << "TX:\n" << Tftp::Packets::PacketStatistic::globalTransmit() << "\n";
}
