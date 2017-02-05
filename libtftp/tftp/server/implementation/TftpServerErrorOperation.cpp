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
 * @brief Definition of class Tftp::Server::TftpServerErrorOperation.
 **/

#include "TftpServerErrorOperation.hpp"

#include <tftp/packet/ErrorPacket.hpp>

#include <helper/Logger.hpp>

namespace Tftp {
namespace Server {

TftpServerErrorOperation::TftpServerErrorOperation(
  const UdpAddressType &clientAddress,
  const UdpAddressType &from,
  const ErrorCode errorCode,
  const string &errorMessage) :
  TftpServerBaseErrorOperation( clientAddress, from),
  errorCode( errorCode),
  errorMessage( errorMessage)
{
}

TftpServerErrorOperation::TftpServerErrorOperation(
  const UdpAddressType &clientAddress,
  const ErrorCode errorCode,
  const string &errorMessage) :
  TftpServerBaseErrorOperation( clientAddress),
  errorCode( errorCode),
  errorMessage( errorMessage)
{
}

void TftpServerErrorOperation::operator()()
{
  sendError( Tftp::Packet::ErrorPacket( errorCode, errorMessage));
}

}
}
