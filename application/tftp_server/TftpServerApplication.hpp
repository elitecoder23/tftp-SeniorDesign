/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class TftpServerApplication.
 **/

#ifndef TFTPSERVERAPPLICATION_HPP
#define TFTPSERVERAPPLICATION_HPP

#include <tftp/server/TftpServer.hpp>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>

//! The TFTP server application.
class TftpServerApplication
{
  public:
    /**
     * @brief Constructor, which instantiates the TFT server application.
     **/
    TftpServerApplication();

    /**
     * @brief Destructor of the TFTP server application.
     **/
    virtual ~TftpServerApplication() noexcept;

    int operator()( int argc, char *argv[]);

    bool stop();

  private:
    void shutdown();

    bool checkFilename( const boost::filesystem::path &filename) const;

    void receivedRequest(
      Tftp::RequestType requestType,
      const std::string &filename,
      Tftp::TransferMode mode,
      const Tftp::Options::OptionList &options,
      const Tftp::UdpAddressType &from);

    void transmitFile(
      const boost::filesystem::path &filename,
      const Tftp::Options::OptionList &options,
      const Tftp::UdpAddressType &from);

    void receiveFile(
      const boost::filesystem::path &filename,
      const Tftp::Options::OptionList &options,
      const Tftp::UdpAddressType &from);

    //! Options description
    boost::program_options::options_description optionsDescription;

    //! base directory of TFTP server
    boost::filesystem::path baseDir;
    //! The TFTP configuration
    Tftp::TftpConfiguration configuration;
    //! The TFTP server instance
    Tftp::Server::TftpServerPtr server;

    //! IO Service
    boost::asio::io_service ioService;
    //! ASIO signal handler
    boost::asio::signal_set signals;

};

#endif
