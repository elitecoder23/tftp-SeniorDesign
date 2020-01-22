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

#include <tftp/file/StreamFile.hpp>

#include <tftp/TftpException.hpp>
#include <tftp/TftpConfiguration.hpp>
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
#include <memory>
#include <thread>

/**
 * @brief TFTP Server Program Entry Point
 *
 * @param[in] argc
 *   Number of arguments.
 * @param[in] argv
 *   The arguments
 *
 * @return The success state of this operation.
 **/
int main( int argc, char * argv[]);

//! Shutdowns the TFTP Server.
static void stop();

/**
 * @brief Performs Validity Check of supplied Filename.
 *
 * @param[in] filename
 *   Filename to check.
 *
 * @return If filename is valid.
 **/
static bool checkFilename( const std::filesystem::path &filename);

/**
 * @brief Handler for Recevifed TFTP Requests.
 *
 * @param[in] remote
 *   Remote address.
 * @param[in] requestType
 *   TFTP Request Type (RRQ/ WRQ)
 * @param[in] filename
 *   Requested filename.
 * @param[in] mode
 *   Transfer Mode
 * @param[in] clientOptions
 *   Received Options.
 **/
static void receivedRequest(
  const boost::asio::ip::udp::endpoint &remote,
  Tftp::RequestType requestType,
  std::string_view filename,
  Tftp::TransferMode mode,
  const Tftp::Options::Options &clientOptions);

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
  const Tftp::Options::Options &clientOptions);

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
  const Tftp::Options::Options &clientOptions);

//! TFTP Server Base Directory
static std::filesystem::path baseDir{};

//! TFTP Server Configuration
static Tftp::TftpConfiguration configuration{};

//! TFTP Server Instance
static Tftp::Server::TftpServerPtr server{};

int main( int argc, char * argv[])
{
  Helper::initLogging();

  std::cout << "TFTP Server - " << Tftp::Version::version() << "\n";

  boost::program_options::options_description optionsDescription{
    "TFTP server options"};

  optionsDescription.add_options()
    (
      "help",
      "print this help screen"
    )

    (
      "server-root",
      boost::program_options::value( &baseDir)->default_value(
        std::filesystem::current_path()),
      "Directory path, where the server shall have its root"
    );

  // Add TFTP options
  optionsDescription.add( configuration.options());

  boost::asio::io_context ioContext;
  boost::asio::signal_set signals{ ioContext, SIGINT, SIGTERM};

  try
  {
    boost::program_options::variables_map options;
    boost::program_options::store(
      boost::program_options::parse_command_line(
        argc,
        argv,
        optionsDescription),
      options);

    if ( options.count( "help") != 0)
    {
      std::cout << optionsDescription << std::endl;
      return EXIT_FAILURE;
    }

    boost::program_options::notify( options);

    // make a absolute path
    baseDir = std::filesystem::canonical( baseDir);

    std::cout
      << "Starting TFTP server in " << baseDir.string()
      << " on port " << configuration.tftpServerPort << "\n";

    // The TFTP server instance
    server = Tftp::Server::TftpServer::instance(
      std::bind(
        &receivedRequest,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4,
        std::placeholders::_5),
      configuration.tftpTimeout,
      configuration.tftpRetries,
      boost::asio::ip::udp::endpoint{
        boost::asio::ip::address_v4::any(),
        configuration.tftpServerPort});

    server->start();

    std::thread serverThread{ std::bind( &Tftp::Server::TftpServer::entry, server)};

    // connect to SIGINT and SIGTERM
    signals.async_wait( boost::bind( &stop));

    ioContext.run();

    serverThread.join();
  }
  catch ( boost::program_options::error &e)
  {
    std::cout << e.what() << std::endl << optionsDescription << std::endl;
    return EXIT_FAILURE;
  }
  catch ( Tftp::TftpException &e)
  {
    std::string const * info = boost::get_error_info < Helper::AdditionalInfo > (e);

    std::cerr
      << "TFTP Server exited with failure: "
      << ((nullptr == info) ? "Unknown" : *info) << "\n";

    return EXIT_FAILURE;
  }
  catch ( boost::exception &e)
  {
    std::cerr
      << "Error in TFTP server: " << boost::diagnostic_information( e) << "\n";
    return EXIT_FAILURE;
  }
  catch ( ...)
  {
    std::cerr << "Error in TFTP server: UNKNOWN EXCEPTION\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

static void stop()
{
  std::cout << "Termination request" << std::endl;

  server->stop();
}

bool checkFilename( const std::filesystem::path &filename)
{
  if ( filename.is_relative())
  {
    return false;
  }

  if ( std::filesystem::is_directory( filename))
  {
    return false;
  }

  auto fileIt{ filename.begin()};

  for (const auto& pathItem : baseDir)
  {
    if ( fileIt == filename.end())
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
  const Tftp::TransferMode mode,
  const Tftp::Options::Options &clientOptions)
{
  // Check transfer mode
  if ( mode != Tftp::TransferMode::OCTET)
  {
    std::cerr << "Wrong transfer mode\n";

    server->errorOperation(
      remote,
      Tftp::ErrorCode::IllegalTftpOperation,
      "wrong transfer mode");

    return;
  }

  if ( !checkFilename( (baseDir / filename).lexically_normal()))
  {
    std::cerr << "Error filename check\n";

    server->errorOperation(
      remote,
      Tftp::ErrorCode::AccessViolation,
      "Illegal filename");

    return;
  }

  switch (requestType)
  {
    case Tftp::RequestType::Read:
      // we are on server side and transmit the data on RRQ
      transmitFile( remote, baseDir / filename, clientOptions);
      break;

    case Tftp::RequestType::Write:
      // we are on server side and receive the data on WRQ
      receiveFile( remote, baseDir /  filename, clientOptions);
      break;

    default:
      return;
  }

}

void transmitFile(
  const boost::asio::ip::udp::endpoint &remote,
  const std::filesystem::path &filename,
  const Tftp::Options::Options &clientOptions)
{
  std::cout
    << "RRQ: " << filename << " from: " << remote.address().to_string() << "\n";

  // open requested file
  std::fstream fileStream{ filename.c_str(), std::fstream::in};

  // check that file was opened successfully
  if ( !fileStream.good())
  {
    std::cerr << "Error opening file\n";

    server->errorOperation(
      remote,
      Tftp::ErrorCode::FileNotFound,
      "file not found");

    return;
  }

  // initiate TFTP operation
  auto operation{
    server->readRequestOperation(
      std::make_shared< Tftp::File::StreamFile< std::fstream>>(
        std::move( fileStream),
        std::filesystem::file_size( filename)),
      []( const Tftp::TransferStatus transferStatus)
      {
        std::cout << "Transfer Completed: " << transferStatus << "\n";
      },
      remote,
      configuration.serverOptions().negotiateServer( clientOptions))};
}

void receiveFile(
  const boost::asio::ip::udp::endpoint &remote,
  const std::filesystem::path &filename,
  const Tftp::Options::Options &clientOptions)
{
  std::cout
    << "WRQ: " << filename << " from: " << remote.address().to_string() << "\n";

  // open requested file
  std::fstream fileStream(
    filename.c_str(),
    std::fstream::out | std::fstream::trunc);

  // check that file was opened successfully
  if ( !fileStream.good())
  {
    std::cerr << "Error opening file\n";

    server->errorOperation(
      remote,
      Tftp::ErrorCode::AccessViolation);

    return;
  }

  // initiate TFTP operation
  auto operation{
    server->writeRequestOperation(
      std::make_shared< Tftp::File::StreamFile< std::fstream>>(
        std::move( fileStream),
        std::filesystem::file_size( filename)),
      []( const Tftp::TransferStatus transferStatus)
      {
        std::cout << "Transfer Completed: " << transferStatus << "\n";
      },
      remote,
      configuration.serverOptions().negotiateServer( clientOptions))};
}
