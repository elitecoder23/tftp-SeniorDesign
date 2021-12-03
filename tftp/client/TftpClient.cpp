/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Client::TftpClient.
 **/

#include "TftpClient.hpp"

#include <tftp/client/implementation/TftpClientImpl.hpp>

namespace Tftp::Client {

TftpClientPtr TftpClient::instance(
  boost::asio::io_context &ioContext,
  const uint8_t tftpTimeout,
  const uint16_t tftpRetries,
  const bool dally )
{
  return std::make_shared< TftpClientImpl>(
    ioContext,
    tftpTimeout,
    tftpRetries,
    dally );
}

}
