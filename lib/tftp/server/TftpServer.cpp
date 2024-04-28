// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Server::TftpServer.
 **/

#include "TftpServer.hpp"

#include "tftp/server/implementation/TftpServerImpl.hpp"

namespace Tftp::Server {

const boost::asio::ip::udp::endpoint TftpServer::DefaultLocalEndpoint{
  boost::asio::ip::address_v4::any(),
  DefaultTftpPort };

TftpServerPtr TftpServer::instance( boost::asio::io_context &ioContext )
{
  // create and return the real TFTP server
  return std::make_shared< TftpServerImpl >( ioContext );
}

}
