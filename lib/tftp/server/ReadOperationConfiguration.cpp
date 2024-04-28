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
 * @brief Definition of Struct Tftp::Server::ReadOperationConfiguration.
 **/

#include "ReadOperationConfiguration.hpp"

#include "tftp/TftpConfiguration.hpp"

#include <utility>

namespace Tftp::Server {

ReadOperationConfiguration::ReadOperationConfiguration(
  const TftpConfiguration &configuration,
  TftpOptionsConfiguration optionsConfiguration,
  OperationCompletedHandler completionHandler,
  TransmitDataHandlerPtr dataHandler,
  boost::asio::ip::udp::endpoint remote,
  Packets::TftpOptions clientOptions,
  Packets::Options additionalNegotiatedOptions,
  std::optional< boost::asio::ip::udp::endpoint > local ) :
  tftpTimeout{ configuration.tftpTimeout },
  tftpRetries{ configuration.tftpRetries },
  optionsConfiguration{ std::move( optionsConfiguration ) },
  completionHandler{ std::move( completionHandler ) },
  dataHandler{ std::move( dataHandler ) },
  remote{ std::move( remote ) },
  clientOptions{ clientOptions },
  additionalNegotiatedOptions{ std::move( additionalNegotiatedOptions ) },
  local{ std::move( local ) }
{
}

ReadOperationConfiguration::ReadOperationConfiguration(
  const std::chrono::seconds tftpTimeout,
  const uint16_t tftpRetries,
  TftpOptionsConfiguration optionsConfiguration,
  OperationCompletedHandler completionHandler,
  TransmitDataHandlerPtr dataHandler,
  boost::asio::ip::udp::endpoint remote,
  Packets::TftpOptions clientOptions,
  Packets::Options additionalNegotiatedOptions,
  std::optional< boost::asio::ip::udp::endpoint > local ) :
  tftpTimeout{ tftpTimeout },
  tftpRetries{ tftpRetries },
  optionsConfiguration{ std::move( optionsConfiguration ) },
  completionHandler{ std::move( completionHandler ) },
  dataHandler{ std::move( dataHandler ) },
  remote{ std::move( remote ) },
  clientOptions{ clientOptions },
  additionalNegotiatedOptions{ std::move( additionalNegotiatedOptions ) },
  local{ std::move( local ) }
{
}

}
