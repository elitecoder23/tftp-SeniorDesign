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
 * @brief Declaration of class TftpServerApplication.
 **/

#ifndef TFTPSERVERAPPLICATION_HPP
#define TFTPSERVERAPPLICATION_HPP

#include <tftp/server/TftpServer.hpp>

#include <boost/filesystem.hpp>
#include <boost/application.hpp>
#include <boost/program_options.hpp>

/**
 * @brief The TFTP server application.
 **/
class TftpServerApplication
{
  public:
    /**
     * @brief Constructor, which instantiates the TFT server application.
     *
     * @param[in] baseDir
     *   Base directory, where the server works in.
     * @param[in] port
     *   UDP port, where the server listens on.
     **/
    TftpServerApplication( boost::application::context &context);

    /**
     * @brief Destructor of the TFTP server application.
     **/
    virtual ~TftpServerApplication() noexcept;

    int operator()();

    bool stop();

  private:
    using string = std::string;

    bool handleCommandLine();

    void shutdown();

    /**
     * @brief
     *
     * @param filename
     *
     * @throw
     **/
    bool checkFilename( const boost::filesystem::path &filename) const;

    /**
     *
     **/
    void receivedRequest(
      Tftp::RequestType requestType,
      const Tftp::UdpAddressType &from,
      const string &filename,
      Tftp::TransferMode mode,
      const Tftp::Options::OptionList &options);

    void transmitFile(
      const Tftp::UdpAddressType &from,
      const string &filename,
      const Tftp::Options::OptionList &options);

    void receiveFile(
      const Tftp::UdpAddressType &from,
      const string &filename,
      const Tftp::Options::OptionList &options);


    boost::application::context &context;
    boost::program_options::options_description optionsDescription;

    //! base directory of TFTP server
    boost::filesystem::path baseDir;
    //! The TFTP configuration
    Tftp::TftpConfiguration configuration;
    //! The TFTP server instance
    Tftp::Server::TftpServerPtr server;
};

#endif
