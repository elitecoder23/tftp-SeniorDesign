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
 * @brief Definition of Struct Tftp::Server::ServerConfiguration.
 **/

#include "ServerConfiguration.hpp"

namespace Tftp::Server {

ServerConfiguration::ServerConfiguration(
  ReceivedTftpRequestHandler handler,
  std::optional< boost::asio::ip::udp::endpoint > serverAddress ) :
  handler{ std::move( handler ) },
  serverAddress{ std::move( serverAddress ) }
{
}

}
