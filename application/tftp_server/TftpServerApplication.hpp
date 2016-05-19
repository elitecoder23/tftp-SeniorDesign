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
 * @brief Declaration of class TftpServerApplication.
 **/

#ifndef TFTPSERVERAPPLICATION_HPP
#define TFTPSERVERAPPLICATION_HPP

#include <tftp/server/TftpServer.hpp>

#include <boost/filesystem.hpp>
#include <boost/application.hpp>
#include <boost/program_options.hpp>

using Tftp::TransferMode;
using Tftp::TftpConfiguration;
using Tftp::UdpAddressType;
using Tftp::Server::NewRequestHandler;
using Tftp::Server::TftpServer;
using Tftp::Server::TftpServerPtr;
using Tftp::Options::OptionList;

using std::string;

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
    virtual ~TftpServerApplication( void) noexcept;

    int operator()( void);

    bool stop( void);

  private:
    bool handleCommandLine( void);

    void shutdown( void);

    /**
     * @brief
     *
     * @param filename
     *
     * @throw
     **/
    void checkFilename( const boost::filesystem::path &filename) const;

    /**
     *
     **/
    void receivedReadRequest(
      const UdpAddressType &from,
      const string &filename,
      TransferMode mode,
      const OptionList &options);

    /**
     *
     **/
    void receivedWriteRequest(
      const UdpAddressType &from,
      const string &filename,
      TransferMode mode,
      const OptionList &options);

    boost::application::context &context;
    boost::program_options::options_description optionsDescription;

    //! base directory of TFTP server
    boost::filesystem::path baseDir;
    //! server port
    uint16_t port;
    //! The TFTP configuration
    TftpConfiguration configuration;
    //! The TFTP server instance
    TftpServerPtr server;
};

#endif
