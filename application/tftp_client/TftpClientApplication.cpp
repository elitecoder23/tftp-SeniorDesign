/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * $Date$
 * $Revision$
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class TftpClientApplication.
 **/

#include "TftpClientApplication.hpp"

#include <tftp/Tftp.hpp>
#include <tftp/TftpException.hpp>
#include <tftp/TftpConfiguration.hpp>
#include <tftp/options/OptionList.hpp>
#include <tftp/client/TftpClient.hpp>
#include <tftp/file/StreamFile.hpp>

#include <helper/Logger.hpp>
#include <helper/BoostAsioProgramOptions.hpp>

#include <boost/program_options.hpp>
#include <boost/exception/all.hpp>
#include <boost/application.hpp>

#include <fstream>
#include <cstdlib>
#include <memory>

using Tftp::TransferMode;
using Tftp::Options::OptionList;
using Tftp::File::StreamFile;
using Tftp::Client::TftpClient;
using Tftp::Client::TftpClientPtr;
using Tftp::Client::TftpClientOperation;

TftpClientApplication::TftpClientApplication(
  boost::application::context &context) :
  context( context),
  optionsDescription( "TFTP Client Options"),
  operation( Operation::INVALID)
{
  optionsDescription.add_options()
    ("help",
      "print this help screen")

    ("operation",
      boost::program_options::value< std::string>( &operationStr)->required(),
      "the desired operation (\"READ\"|\"WRITE\")"
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
    )
    ("port",
      boost::program_options::value< uint16_t>( &port)->default_value(
        Tftp::DEFAULT_TFTP_PORT),
      "UDP port, where the server is listen"
    )
    (
      "blocksize-option",
      boost::program_options::value< uint16_t>( &configuration.blockSizeOptionValue)->notifier(
        [this]( const uint16_t &){
          this->configuration.handleBlockSizeOption = true;
        }),
      "blocksize of transfers to use"
    )
    (
      "timeout-option",
      boost::program_options::value< uint16_t>( &configuration.timoutOptionValue)->notifier(
        [this]( const uint16_t &){
          this->configuration.handleTimeoutOption = true;
        }),
      "If set handles the timeout option negotiation"
    )
    (
      "handle-transfer-size-option",
      boost::program_options::bool_switch( &configuration.handleTransferSizeOption),
      "If set handles the transfer size option negotiation"
    );
}

int TftpClientApplication::operator()( void)
{
  try
  {
    std::cout << "TFTP client" << std::endl;

    if ( !handleCommandLine())
    {
      return EXIT_FAILURE;
    }

    // Assemble TFTP configuration

    TftpClientPtr tftpClient = TftpClient::createInstance( configuration);
    TftpClientOperation op;
    std::fstream fileStream;
    StreamFile file( fileStream);
    boost::asio::ip::udp::endpoint serverAddress( address, port);

    switch ( operation)
    {
      case Operation::READ:
        fileStream.open( localFile, std::fstream::out | std::fstream::trunc);

        op = tftpClient->createReadRequestOperation(
          file,
          serverAddress,
          remoteFile,
          Tftp::TransferMode::OCTET);
        break;

      case Operation::WRITE:
        fileStream.open( localFile, std::fstream::in);

        op = tftpClient->createWriteRequestOperation(
          file,
          serverAddress,
          remoteFile,
          Tftp::TransferMode::OCTET);
        break;

      default:
        std::cerr << "Internal invalid operation" << std::endl;
        return EXIT_FAILURE;
    }

    // execute operation
    op();
  }
  catch ( Tftp::TftpException &e)
  {
    std::string const * info = boost::get_error_info< AdditionalInfo>( e);

    std::cerr <<
    	"TFTP transfer failed: " <<
//    	typeid( e).name() << " - " <<
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

bool TftpClientApplication::handleCommandLine( void)
{
  try
  {
    std::shared_ptr< boost::application::args> args =
    	context.find< boost::application::args>();

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

    if ( 0 == operationStr.compare( "READ"))
    {
      operation = Operation::READ;
    }
    else if ( 0 == operationStr.compare( "WRITE"))
    {
      operation = Operation::WRITE;
    }
    else
    {
      std::cout << optionsDescription << std::endl;
      return false;
    }

    // Activate blocksize option, if parameter is set
    configuration.handleBlockSizeOption = (0 != options.count( "blocksize-option"));
    // Activate timeout option, if parameter is set
    configuration.handleTimeoutOption = (0 != options.count( "timeout-option"));
  }
  catch ( boost::program_options::error &e)
  {
    std::cout << e.what() << std::endl << optionsDescription << std::endl;
    return false;
  }

  return true;
}
