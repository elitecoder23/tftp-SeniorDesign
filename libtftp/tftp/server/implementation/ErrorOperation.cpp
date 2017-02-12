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
 * @brief Definition of class Tftp::Server::ErrorOperation.
 **/

#include "ErrorOperation.hpp"

#include <tftp/packets/ErrorPacket.hpp>

#include <helper/Logger.hpp>

namespace Tftp {
namespace Server {

ErrorOperation::ErrorOperation(
  const UdpAddressType &clientAddress,
  const UdpAddressType &from,
  const ErrorCode errorCode,
  const string &errorMessage) :
  BaseErrorOperation( clientAddress, from),
  errorCode( errorCode),
  errorMessage( errorMessage)
{
}

ErrorOperation::ErrorOperation(
  UdpAddressType &&clientAddress,
  UdpAddressType &&from,
  ErrorCode errorCode,
  string &&errorMessage) :
  BaseErrorOperation( clientAddress, from),
  errorCode( errorCode),
  errorMessage( errorMessage)
{
}

ErrorOperation::ErrorOperation(
  const UdpAddressType &clientAddress,
  const ErrorCode errorCode,
  const string &errorMessage) :
  BaseErrorOperation( clientAddress),
  errorCode( errorCode),
  errorMessage( errorMessage)
{
}

ErrorOperation::ErrorOperation(
  UdpAddressType &&clientAddress,
  const ErrorCode errorCode,
  string &&errorMessage) :
  BaseErrorOperation( clientAddress),
  errorCode( errorCode),
  errorMessage( errorMessage)
{
}

void ErrorOperation::operator()()
{
  sendError( Packets::ErrorPacket( errorCode, errorMessage));
}

}
}
