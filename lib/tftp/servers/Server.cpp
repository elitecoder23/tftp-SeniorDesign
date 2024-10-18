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
 * @brief Definition of Class Tftp::Servers::Server.
 **/

#include "Server.hpp"

#include <tftp/servers/implementation/ServerImpl.hpp>

namespace Tftp::Servers {

const boost::asio::ip::udp::endpoint Server::DefaultLocalEndpoint{
  boost::asio::ip::address_v4::any(),
  DefaultTftpPort };

ServerPtr Server::instance( boost::asio::io_context &ioContext )
{
  // create and return the real TFTP server
  return std::make_shared< ServerImpl >( ioContext );
}

}
