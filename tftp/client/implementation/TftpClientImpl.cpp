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

TftpClientImpl::TftpClientImpl( const TftpConfiguration &configuration):
  configurationV{ configuration},
  work{ ioContext}
{
}

void TftpClientImpl::entry() noexcept
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info)
    << "Start TFTP client I/O context";

  ioContext.run();

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info)
    << "TFTP client I/O context finished";
}

void TftpClientImpl::stop()
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info)
    << "Stop TFTP client I/O context";

  ioContext.stop();
}

OperationPtr TftpClientImpl::readRequestOperation(
  OptionNegotiationHandler optionNegotiationHandler,
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const TransferMode mode,
  const Options::OptionList &clientOptions)
{
  return std::make_shared< ReadRequestOperationImpl>(
    ioContext,
    configurationV.tftpTimeout,
    configurationV.tftpRetries,
    configurationV.handleTransferSizeOption,
    optionNegotiationHandler,
    dataHandler,
    completionHandler,
    remote,
    filename,
    mode,
    clientOptions);
}

OperationPtr TftpClientImpl::readRequestOperation(
  OptionNegotiationHandler optionNegotiationHandler,
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const TransferMode mode,
  const Options::OptionList &clientOptions,
  const boost::asio::ip::udp::endpoint &local)
{
  return std::make_shared< ReadRequestOperationImpl>(
    ioContext,
    configurationV.tftpTimeout,
    configurationV.tftpRetries,
    configurationV.handleTransferSizeOption,
    optionNegotiationHandler,
    dataHandler,
    completionHandler,
    remote,
    filename,
    mode,
    clientOptions,
    local);
}

OperationPtr TftpClientImpl::writeRequestOperation(
  OptionNegotiationHandler optionNegotiationHandler,
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const TransferMode mode,
  const Options::OptionList &clientOptions)
{
  return std::make_shared< WriteRequestOperationImpl>(
    ioContext,
    configurationV.tftpTimeout,
    configurationV.tftpRetries,
    configurationV.handleTransferSizeOption,
    optionNegotiationHandler,
    dataHandler,
    completionHandler,
    remote,
    filename,
    mode,
    clientOptions);
}

OperationPtr TftpClientImpl::writeRequestOperation(
  OptionNegotiationHandler optionNegotiationHandler,
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const TransferMode mode,
  const Options::OptionList &clientOptions,
  const boost::asio::ip::udp::endpoint &local)
{
  return std::make_shared< WriteRequestOperationImpl>(
    ioContext,
    configurationV.tftpTimeout,
    configurationV.tftpRetries,
    configurationV.handleTransferSizeOption,
    optionNegotiationHandler,
    dataHandler,
    completionHandler,
    remote,
    filename,
    mode,
    clientOptions,
    local);
}

}
