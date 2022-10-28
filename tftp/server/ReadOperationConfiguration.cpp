/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Struct Tftp::Server::ReadOperationConfiguration.
 **/

#include "ReadOperationConfiguration.hpp"

#include <tftp/TftpConfiguration.hpp>

#include <utility>

namespace Tftp::Server {

ReadOperationConfiguration::ReadOperationConfiguration(
  const TftpConfiguration &configuration,
  TftpOptionsConfiguration optionsConfiguration,
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  boost::asio::ip::udp::endpoint remote,
  Packets::TftpOptions clientOptions,
  Packets::Options additionalNegotiatedOptions,
  std::optional< boost::asio::ip::udp::endpoint > local ) :
  tftpTimeout{ configuration.tftpTimeout },
  tftpRetries{ configuration.tftpRetries },
  optionsConfiguration{ std::move( optionsConfiguration ) },
  dataHandler{ std::move( dataHandler ) },
  completionHandler{ std::move( completionHandler ) },
  remote{ std::move( remote ) },
  clientOptions{ clientOptions },
  additionalNegotiatedOptions{ std::move( additionalNegotiatedOptions ) },
  local{ std::move( local ) }
{
}

}
