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

/**
 * @dir
 * @brief TFTP Server CLI Application.
 **/

#include <tftp/server/TftpServer.hpp>
#include <tftp/server/Operation.hpp>
#include <tftp/server/ServerConfiguration.hpp>
#include <tftp/server/ReadOperationConfiguration.hpp>
#include <tftp/server/WriteOperationConfiguration.hpp>

#include <tftp/file/StreamFile.hpp>

#include <tftp/TftpException.hpp>
#include <tftp/TftpConfiguration.hpp>
#include <tftp/TftpOptionsConfiguration.hpp>
#include <tftp/TransferStatusDescription.hpp>
#include <tftp/Version.hpp>

#include <helper/Logger.hpp>

#include <boost/exception/all.hpp>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>

#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <utility>

/**
 * @brief TFTP Server Program Entry Point
 *
 * @param[in] argc
 *   Number of arguments.
 * @param[in] argv
 *   Arguments
 *
 * @return Success state of this operation.
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
  boost::asio::ip::udp::endpoint remote,
  Tftp::RequestType requestType,
  std::string filename,
  Tftp::Packets::TransferMode mode,
  Tftp::Packets::Options clientOptions,
  Tftp::Packets::Options additionalClientOptions );

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
  boost::asio::ip::udp::endpoint remote,
  std::filesystem::path filename,
  Tftp::Packets::Options clientOptions );

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
  boost::asio::ip::udp::endpoint remote,
  std::filesystem::path filename,
  Tftp::Packets::Options clientOptions );

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
  Helper::initLogging();

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

  try
  {
    boost::asio::io_context ioContext;
    boost::asio::signal_set signals{ ioContext, SIGINT, SIGTERM };

    boost::program_options::variables_map options;
    boost::program_options::store(
      boost::program_options::parse_command_line(
        argc,
        argv,
        optionsDescription),
      options );

    if ( options.count( "help" ) != 0 )
    {
      std::cout << optionsDescription << "\n";
      return EXIT_FAILURE;
    }

    boost::program_options::notify( options );

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
  }
  catch ( const boost::program_options::error &e )
  {
    std::cout << e.what() << std::endl << optionsDescription << "\n";
    return EXIT_FAILURE;
  }
  catch ( const boost::exception &e )
  {
    std::cerr
      << "Error in TFTP server: " << boost::diagnostic_information( e ) << "\n";
    return EXIT_FAILURE;
  }
  catch ( ... )
  {
    std::cerr << "Error in TFTP server: UNKNOWN EXCEPTION\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
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
  boost::asio::ip::udp::endpoint remote,
  const Tftp::RequestType requestType,
  std::string filename,
  const Tftp::Packets::TransferMode mode,
  Tftp::Packets::Options clientOptions,
  Tftp::Packets::Options additionalClientOptions )
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
      transmitFile( remote, baseDir / filename, std::move( clientOptions ) );
      break;

    case Tftp::RequestType::Write:
      // we are on server side and receive the data on WRQ
      receiveFile( remote, baseDir /  filename, std::move( clientOptions ) );
      break;

    default:
      return;
  }
}

static void transmitFile(
  boost::asio::ip::udp::endpoint remote,
  std::filesystem::path filename,
  Tftp::Packets::Options clientOptions )
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
      std::make_shared< Tftp::File::StreamFile >(
        Tftp::File::TftpFile::Operation::Transmit,
        std::move( filename ),
        std::filesystem::file_size( filename ) ),
      []( const Tftp::TransferStatus transferStatus ) {
        std::cout << "Transfer Completed: " << transferStatus << "\n";
      },
      std::move( remote ),
      std::move( clientOptions ),
      {} /* no additional options */
    } ) };

  operation->start();
}

static void receiveFile(
  boost::asio::ip::udp::endpoint remote,
  std::filesystem::path filename,
  Tftp::Packets::Options clientOptions )
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
      std::make_shared< Tftp::File::StreamFile >(
        Tftp::File::TftpFile::Operation::Receive,
        std::move( filename ) ),
      []( const Tftp::TransferStatus transferStatus ) {
        std::cout << "Transfer Completed: " << transferStatus << "\n";
      },
      std::move( remote ),
      std::move( clientOptions ),
      {} /* no additional options */
    } ) };

  operation->start();
}
