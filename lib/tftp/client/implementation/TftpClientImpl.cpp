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
 * @brief Definition of Class Tftp::Client::TftpClientImpl.
 **/

#include "TftpClientImpl.hpp"

#include "tftp/client/implementation/ReadOperationImpl.hpp"
#include "tftp/client/implementation/WriteOperationImpl.hpp"

namespace Tftp::Client {

TftpClientImpl::TftpClientImpl( boost::asio::io_context &ioContext ) :
  ioContext{ ioContext }
{
}

ReadOperationPtr TftpClientImpl::readOperation()
{
  return std::make_shared< ReadOperationImpl >( ioContext );
}

WriteOperationPtr TftpClientImpl::writeOperation()
{
  return std::make_shared< WriteOperationImpl >( ioContext );
}

}
