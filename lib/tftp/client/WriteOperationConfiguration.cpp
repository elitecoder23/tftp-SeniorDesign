/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Struct Tftp::Client::WriteOperationConfiguration.
 **/

#include "WriteOperationConfiguration.hpp"

#include <tftp/TftpConfiguration.hpp>

#include <utility>

namespace Tftp::Client {

WriteOperationConfiguration::WriteOperationConfiguration(
  const TftpConfiguration &configuration,
  TftpOptionsConfiguration optionsConfiguration,
  OptionNegotiationHandler optionNegotiationHandler,
  OperationCompletedHandler completionHandler,
  TransmitDataHandlerPtr dataHandler,
  std::string filename,
  Packets::TransferMode mode,
  Packets::Options additionalOptions,
  boost::asio::ip::udp::endpoint remote,
  std::optional< boost::asio::ip::udp::endpoint > local ) :
  tftpTimeout{ configuration.tftpTimeout },
  tftpRetries{ configuration.tftpRetries },
  optionsConfiguration{std::move( optionsConfiguration )},
  optionNegotiationHandler{ std::move( optionNegotiationHandler ) },
  completionHandler{ std::move( completionHandler ) },
  dataHandler{ std::move( dataHandler ) },
  filename{ std::move( filename ) },
  mode{ mode },
  additionalOptions{ std::move( additionalOptions ) },
  remote{ std::move( remote ) },
  local{ std::move( local ) }
{
}

WriteOperationConfiguration::WriteOperationConfiguration(
  std::chrono::seconds tftpTimeout,
  uint16_t tftpRetries,
  TftpOptionsConfiguration optionsConfiguration,
  OptionNegotiationHandler optionNegotiationHandler,
  OperationCompletedHandler completionHandler,
  TransmitDataHandlerPtr dataHandler,
  std::string filename,
  Packets::TransferMode mode,
  Packets::Options additionalOptions,
  boost::asio::ip::udp::endpoint remote,
  std::optional< boost::asio::ip::udp::endpoint > local ) :
  tftpTimeout{ tftpTimeout },
  tftpRetries{ tftpRetries },
  optionsConfiguration{std::move( optionsConfiguration )},
  optionNegotiationHandler{ std::move( optionNegotiationHandler ) },
  completionHandler{ std::move( completionHandler ) },
  dataHandler{ std::move( dataHandler ) },
  filename{ std::move( filename ) },
  mode{ mode },
  additionalOptions{ std::move( additionalOptions ) },
  remote{ std::move( remote ) },
  local{ std::move( local ) }
{
}

}
