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
 * @brief Definition of class Tftp::Server::TftpServer.
 **/

#include "TftpServer.hpp"

#include <tftp/server/implementation/TftpServerImpl.hpp>

namespace Tftp {
namespace Server {

const Tftp::UdpAddressType TftpServer::DefaultLocalEndpoint = UdpAddressType(
  boost::asio::ip::address_v4::any(),
  DEFAULT_TFTP_PORT);

TftpServerPtr TftpServer::createInstance(
  ReceivedTftpRequestHandler handler,
  const TftpConfiguration &configuration,
  const Options::OptionList& additionalOptions,
  const UdpAddressType &serverAddress)
{
  // create and return the real TFTP server
  return std::make_shared< TftpServerImpl>(
    handler,
    configuration,
    additionalOptions,
    serverAddress);
}

}
}
