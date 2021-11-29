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

#include <tftp/client/implementation/ReadRequestOperationImpl.hpp>
#include <tftp/client/implementation/WriteRequestOperationImpl.hpp>

#include <tftp/TftpLogger.hpp>

namespace Tftp::Client {

TftpClientImpl::TftpClientImpl(
  boost::asio::io_context &ioContext,
  const uint8_t tftpTimeout,
  const uint16_t tftpRetries ):
  tftpTimeout{ tftpTimeout },
  tftpRetries{ tftpRetries },
  ioContext{ ioContext }
{
}

OperationPtr TftpClientImpl::readRequestOperation(
  OptionNegotiationHandler optionNegotiationHandler,
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const TransferMode mode,
  const TftpOptionsConfiguration &optionsConfiguration,
  const Options &additionalOptions,
  const bool dally )
{
  auto operation{ std::make_shared< ReadRequestOperationImpl>(
    ioContext,
    tftpTimeout,
    tftpRetries,
    optionNegotiationHandler,
    dataHandler,
    completionHandler,
    remote,
    filename,
    mode,
    optionsConfiguration,
    additionalOptions,
    dally ) };

  operation->request();

  return operation;
}

OperationPtr TftpClientImpl::readRequestOperation(
  OptionNegotiationHandler optionNegotiationHandler,
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const TransferMode mode,
  const TftpOptionsConfiguration &optionsConfiguration,
  const Options &additionalOptions,
  const bool dally,
  const boost::asio::ip::udp::endpoint &local)
{
  auto operation{ std::make_shared< ReadRequestOperationImpl>(
    ioContext,
    tftpTimeout,
    tftpRetries,
    optionNegotiationHandler,
    dataHandler,
    completionHandler,
    remote,
    filename,
    mode,
    optionsConfiguration,
    additionalOptions,
    dally,
    local ) };

  operation->request();

  return operation;
}

OperationPtr TftpClientImpl::writeRequestOperation(
  OptionNegotiationHandler optionNegotiationHandler,
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const TransferMode mode,
  const TftpOptionsConfiguration &optionsConfiguration,
  const Options &additionalOptions )
{
  auto operation{ std::make_shared< WriteRequestOperationImpl>(
    ioContext,
    tftpTimeout,
    tftpRetries,
    optionNegotiationHandler,
    dataHandler,
    completionHandler,
    remote,
    filename,
    mode,
    optionsConfiguration,
    additionalOptions ) };

  operation->request();

  return operation;
}

OperationPtr TftpClientImpl::writeRequestOperation(
  OptionNegotiationHandler optionNegotiationHandler,
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const TransferMode mode,
  const TftpOptionsConfiguration &optionsConfiguration,
  const Options &additionalOptions,
  const boost::asio::ip::udp::endpoint &local )
{
  auto operation{ std::make_shared< WriteRequestOperationImpl>(
    ioContext,
    tftpTimeout,
    tftpRetries,
    optionNegotiationHandler,
    dataHandler,
    completionHandler,
    remote,
    filename,
    mode,
    optionsConfiguration,
    additionalOptions,
    local ) };

  operation->request();

  return operation;
}

}
