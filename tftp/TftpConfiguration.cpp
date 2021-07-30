/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::TftpConfiguration.
 **/

#include "TftpConfiguration.hpp"

#include <helper/SafeCast.hpp>

#include <boost/property_tree/ptree.hpp>

namespace Tftp {

TftpConfiguration::TftpConfiguration(
  const uint16_t defaultTftpPort ) noexcept :
  tftpTimeout{ DefaultTftpReceiveTimeout },
  tftpRetries{ DefaultTftpRetries },
  tftpServerPort{ defaultTftpPort },
  tftpOptions{}
{
}

TftpConfiguration::TftpConfiguration(
  const boost::property_tree::ptree &config,
  const uint16_t defaultTftpPort ) :
  tftpTimeout{ config.get( "timeout", DefaultTftpReceiveTimeout ) },
  tftpRetries{ config.get( "retries", DefaultTftpRetries ) },
  tftpServerPort{ config.get( "port", defaultTftpPort ) },
  tftpOptions{ config.get_child( "options", {} ) }
{
}

boost::property_tree::ptree TftpConfiguration::toProperties() const
{
  boost::property_tree::ptree properties{};

  properties.add( "timeout", tftpTimeout );
  properties.add( "retries", tftpRetries );
  properties.add( "port", tftpServerPort );

  properties.add_child( "options", tftpOptions.toProperties() );

  return properties;
}

boost::program_options::options_description TftpConfiguration::options()
{
  boost::program_options::options_description options{ "TFTP Options" };

  options.add_options()
  (
    "server-port",
    boost::program_options::value( &tftpServerPort )->default_value(
      tftpServerPort )->value_name( "port" ),
    "UDP port, where the server is listen."
  );

  options.add( tftpOptions.options() );

  return options;
}

}
