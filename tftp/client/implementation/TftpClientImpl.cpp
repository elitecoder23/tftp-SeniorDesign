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
  const uint8_t tftpTimeout,
  const uint16_t tftpRetries,
  const bool dally ):
  ioContext{ ioContext },
  tftpTimeout{ tftpTimeout },
  tftpRetries{ tftpRetries },
  dally{ dally }
{
}

OperationPtr TftpClientImpl::readOperation(
  OptionNegotiationHandler optionNegotiationHandler,
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  TransferMode mode,
  const TftpOptionsConfiguration &optionsConfiguration,
  const Options &additionalOptions )
{
  return std::make_shared< ReadOperationImpl >(
    ioContext,
    tftpTimeout,
    tftpRetries,
    dally,
    optionNegotiationHandler,
    dataHandler,
    completionHandler,
    remote,
    filename,
    mode,
    optionsConfiguration,
    additionalOptions );
}

OperationPtr TftpClientImpl::readOperation(
  OptionNegotiationHandler optionNegotiationHandler,
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  TransferMode mode,
  const TftpOptionsConfiguration &optionsConfiguration,
  const Options &additionalOptions,
  const boost::asio::ip::udp::endpoint &local )
{
  return std::make_shared< ReadOperationImpl >(
    ioContext,
    tftpTimeout,
    tftpRetries,
    dally,
    optionNegotiationHandler,
    dataHandler,
    completionHandler,
    remote,
    filename,
    mode,
    optionsConfiguration,
    additionalOptions,
    local );
}

OperationPtr TftpClientImpl::writeOperation(
  OptionNegotiationHandler optionNegotiationHandler,
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  TransferMode mode,
  const TftpOptionsConfiguration &optionsConfiguration,
  const Options &additionalOptions )
{
  return std::make_shared< WriteOperationImpl >(
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
    additionalOptions );
}

OperationPtr TftpClientImpl::writeOperation(
  OptionNegotiationHandler optionNegotiationHandler,
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  TransferMode mode,
  const TftpOptionsConfiguration &optionsConfiguration,
  const Options &additionalOptions,
  const boost::asio::ip::udp::endpoint &local )
{
  return std::make_shared< WriteOperationImpl >(
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
    local );
}

}
