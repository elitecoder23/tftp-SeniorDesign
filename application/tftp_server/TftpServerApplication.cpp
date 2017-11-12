/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class TftpServerApplication.
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

TftpServerApplication::TftpServerApplication(
  boost::application::context &context) :
  context( context),
  optionsDescription( "TFTP server options")
{
  optionsDescription.add_options()
    (
      "help",
      "print this help screen"
    )

    (
      "server-root",
      boost::program_options::value< boost::filesystem::path>( &baseDir)->default_value( boost::filesystem::current_path()),
      "Directory path, where the server shall have its root"
    );

  // Add TFTP options
  optionsDescription.add( configuration.getOptions());
}

TftpServerApplication::~TftpServerApplication() noexcept
{
}

int TftpServerApplication::operator()()
{
  try
  {
    std::cout << "TFTP server\n";

    if ( !handleCommandLine())
    {
      return EXIT_FAILURE;
    }

    // make a absolute path
    baseDir = boost::filesystem::canonical( baseDir);

    std::cout <<
      "Starting TFTP server in " <<
      baseDir.string() <<
      " on port " <<
      configuration.tftpServerPort << "\n";

    // The TFTP server instance
    server = Tftp::Server::TftpServer::createInstance(
      std::bind(
        &TftpServerApplication::receivedRequest,
        this,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4,
        std::placeholders::_5),
      configuration,
      Tftp::Options::OptionList(),
      Tftp::UdpAddressType(
        boost::asio::ip::address_v4::any(),
        configuration.tftpServerPort));

    server->start();
    server->entry();
  }
  catch ( Tftp::TftpException &e)
  {
    std::string const * info = boost::get_error_info < AdditionalInfo > (e);

    std::cerr << "TFTP Server exited with failure: "
      << ((0 == info) ? "Unknown" : *info) << std::endl;

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

bool TftpServerApplication::handleCommandLine()
{
  try
  {
    std::shared_ptr < boost::application::args > args = context.find<
      boost::application::args>();

    boost::program_options::variables_map options;
    boost::program_options::store(
      boost::program_options::parse_command_line(
        args->argc(),
        args->argv(),
        optionsDescription),
      options);
    boost::program_options::notify( options);

    if ( options.count( "help") != 0)
    {
      std::cout << optionsDescription << std::endl;
      return false;
    }
  }
  catch ( boost::program_options::error &e)
  {
    std::cout << e.what() << std::endl << optionsDescription << std::endl;
    return false;
  }

  return true;
}

bool TftpServerApplication::checkFilename( const boost::filesystem::path &filename) const
{
  if ( filename.is_relative())
  {
    return false;
  }

  if ( boost::filesystem::is_directory( filename))
  {
    return false;
  }

  boost::filesystem::path::iterator fileIt( filename.begin());

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
  const Tftp::RequestType requestType,
  const string &filename,
  const Tftp::TransferMode mode,
  const Tftp::Options::OptionList &options,
  const Tftp::UdpAddressType &from)
{
  // Check transfer mode
  if ( mode != Tftp::TransferMode::OCTET)
  {
    std::cerr << "Wrong transfer mode";

    auto operation( server->createErrorOperation(
      from,
      Tftp::ErrorCode::IllegalTftpOperation,
      "wrong transfer mode"));

    operation->start();

    return;
  }

  if (!checkFilename( (baseDir / filename).lexically_normal()))
  {
    std::cerr << "Error filename check\n";

    auto operation( server->createErrorOperation(
      from,
      Tftp::ErrorCode::AccessViolation,
      "Illegal filename"));

    operation->start();

    return;
  }

  switch (requestType)
  {
    case Tftp::RequestType::Read:
      // we are on server side and transmit the data on RRQ
      transmitFile( baseDir / filename, options, from);
      break;

    case Tftp::RequestType::Write:
      // we are on server side and receive the data on WRQ
      receiveFile( baseDir /  filename, options, from);
      break;

    default:
      return;
  }

}

void TftpServerApplication::transmitFile(
  const boost::filesystem::path &filename,
  const Tftp::Options::OptionList &options,
  const Tftp::UdpAddressType &from)
{
  std::cout << "RRQ: " << filename << " from: "
    << from.address().to_string() << "\n";

  // open requested file
  std::fstream fileStream( filename.c_str(), std::fstream::in);

  // check that file was opened successfully
  if ( !fileStream.good())
  {
    std::cerr << "Error opening file\n";

    auto operation( server->createErrorOperation(
      from,
      Tftp::ErrorCode::FileNotFound,
      "file not found"));

    operation->start();

    return;
  }

  // initiate TFTP operation
  auto operation(
    server->createReadRequestOperation(
      std::make_shared< Tftp::File::StreamFile< std::fstream>>(
        std::move( fileStream),
        boost::filesystem::file_size( filename)),
      {},
      from,
      options));

  // executes the TFTP operation
  operation->start();
}

void TftpServerApplication::receiveFile(
  const boost::filesystem::path &filename,
  const Tftp::Options::OptionList &options,
  const Tftp::UdpAddressType &from)
{
  std::cout << "WRQ: " << filename << " from: "
    << from.address().to_string() << "\n";

  // open requested file
  std::fstream fileStream(
    filename.c_str(),
    std::fstream::out | std::fstream::trunc);

  // check that file was opened successfully
  if ( !fileStream.good())
  {
    std::cerr << "Error opening file";

    auto operation( server->createErrorOperation(
      from,
      Tftp::ErrorCode::AccessViolation));

    operation->start();

    return;
  }

  // initiate TFTP operation
  auto operation(
    server->createWriteRequestOperation(
      std::make_shared< Tftp::File::StreamFile< std::fstream>>(
        std::move( fileStream),
        boost::filesystem::file_size( filename)),
      {},
      from,
      options));

  // executes the TFTP operation
  operation->start();
}

void TftpServerApplication::shutdown()
{
  server->stop();
}
