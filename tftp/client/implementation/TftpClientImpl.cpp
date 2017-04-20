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
 * @brief Definition of class Tftp::Client::TftpClientImpl.
 **/

#include "TftpClientImpl.hpp"

#include <tftp/client/implementation/ReadRequestOperationImpl.hpp>
#include <tftp/client/implementation/WriteRequestOperationImpl.hpp>

namespace Tftp {
namespace Client {

TftpClientImpl::TftpClientImpl(
  const TftpConfiguration &configuration,
  const Options::OptionList& additionalOptions):
  configuration( configuration),
  options( configuration.getClientOptions( additionalOptions))
{
}

void TftpClientImpl::entry()
{
  ioService.run();
}

void TftpClientImpl::stop()
{
  ioService.stop();
}

OperationPtr TftpClientImpl::createReadRequestOperation(
  ReceiveDataOperationHandlerPtr dataHandler,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode,
  const UdpAddressType &from,
  OperationCompletedHandler completionHandler)
{
  return std::make_shared< ReadRequestOperationImpl>(
    ioService,
    dataHandler,
    *this,
    serverAddress,
    filename,
    mode,
    from,
    completionHandler);
}

OperationPtr TftpClientImpl::createReadRequestOperation(
  ReceiveDataOperationHandlerPtr dataHandler,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode,
  OperationCompletedHandler completionHandler)
{
  return std::make_shared< ReadRequestOperationImpl>(
    ioService,
    dataHandler,
    *this,
    serverAddress,
    filename,
    mode,
    completionHandler);
}

OperationPtr TftpClientImpl::createWriteRequestOperation(
  TransmitDataOperationHandlerPtr dataHandler,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode,
  const UdpAddressType &from,
  OperationCompletedHandler completionHandler)
{
  return std::make_shared< WriteRequestOperationImpl>(
    ioService,
    dataHandler,
    *this,
    serverAddress,
    filename,
    mode,
    from,
    completionHandler);
}

OperationPtr TftpClientImpl::createWriteRequestOperation(
  TransmitDataOperationHandlerPtr dataHandler,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode,
  OperationCompletedHandler completionHandler)
{
  return std::make_shared< WriteRequestOperationImpl>(
    ioService,
    dataHandler,
    *this,
    serverAddress,
    filename,
    mode,
    completionHandler);
}

const Tftp::TftpConfiguration& TftpClientImpl::getConfiguration() const
{
  return configuration;
}

const Options::OptionList& TftpClientImpl::getOptionList() const
{
  return options;
}

}
}
