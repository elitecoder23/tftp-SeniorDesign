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

/**
 * @dir
 * @brief TFTP Client CLI Application.
 **/

#include <tftp/Tftp.hpp>
#include <tftp/TftpConfiguration.hpp>
#include <tftp/TftpException.hpp>

#include <tftp/RequestTypeDescription.hpp>

#include <tftp/file/StreamFile.hpp>

#include <tftp/client/Client.hpp>
#include <tftp/client/TftpClient.hpp>
#include <tftp/client/Operation.hpp>

#include <helper/Logger.hpp>
#include <helper/BoostAsioProgramOptions.hpp>

#include <boost/program_options.hpp>
#include <boost/asio.hpp>
#include <boost/exception/all.hpp>

#include <cstdlib>
#include <memory>
#include <string>
#include <iostream>

/**
 * @brief Program Entry Point
 *
 * @param[in] argc
 *   Number of arguments.
 * @param[in] argv
 *   The arguments
 *
 * @return The success state of this operation.
 **/
int main( int argc, char *argv[] );

int main( int argc, char *argv[] )
{
  Tftp::RequestType requestType{ Tftp::RequestType::Invalid };
  std::string localFile{};
  std::string remoteFile{};
  boost::asio::ip::address address{};
  Tftp::TftpConfiguration configuration{};

  Helper::initLogging();

  boost::program_options::options_description optionsDescription{
    "TFTP Client Options" };

  optionsDescription.add_options()
    (
      "help",
      "print this help screen"
    )
    (
      "request-type",
      boost::program_options::value( &requestType)->required(),
      "the desired operation (\"Read\"|\"Write\")"
    )
    (
      "local-file",
      boost::program_options::value( &localFile),
      "filename of local file"
    )
    (
      "remote-file",
      boost::program_options::value( &remoteFile)->required(),
      "filename of remote file"
    )
    (
      "address",
      boost::program_options::value( &address)->required(),
      "remote address"
    );

  // Add common TFTP options
  optionsDescription.add( configuration.options() );

  try
  {
    std::cout << "TFTP client\n";

    boost::program_options::variables_map options;
    boost::program_options::store(
      boost::program_options::parse_command_line(
        argc,
        argv,
        optionsDescription),
      options );

    if ( options.count( "help" ) != 0)
    {
      std::cout << optionsDescription << "\n";
      return EXIT_FAILURE;
    }

    boost::program_options::notify( options );

    // Assemble TFTP configuration
    boost::asio::io_context ioContext;

    auto tftpClient{ Tftp::Client::TftpClient::instance(
      ioContext,
      configuration.tftpTimeout,
      configuration.tftpRetries ) };

    Tftp::Client::OperationPtr tftpOperation{};

    auto optionNegotiation = [](
      const Tftp::Options &serverOptions ) -> bool
    {
      return true;
    };

    switch ( requestType)
    {
      case Tftp::RequestType::Read:
        tftpOperation= tftpClient->readRequestOperation(
          optionNegotiation,
          std::make_shared< Tftp::File::StreamFile>(
            Tftp::File::TftpFile::Operation::Receive,
            localFile ),
          std::bind( &boost::asio::io_context::stop, std::ref( ioContext ) ),
          boost::asio::ip::udp::endpoint{ address, configuration.tftpServerPort },
          remoteFile,
          Tftp::TransferMode::OCTET,
          configuration.tftpOptions,
          {}, /* no additional options */
          configuration.dally );
        break;

      case Tftp::RequestType::Write:
        tftpOperation = tftpClient->writeRequestOperation(
          optionNegotiation,
          std::make_shared< Tftp::File::StreamFile>(
            Tftp::File::TftpFile::Operation::Transmit,
            localFile,
            std::filesystem::file_size( localFile)),
          std::bind( &boost::asio::io_context::stop, std::ref( ioContext ) ),
          boost::asio::ip::udp::endpoint{ address, configuration.tftpServerPort },
          remoteFile,
          Tftp::TransferMode::OCTET,
          configuration.tftpOptions,
          {} /* no additional options */ );
        break;

      default:
        std::cerr << "Internal invalid operation\n";
        return EXIT_FAILURE;
    }

    // Start client and its operations
    ioContext.run();
  }
  catch ( boost::program_options::error &e )
  {
    std::cout << e.what() << "\n" << optionsDescription << "\n";
    return EXIT_FAILURE;
  }
  catch ( Tftp::TftpException &e)
  {
    auto const * const info = boost::get_error_info< Helper::AdditionalInfo>( e );

    std::cerr
      << "TFTP transfer failed: "
      //      typeid( e).name() << " - " <<
      << ((nullptr==info) ? "Unknown" : *info)
      << "\n";
    return EXIT_FAILURE;
  }
  catch ( boost::exception &e)
  {
    std::cerr
      << "Error in TFTP client: " << boost::diagnostic_information( e) << "\n";
    return EXIT_FAILURE;
  }
  catch ( ...)
  {
    std::cerr << "Error in TFTP client: UNKNOWN EXCEPTION\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
