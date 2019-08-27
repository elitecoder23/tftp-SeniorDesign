/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class TftpServerApplication.
 **/

#ifndef TFTPSERVERAPPLICATION_HPP
#define TFTPSERVERAPPLICATION_HPP

#include <tftp/server/TftpServer.hpp>

#include <boost/program_options.hpp>
#include <boost/asio.hpp>

#include <filesystem>

//! TFTP Server Application.
class TftpServerApplication
{
  public:
    /**
     * @brief Instantiates the TFTP Server Application.
     **/
    TftpServerApplication();

    //! Destructor
    virtual ~TftpServerApplication() noexcept;

    /**
     * @brief Entry Point of TFTP Server.
     *
     * @param[in] argc
     *   argument count
     * @param[in] argv
     *   argument values.
     *
     * @return Return code.
     **/
    int operator()( int argc, char *argv[]);

    /**
     * @brief Stops the TFTP server.
     *
     * @return
     **/
    bool stop();

  private:
    //! Shutdowns the TFTP Server.
    void shutdown();

    /**
     * @brief Performs Validity Check of supplied Filename.
     *
     * @param[in] filename
     *   Filename to check.
     *
     * @return If filename is valid.
     **/
    [[nodiscard]] bool checkFilename(
      const std::filesystem::path &filename) const;

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
     * @param[in] options
     *   Received Options.
     **/
    void receivedRequest(
      const boost::asio::ip::udp::endpoint &remote,
      Tftp::RequestType requestType,
      std::string_view filename,
      Tftp::TransferMode mode,
      const Tftp::Options::Options &options);

    /**
     * @brief Transmits a requested file (RRQ).
     *
     * @param[in] remote
     *   Remote address.
     * @param[in] filename
     *   Requested filename.
     * @param[in] options
     *   Received Options.
     **/
    void transmitFile(
      const boost::asio::ip::udp::endpoint &remote,
      const std::filesystem::path &filename,
      const Tftp::Options::Options &options);

    /**
     * @brief Receives a requested file (WRQ).
     *
     * @param[in] remote
     *   Remote address.
     * @param[in] filename
     *   Requested filename.
     * @param[in] options
     *   Received Options.
     **/
    void receiveFile(
      const boost::asio::ip::udp::endpoint &remote,
      const std::filesystem::path &filename,
      const Tftp::Options::Options &options);

    //! Options Description
    boost::program_options::options_description optionsDescription;

    //! Base Directory of TFTP Server
    std::filesystem::path baseDir;
    //! TFTP Configuration
    Tftp::TftpConfiguration configuration;
    //! TFTP Server Instance
    Tftp::Server::TftpServerPtr server;

    //! I/O Context
    boost::asio::io_context ioContext;
    //! ASIO Signal Handler
    boost::asio::signal_set signals;
};

#endif
