/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Struct Tftp::Server::WriteOperationConfiguration.
 **/

#include "WriteOperationConfiguration.hpp"

#include <tftp/TftpConfiguration.hpp>

#include <utility>

namespace Tftp::Server {

WriteOperationConfiguration::WriteOperationConfiguration(
  const TftpConfiguration &configuration,
  TftpOptionsConfiguration optionsConfiguration,
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  boost::asio::ip::udp::endpoint remote,
  Packets::TftpOptions clientOptions,
  Packets::Options additionalNegotiatedOptions,
  std::optional< boost::asio::ip::udp::endpoint > local ) :
  tftpTimeout{ configuration.tftpTimeout },
  tftpRetries{ configuration.tftpRetries },
  dally{ configuration.dally },
  optionsConfiguration{ std::move( optionsConfiguration ) },
  dataHandler{ std::move( dataHandler ) },
  completionHandler{ std::move( completionHandler ) },
  remote{ std::move( remote ) },
  clientOptions{ std::move( clientOptions ) },
  additionalNegotiatedOptions{ std::move( additionalNegotiatedOptions ) },
  local{ std::move( local ) }
{
}

}
