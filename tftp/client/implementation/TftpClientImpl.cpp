/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Client::TftpClientImpl.
 **/

#include "TftpClientImpl.hpp"

#include <tftp/client/implementation/ReadOperationImpl.hpp>
#include <tftp/client/implementation/WriteOperationImpl.hpp>

#include <tftp/TftpLogger.hpp>

namespace Tftp::Client {

TftpClientImpl::TftpClientImpl(
  boost::asio::io_context &ioContext,
  ClientConfiguration configuration ):
  ioContext{ ioContext },
  configurationV{ std::move( configuration ) }
{
}

OperationPtr TftpClientImpl::readOperation(
  ReadOperationConfiguration configuration )
{
  return std::make_shared< ReadOperationImpl >(
    ioContext,
    configurationV.tftpTimeout,
    configurationV.tftpRetries,
    configurationV.dally,
    std::move( configuration ) );
}

OperationPtr TftpClientImpl::writeOperation(
  WriteOperationConfiguration configuration )
{
  return std::make_shared< WriteOperationImpl >(
    ioContext,
    configurationV.tftpTimeout,
    configurationV.tftpRetries,
    std::move( configuration ) );
}

}
