/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class TftpServerApplication.
 **/

#include "TftpServerApplication.hpp"

#include <tftp/file/StreamFile.hpp>
#include <tftp/TftpException.hpp>
#include <tftp/TftpConfiguration.hpp>
#include <tftp/server/Operation.hpp>

#include <helper/Logger.hpp>

#include <boost/exception/all.hpp>

#include <fstream>
#include <cstdlib>
#include <iostream>
#include <memory>

using Tftp::TftpException;

TftpServerApplication::TftpServerApplication() :
  optionsDescription( "TFTP server options"),
  signals( ioContext, SIGINT, SIGTERM)
{
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
}

TftpServerApplication::~TftpServerApplication() noexcept
{
}

int TftpServerApplication::operator()( int argc, char *argv[])
{
  try
  {
    std::cout << "TFTP server\n";

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

    std::cout <<
      "Starting TFTP server in " <<
      baseDir.string() <<
      " on port " <<
      configuration.tftpServerPort << "\n";

    // The TFTP server instance
    server = Tftp::Server::TftpServer::instance(
      std::bind(
        &TftpServerApplication::receivedRequest,
        this,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4,
        std::placeholders::_5),
      configuration,
      Tftp::Options::OptionList{},
      boost::asio::ip::udp::endpoint{
        boost::asio::ip::address_v4::any(),
        configuration.tftpServerPort});

    server->start();
    server->entry();

    // connect to SIGINT and SIGTERM
    signals.async_wait(
     boost::bind(
       &TftpServerApplication::stop,
       this));

    ioContext.run();
  }
  catch ( boost::program_options::error &e)
  {
    std::cout << e.what() << std::endl << optionsDescription << std::endl;
    return EXIT_FAILURE;
  }
  catch ( Tftp::TftpException &e)
  {
    std::string const * info = boost::get_error_info < AdditionalInfo > (e);

    std::cerr << "TFTP Server exited with failure: "
      << ((nullptr == info) ? "Unknown" : *info) << std::endl;

    return EXIT_FAILURE;
  }
  catch ( boost::exception &e)
  {
    std::cerr << "Error in TFTP server: " << boost::diagnostic_information( e)
      << std::endl;
    return EXIT_FAILURE;
  }
  catch ( ...)
  {
    std::cerr << "Error in TFTP server: UNKNOWN EXCEPTION" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}

bool TftpServerApplication::stop()
{
  std::cout << "Termination request" << std::endl;

  server->stop();

  return true;
}

bool TftpServerApplication::checkFilename(
  const std::filesystem::path &filename) const
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

void TftpServerApplication::receivedRequest(
  const boost::asio::ip::udp::endpoint &remote,
  const Tftp::RequestType requestType,
  std::string_view filename,
  const Tftp::TransferMode mode,
  const Tftp::Options::OptionList &options)
{
  // Check transfer mode
  if ( mode != Tftp::TransferMode::OCTET)
  {
    std::cerr << "Wrong transfer mode";

    using namespace std::string_view_literals;
    auto operation( server->errorOperation(
      {},
      remote,
      {boost::asio::ip::address_v4::any(), 0},
      Tftp::ErrorCode::IllegalTftpOperation,
      "wrong transfer mode"sv));

    operation->start();

    return;
  }

  if (!checkFilename( (baseDir / filename).lexically_normal()))
  {
    std::cerr << "Error filename check\n";

    using namespace std::string_view_literals;
    auto operation( server->errorOperation(
      {},
      remote,
      {boost::asio::ip::address_v4::any(), 0},
      Tftp::ErrorCode::AccessViolation,
      "Illegal filename"sv));

    operation->start();

    return;
  }

  switch (requestType)
  {
    case Tftp::RequestType::Read:
      // we are on server side and transmit the data on RRQ
      transmitFile( remote, baseDir / filename, options);
      break;

    case Tftp::RequestType::Write:
      // we are on server side and receive the data on WRQ
      receiveFile( remote, baseDir /  filename, options);
      break;

    default:
      return;
  }

}

void TftpServerApplication::transmitFile(
  const boost::asio::ip::udp::endpoint &remote,
  const std::filesystem::path &filename,
  const Tftp::Options::OptionList &options)
{
  std::cout << "RRQ: " << filename << " from: "
    << remote.address().to_string() << "\n";

  // open requested file
  std::fstream fileStream( filename.c_str(), std::fstream::in);

  // check that file was opened successfully
  if ( !fileStream.good())
  {
    std::cerr << "Error opening file\n";

    using namespace std::string_view_literals;
    auto operation( server->errorOperation(
      {},
      remote,
      {boost::asio::ip::address_v4::any(), 0},
      Tftp::ErrorCode::FileNotFound,
      "file not found"sv));

    operation->start();

    return;
  }

  // initiate TFTP operation
  auto operation(
    server->readRequestOperation(
      std::make_shared< Tftp::File::StreamFile< std::fstream>>(
        std::move( fileStream),
        std::filesystem::file_size( filename)),
      {},
      remote,
      options,
      {boost::asio::ip::address_v4::any(), 0}));

  // executes the TFTP operation
  operation->start();
}

void TftpServerApplication::receiveFile(
  const boost::asio::ip::udp::endpoint &remote,
  const std::filesystem::path &filename,
  const Tftp::Options::OptionList &options)
{
  std::cout << "WRQ: " << filename << " from: "
    << remote.address().to_string() << "\n";

  // open requested file
  std::fstream fileStream(
    filename.c_str(),
    std::fstream::out | std::fstream::trunc);

  // check that file was opened successfully
  if ( !fileStream.good())
  {
    std::cerr << "Error opening file";

    auto operation( server->errorOperation(
      {},
      remote,
      {boost::asio::ip::address_v4::any(), 0},
      Tftp::ErrorCode::AccessViolation));

    operation->start();

    return;
  }

  // initiate TFTP operation
  auto operation(
    server->writeRequestOperation(
      std::make_shared< Tftp::File::StreamFile< std::fstream>>(
        std::move( fileStream),
        std::filesystem::file_size( filename)),
      {},
      remote,
      options,
      {boost::asio::ip::address_v4::any(), 0}));

  // executes the TFTP operation
  operation->start();
}

void TftpServerApplication::shutdown()
{
  server->stop();
}
