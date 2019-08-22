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

#include <tftp/TftpLogger.hpp>

namespace Tftp::Client {

TftpClientImpl::TftpClientImpl(
  const TftpConfiguration &configuration,
  const Options::OptionList& additionalOptions):
  configurationV( configuration),
  optionsV( configuration.clientOptions( additionalOptions)),
  work( ioContext)
{
}

void TftpClientImpl::entry() noexcept
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
    "Start TFTP client I/O context";

  ioContext.run();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
    "TFTP client I/O context finished";
}

void TftpClientImpl::stop()
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
    << "Stop TFTP client I/O context";

  ioContext.stop();
}

OperationPtr TftpClientImpl::readRequestOperation(
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  const std::string &filename,
  const TransferMode mode,
  const boost::asio::ip::udp::endpoint &local)
{
  return std::make_shared< ReadRequestOperationImpl>(
    ioContext,
    dataHandler,
    completionHandler,
    *this,
    remote,
    filename,
    mode,
    local);
}

OperationPtr TftpClientImpl::writeRequestOperation(
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  const std::string &filename,
  const TransferMode mode,
  const boost::asio::ip::udp::endpoint &local)
{
  return std::make_shared< WriteRequestOperationImpl>(
    ioContext,
    dataHandler,
    completionHandler,
    *this,
    remote,
    filename,
    mode,
    local);
}

const Tftp::TftpConfiguration& TftpClientImpl::configuration() const
{
  return configurationV;
}

const Options::OptionList& TftpClientImpl::options() const
{
  return optionsV;
}

}
