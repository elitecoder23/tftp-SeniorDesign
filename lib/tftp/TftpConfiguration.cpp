// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::TftpConfiguration.
 **/

#include "TftpConfiguration.hpp"

#include <tftp/TftpException.hpp>

#include <boost/property_tree/ptree.hpp>

namespace Tftp {

TftpConfiguration::TftpConfiguration(
  const uint16_t defaultTftpPort ) noexcept :
  tftpServerPort{ defaultTftpPort },
  defaultTftpPort{ defaultTftpPort }
{
}

TftpConfiguration::TftpConfiguration(
  const boost::property_tree::ptree &properties,
  const uint16_t defaultTftpPort ) :
  tftpServerPort{ defaultTftpPort },
  defaultTftpPort{ defaultTftpPort }
{
  fromProperties( properties );
}

TftpConfiguration& TftpConfiguration::operator=( const TftpConfiguration &other ) noexcept
{
  tftpTimeout = other.tftpTimeout;
  tftpRetries = other.tftpRetries;
  tftpServerPort = other.tftpServerPort;
  dally = other.dally;

  return *this;
}

TftpConfiguration& TftpConfiguration::operator=( TftpConfiguration &&other ) noexcept
{
  tftpTimeout = other.tftpTimeout;
  tftpRetries = other.tftpRetries;
  tftpServerPort = other.tftpServerPort;
  dally = other.dally;

  return *this;
}

void TftpConfiguration::fromProperties( const boost::property_tree::ptree &properties )
{
  // convert to std::chrono (is similar to std::optional::transform)
  tftpTimeout = properties.get_optional< std::chrono::seconds::rep >( "timeout" )
    .map(
      []( const auto timeout )
      {
        return std::chrono::seconds{ timeout };
      } )
    .get_value_or( tftpTimeout );
  tftpRetries = properties.get( "retries", tftpRetries ) ;
  tftpServerPort = properties.get( "port", defaultTftpPort );
  dally = properties.get( "dally", dally ) ;
}

boost::property_tree::ptree TftpConfiguration::toProperties( const bool full ) const
{
  boost::property_tree::ptree properties{};

  if ( full || ( tftpTimeout != DefaultTftpReceiveTimeout ) )
  {
    properties.add( "timeout", tftpTimeout.count() );
  }

  if ( full || ( tftpRetries != DefaultTftpRetries ) )
  {
    properties.add( "retries", tftpRetries );
  }

  if ( full || ( tftpServerPort != defaultTftpPort ) )
  {
    properties.add( "port", tftpServerPort );
  }

  if ( full || dally )
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
    boost::program_options::value( &tftpServerPort )
      ->value_name( "port" ),
    "UDP port, where the server is listen"
  )
  (
    "tftp-timeout",
    boost::program_options::value< std::chrono::seconds::rep >()
      ->value_name( "timeout" )
      ->notifier(
        [ this ]( const auto timeoutInt )
        {
          tftpTimeout = std::chrono::seconds{ timeoutInt };
        } ),
    "Default TFTP packet timeout in seconds, when no timeout option is negotiated"
  )
  (
    "dally",
    boost::program_options::value( &dally )
      ->implicit_value( true, "true" )
      ->value_name( "true|false" ),
    "TFTP Dally Option"
    " - Wait when last ACK has been sent to prevent aborts on last ACK miss"
  );

  return options;
}

}
