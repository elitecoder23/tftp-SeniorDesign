/*
 * $Date$
 * $Revision$
 */
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class TftpClientApplication.
 **/

#include "TftpClientApplication.hpp"

#include <tftp/Tftp.hpp>
#include <tftp/TftpException.hpp>
#include <tftp/TftpConfiguration.hpp>
#include <tftp/RequestTypeDescription.hpp>

#include <tftp/options/OptionList.hpp>

#include <tftp/client/TftpClient.hpp>
#include <tftp/client/Operation.hpp>

#include <tftp/file/StreamFile.hpp>

#include <helper/Logger.hpp>
#include <helper/BoostAsioProgramOptions.hpp>

#include <boost/program_options.hpp>
#include <boost/exception/all.hpp>
#include <boost/application.hpp>

#include <fstream>
#include <cstdlib>
#include <memory>

TftpClientApplication::TftpClientApplication(
  boost::application::context &context) :
  context( context),
  optionsDescription( "TFTP Client Options"),
  requestType( Tftp::RequestType::Invalid)
{
  optionsDescription.add_options()
    ("help",
      "print this help screen")

    ("request-type",
      boost::program_options::value( &requestType)->required(),
      "the desired operation (\"Read\"|\"Write\")"
    )
    ("local-file",
      boost::program_options::value< std::string>( &localFile),
      "filename of local file"
    )
    ("remote-file",
      boost::program_options::value< std::string>( &remoteFile)->required(),
      "filename of remote file"
    )
    ("address",
      boost::program_options::value< boost::asio::ip::address>( &address)->required(),
      "remote address"
    );

  // Add common TFTP options
  optionsDescription.add( configuration.getOptions());
}

int TftpClientApplication::operator()()
{
  try
  {
    std::cout << "TFTP client\n";

    if ( !handleCommandLine())
    {
      return EXIT_FAILURE;
    }

    // Assemble TFTP configuration

    auto tftpClient = Tftp::Client::TftpClient::createInstance(
      configuration);

    switch ( requestType)
    {
      case Tftp::RequestType::Read:
        read( tftpClient);
        break;

      case Tftp::RequestType::Write:
        write( tftpClient);
        break;

      default:
        std::cerr << "Internal invalid operation" << std::endl;
        return EXIT_FAILURE;
    }
  }
  catch ( Tftp::TftpException &e)
  {
    std::string const * info = boost::get_error_info< AdditionalInfo>( e);

    std::cerr <<
      "TFTP transfer failed: " <<
//      typeid( e).name() << " - " <<
      ((nullptr==info) ? "Unknown" : *info) <<
      std::endl;

    return EXIT_FAILURE;
  }
  catch ( boost::exception &e)
  {
    std::cerr << "Error in TFTP client: " << boost::diagnostic_information( e)
      << std::endl;
    return EXIT_FAILURE;
  }
  catch ( ...)
  {
    std::cerr << "Error in TFTP client: UNKNOWN EXCEPTION" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void TftpClientApplication::read( TftpClientPtr client)
{
  Tftp::File::StreamFile< std::fstream> file(
    std::fstream( localFile, std::fstream::out | std::fstream::trunc));

  auto op( client->createReadRequestOperation(
    file,
    Tftp::UdpAddressType( address, configuration.tftpServerPort),
    remoteFile,
    Tftp::TransferMode::OCTET,
    std::bind( &Tftp::Client::TftpClient::stop, client)));

  // execute operation
  (*op)();

  (*client)();
}

void TftpClientApplication::write( TftpClientPtr client)
{
  Tftp::File::StreamFile< std::fstream> file(
    std::fstream( localFile, std::fstream::in));

  auto op( client->createWriteRequestOperation(
    file,
    Tftp::UdpAddressType( address, configuration.tftpServerPort),
    remoteFile,
    Tftp::TransferMode::OCTET,
    std::bind( &Tftp::Client::TftpClient::stop, client)));

  // execute operation
  (*op)();

  (*client)();
}

bool TftpClientApplication::handleCommandLine()
{
  try
  {
    std::shared_ptr< boost::application::args> args(
      context.find< boost::application::args>());

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
