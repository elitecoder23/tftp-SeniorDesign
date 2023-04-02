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

#include <tftp/Tftp.hpp>
#include <tftp/TftpConfiguration.hpp>
#include <tftp/TftpOptionsConfiguration.hpp>
#include <tftp/TftpException.hpp>

#include <tftp/RequestTypeDescription.hpp>

#include <tftp/file/StreamFile.hpp>

#include <tftp/client/Client.hpp>
#include <tftp/client/TftpClient.hpp>
#include <tftp/client/Operation.hpp>
#include <tftp/client/ReadOperationConfiguration.hpp>
#include <tftp/client/WriteOperationConfiguration.hpp>

#include <tftp/packets/PacketStatistic.hpp>

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
 *   Number of Arguments.
 * @param[in] argv
 *   Arguments
 *
 * @return Success state of this operation.
 **/
int main( int argc, char * argv[] );

/**
 * @brief Option Negotiation callback.
 *
 * Checks Server options.
 * As we don't send any additional options, the received options must be empty.
 *
 * @param[in,out] operation
 *   TFTP Operation
 * @param[in] serverOptions
 *   Received Server Options.
 *
 * @return if @p serverOptions are empty.
 **/
static bool optionNegotiation(
  const Tftp::Client::OperationPtr &operation,
  const Tftp::Packets::Options &serverOptions );

/**
 * @brief Operation Completed callback
 *
 * @param[in] ioContext
 *   IO Context
 * @param[in] operation
 *   TFTP Operation
 * @param[in] transferStatus
 *   Transfer Status
 **/
static void operationCompleted(
  boost::asio::io_context &ioContext,
  const Tftp::Client::OperationPtr &operation,
  Tftp::TransferStatus transferStatus );

int main( int argc, char * argv[] )
{
  Tftp::RequestType requestType{};
  std::filesystem::path localFile{};
  std::string remoteFile{};
  boost::asio::ip::address address{};
  Tftp::TftpConfiguration tftpConfiguration{};
  Tftp::TftpOptionsConfiguration tftpOptionsConfiguration{};

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
    boost::program_options::value( &remoteFile)->required(),
    "filename of remote file"
  )
  (
    "address",
    boost::program_options::value( &address)->required(),
    "remote address"
  );

  // Add common TFTP options
  optionsDescription.add( tftpConfiguration.options() );
  optionsDescription.add( tftpOptionsConfiguration.options() );

  try
  {
    std::cout << "TFTP Client\n";

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

    // Assemble TFTP configuration
    boost::asio::io_context ioContext;

    auto tftpClient{ Tftp::Client::TftpClient::instance( ioContext ) };

    Tftp::Client::OperationPtr tftpOperation{};

    switch ( requestType )
    {
      case Tftp::RequestType::Read:
        tftpOperation = tftpClient->readOperation(
          Tftp::Client::ReadOperationConfiguration{
            tftpConfiguration,
            tftpOptionsConfiguration,
            std::bind_front( &optionNegotiation ),
            std::bind_front( &operationCompleted, std::ref( ioContext ) ),
            std::make_shared< Tftp::File::StreamFile >(
              Tftp::File::TftpFile::Operation::Receive,
              localFile ),
            remoteFile,
            Tftp::Packets::TransferMode::OCTET,
            {}, /* no additional options */
            boost::asio::ip::udp::endpoint{
              address,
              tftpConfiguration.tftpServerPort } } );
        break;

      case Tftp::RequestType::Write:
        tftpOperation = tftpClient->writeOperation(
          Tftp::Client::WriteOperationConfiguration{
            tftpConfiguration,
            tftpOptionsConfiguration,
            std::bind_front( &optionNegotiation ),
            std::bind_front( &operationCompleted, std::ref( ioContext ) ),
            std::make_shared< Tftp::File::StreamFile >(
              Tftp::File::TftpFile::Operation::Transmit,
              localFile,
              std::filesystem::file_size( localFile ) ),
            remoteFile,
            Tftp::Packets::TransferMode::OCTET,
            {}, /* no additional options */
            boost::asio::ip::udp::endpoint{
              address,
              tftpConfiguration.tftpServerPort } } );
        break;

      default:
        std::cerr << "Internal invalid operation\n";
        return EXIT_FAILURE;
    }

    // start request
    tftpOperation->request();

    // Start client and its operations
    ioContext.run();

    // Print Packet Statistic
    std::cout
      << "RX:\n" << Tftp::Packets::PacketStatistic::globalReceive() << "\n"
      << "TX:\n" << Tftp::Packets::PacketStatistic::globalTransmit() << "\n";
  }
  catch ( const boost::program_options::error &e )
  {
    std::cout << e.what() << "\n" << optionsDescription << "\n";
    return EXIT_FAILURE;
  }
  catch ( const boost::exception &e )
  {
    std::cerr
      << "Error in TFTP client: " << boost::diagnostic_information( e ) << "\n";
    return EXIT_FAILURE;
  }
  catch ( ... )
  {
    std::cerr << "Error in TFTP client: UNKNOWN EXCEPTION\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

static bool optionNegotiation(
  [[maybe_unused]] const Tftp::Client::OperationPtr &operation,
  const Tftp::Packets::Options &serverOptions )
{
  return serverOptions.empty();
}

static void operationCompleted(
  boost::asio::io_context &ioContext,
  [[maybe_unused]] const Tftp::Client::OperationPtr &operation,
  [[maybe_unused]] Tftp::TransferStatus transferStatus )
{
  ioContext.stop();
}
