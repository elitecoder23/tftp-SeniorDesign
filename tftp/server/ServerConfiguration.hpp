/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Struct Tftp::Server::ServerConfiguration.
 **/

#ifndef TFTP_SERVER_SERVERCONFIGURATION_HPP
#define TFTP_SERVER_SERVERCONFIGURATION_HPP

#include <tftp/server/Server.hpp>

#include <boost/asio/ip/udp.hpp>

#include <optional>

namespace Tftp::Server {

//! TFTP Server Configuration
struct ServerConfiguration
{
  //! TFTP Request Received Handler
  ReceivedTftpRequestHandler handler;
  //! Address where the TFTP server should listen on.
  std::optional< boost::asio::ip::udp::endpoint > serverAddress;

  /**
   * @brief Initialises TFTP Server Configuration.
   *
   * @param[in] handler
   *  New TFTP Request handler
   * @param[in] serverAddress
   *  TFTP Server Address.
   *  If not provided use default.
   **/
  ServerConfiguration(
    ReceivedTftpRequestHandler handler,
    std::optional< boost::asio::ip::udp::endpoint > serverAddress = {} );
};

}

#endif
