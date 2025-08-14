// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief TFTP Client CLI Application.
 **/

#include <tftp/clients/ReadOperation.hpp>
#include <tftp/clients/Client.hpp>
#include <tftp/clients/WriteOperation.hpp>

#include <tftp/files/StreamFile.hpp>

#include <tftp/packets/PacketStatistic.hpp>

#include <tftp/TftpConfiguration.hpp>
#include <tftp/TftpException.hpp>
#include <tftp/TftpOptionsConfiguration.hpp>
#include <tftp/RequestTypeDescription.hpp>
#include <tftp/Version.hpp>

#include <helper/BoostAsioProgramOptions.hpp>

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
static bool optionNegotiation( const Tftp::Packets::Options &serverOptions );

/**
 * @brief Operation Completed callback
 *
 * @param[in] ioContext
 *   IO Context
 * @param[in] transferStatus
 *   Transfer Status.
 **/
static void operationCompleted( boost::asio::io_context &ioContext, Tftp::TransferStatus transferStatus );

/**
 * @brief Initiates and executes the TFTP Client Read Operation.
 *
 * @param[in] tftpClient
 *   TFTP Client
 * @param[in] tftpConfiguration
 *   TFTP Configuration
 * @param[in] tftpOptionsConfiguration
 *   TFTP Options Configuration
 * @param[in] localFile
 *   Local Filename
 * @param[in] remoteFile
 *   Remote Filename
 * @param[in] address
 *   Remote IP-Address
 * @param[in] ioContext
 *   ASIO IO-Context
 **/
static Tftp::Clients::OperationPtr readOperation(
  const Tftp::Clients::ClientPtr &tftpClient,
  const Tftp::TftpConfiguration &tftpConfiguration,
  const Tftp::TftpOptionsConfiguration &tftpOptionsConfiguration,
  const std::filesystem::path &localFile,
  const std::string &remoteFile,
  const boost::asio::ip::address &address,
  boost::asio::io_context &ioContext );

/**
 * @brief Initiates and executes the TFTP Client Read Operation.
 *
* @param[in] tftpClient
 *   TFTP Client
 * @param[in] tftpConfiguration
 *   TFTP Configuration
 * @param[in] tftpOptionsConfiguration
 *   TFTP Options Configuration
 * @param[in] localFile
 *   Local Filename
 * @param[in] remoteFile
 *   Remote Filename
 * @param[in] address
 *   Remote IP-Address
 * @param[in] ioContext
 *   ASIO IO-Context
 **/
static Tftp::Clients::OperationPtr writeOperation(
  const Tftp::Clients::ClientPtr &tftpClient,
  const Tftp::TftpConfiguration &tftpConfiguration,
  const Tftp::TftpOptionsConfiguration &tftpOptionsConfiguration,
  const std::filesystem::path &localFile,
  const std::string &remoteFile,
  const boost::asio::ip::address &address,
  boost::asio::io_context &ioContext );

int main( const int argc, char * argv[] )
{
  try
  {
    std::cout << std::format( "TFTP Client - {}\n", Tftp::Version::VersionInformation );

    Tftp::RequestType requestType{};
    std::filesystem::path localFile;
    std::string remoteFile;
    boost::asio::ip::address address;
    Tftp::TftpConfiguration tftpConfiguration;
    Tftp::TftpOptionsConfiguration tftpOptionsConfiguration;

    boost::program_options::options_description optionsDescription{ "TFTP Client Options" };

    optionsDescription.add_options()
    (
      "help,h",
      "Print this help screen."
    )
    (
      "request-type,r",
      boost::program_options::value( &requestType )->required(),
      R"(The desired TFTP operation ("Read"|"Write").)"
    )
    (
      "local-file,l",
      boost::program_options::value( &localFile ),
      "Filename of local file."
    )
    (
      "remote-file,r",
      boost::program_options::value( &remoteFile )->required(),
      "Filename of remote file."
    )
    (
      "address,a",
      boost::program_options::value( &address )->required(),
      "Remote address of the TFTP server."
    );

    // Add TFTP options
    optionsDescription.add( tftpConfiguration.options() );
    optionsDescription.add( tftpOptionsConfiguration.options() );

    boost::program_options::variables_map variablesMap;
    boost::program_options::store(
      boost::program_options::parse_command_line( argc, argv, optionsDescription ),
      variablesMap );

    if ( 0U != variablesMap.count( "help" ) )
    {
      std::cout
        << "Performs a TFTP client transfer.\n\n"
        << optionsDescription << "\n";
      return EXIT_FAILURE;
    }

    boost::program_options::notify( variablesMap );

    // Assemble TFTP configuration
    boost::asio::io_context ioContext;

    auto tftpClient{ Tftp::Clients::Client::instance( ioContext ) };

    Tftp::Clients::OperationPtr tftpOperation{};

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

    ioContext.run();

    // Print Packet Statistic
    std::cout
      << "RX:\n" << Tftp::Packets::PacketStatistic::globalReceive() << "\n"
      << "TX:\n" << Tftp::Packets::PacketStatistic::globalTransmit() << "\n";

    return EXIT_SUCCESS;
  }
  catch ( const boost::program_options::error &e )
  {
    std::cerr << std::format(
      "Error parsing command line: {}\n"
      "Enter '{} --help' for command line description.\n",
      e.what(),
      argv[ 0 ] );
    return EXIT_FAILURE;
  }
  catch ( const boost::exception &e )
  {
    std::cerr << std::format( "Error: {}\n", boost::diagnostic_information( e ) );
    return EXIT_FAILURE;
  }
  catch ( const std::exception &e )
  {
    std::cerr << std::format( "Error: {}\n", boost::diagnostic_information( e ) );
    return EXIT_FAILURE;
  }
  catch ( ... )
  {
    std::cerr << "Unknown exception occurred\n";
    return EXIT_FAILURE;
  }
}

static bool optionNegotiation( const Tftp::Packets::Options &serverOptions )
{
  return serverOptions.empty();
}

static void operationCompleted(
  boost::asio::io_context &ioContext,
  [[maybe_unused]] Tftp::TransferStatus transferStatus )
{
  ioContext.stop();
}

static Tftp::Clients::OperationPtr readOperation(
  const Tftp::Clients::ClientPtr &tftpClient,
  const Tftp::TftpConfiguration &tftpConfiguration,
  const Tftp::TftpOptionsConfiguration &tftpOptionsConfiguration,
  const std::filesystem::path &localFile,
  const std::string &remoteFile,
  const boost::asio::ip::address &address,
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
    .dataHandler( std::make_shared< Tftp::Files::StreamFile >( Tftp::Files::File::Operation::Receive, localFile ) )
    .filename( remoteFile )
    .mode( Tftp::Packets::TransferMode::OCTET )
    .remote( boost::asio::ip::udp::endpoint{ address,tftpConfiguration.tftpServerPort } );

  return tftpOperation;
}

static  Tftp::Clients::OperationPtr writeOperation(
  const Tftp::Clients::ClientPtr &tftpClient,
  const Tftp::TftpConfiguration &tftpConfiguration,
  const Tftp::TftpOptionsConfiguration &tftpOptionsConfiguration,
  const std::filesystem::path &localFile,
  const std::string &remoteFile,
  const boost::asio::ip::address &address,
  boost::asio::io_context &ioContext )
{
  auto tftpOperation{ tftpClient->writeOperation() };

  tftpOperation
    ->tftpTimeout( tftpConfiguration.tftpTimeout )
    .tftpRetries( tftpConfiguration.tftpRetries )
    .optionsConfiguration( tftpOptionsConfiguration )
    .optionNegotiationHandler( std::bind_front( &optionNegotiation ) )
    .completionHandler( std::bind_front( &operationCompleted, std::ref( ioContext ) ) )
    .dataHandler(
      std::make_shared< Tftp::Files::StreamFile >(
        Tftp::Files::File::Operation::Transmit,
        localFile,
        std::filesystem::file_size( localFile ) ) )
    .filename( remoteFile )
    .mode( Tftp::Packets::TransferMode::OCTET )
    .remote( boost::asio::ip::udp::endpoint{ address,tftpConfiguration.tftpServerPort } );

  return tftpOperation;
}
