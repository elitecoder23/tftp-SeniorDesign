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
  defaultTftpPort{ defaultTftpPort },
  tftpTimeout{ DefaultTftpReceiveTimeout },
  tftpRetries{ DefaultTftpRetries },
  tftpServerPort{ defaultTftpPort },
  tftpOptions{},
  dally{ false }
{
}

TftpConfiguration::TftpConfiguration(
  const boost::property_tree::ptree &properties,
  const uint16_t defaultTftpPort ) :
  defaultTftpPort{ defaultTftpPort }
{
  fromProperties( properties );
}

void TftpConfiguration::fromProperties(
  const boost::property_tree::ptree &properties )
{
  //! @todo use std::optional::transform when C++23 is available
  const auto tftpTimeOutInt{ properties.get< std::chrono::seconds::rep >(
    "timeout",
    DefaultTftpReceiveTimeout.count() ) };
  tftpTimeout = std::chrono::seconds{ tftpTimeOutInt };
  tftpRetries = properties.get( "retries", DefaultTftpRetries ) ;
  tftpServerPort = properties.get( "port", defaultTftpPort );
  tftpOptions.fromProperties( properties.get_child( "options", {} ) );
  dally = properties.get( "dally", false ) ;
}

boost::property_tree::ptree TftpConfiguration::toProperties() const
{
  boost::property_tree::ptree properties{};

  if ( tftpTimeout != DefaultTftpReceiveTimeout )
  {
    properties.add( "timeout", tftpTimeout.count() );
  }
  if ( tftpRetries != DefaultTftpRetries )
  {
    properties.add( "retries", tftpRetries );
  }
  if ( tftpServerPort != defaultTftpPort )
  {
    properties.add( "port", tftpServerPort );
  }

  const auto tftpOptionsPtree{ tftpOptions.toProperties() };
  if ( !tftpOptionsPtree.empty() )
  {
    properties.add_child( "options", tftpOptionsPtree );
  }

  if ( dally )
  {
    properties.add( "dally", dally );
  }

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
  )
  (
    "dally",
    boost::program_options::bool_switch( &dally ),
    "Dally Option"
  );

  options.add( tftpOptions.options() );

  return options;
}

}
