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

void TftpClientImpl::operator()()
{
  ioService.run();
}

void TftpClientImpl::stop()
{
  ioService.stop();
}

OperationPtr TftpClientImpl::createReadRequestOperation(
  ReceiveDataOperationHandler &handler,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode,
  const UdpAddressType &from,
  OperationCompletedHandler operationCompletedHandler)
{
  return std::make_shared< ReadRequestOperationImpl>(
    ioService,
    handler,
    *this,
    serverAddress,
    filename,
    mode,
    from,
    operationCompletedHandler);
}

OperationPtr TftpClientImpl::createReadRequestOperation(
  ReceiveDataOperationHandler &handler,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode,
  OperationCompletedHandler operationCompletedHandler)
{
  return std::make_shared< ReadRequestOperationImpl>(
    ioService,
    handler,
    *this,
    serverAddress,
    filename,
    mode,
    operationCompletedHandler);
}

OperationPtr TftpClientImpl::createWriteRequestOperation(
  TransmitDataOperationHandler &handler,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode,
  const UdpAddressType &from,
  OperationCompletedHandler operationCompletedHandler)
{
  return std::make_shared< WriteRequestOperationImpl>(
    ioService,
    handler,
    *this,
    serverAddress,
    filename,
    mode,
    from,
    operationCompletedHandler);
}

OperationPtr TftpClientImpl::createWriteRequestOperation(
  TransmitDataOperationHandler &handler,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode,
  OperationCompletedHandler operationCompletedHandler)
{
  return std::make_shared< WriteRequestOperationImpl>(
    ioService,
    handler,
    *this,
    serverAddress,
    filename,
    mode,
    operationCompletedHandler);
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
