/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::TftpOptionsConfiguration.
 **/

#include "TftpOptionsConfiguration.hpp"

#include <boost/property_tree/ptree.hpp>

namespace Tftp {

TftpOptionsConfiguration::TftpOptionsConfiguration() noexcept :
  handleTransferSizeOption{ false }
{
}

TftpOptionsConfiguration::TftpOptionsConfiguration(
  const boost::property_tree::ptree &properties )
{
  fromProperties( properties );
}

void TftpOptionsConfiguration::fromProperties(
  const boost::property_tree::ptree &properties )
{
  handleTransferSizeOption = properties.get( "transferSize", false );
  blockSizeOption = properties.get_optional< uint16_t>( "blockSize" );
  //! @todo use std::optional::transform when C++23 is available
  const auto timeoutOptionInt{
    properties.get_optional< std::chrono::seconds::rep >( "timeout" ) };
  timeoutOption.reset();
  if ( timeoutOptionInt )
  {
    timeoutOption = std::chrono::seconds{ *timeoutOptionInt };
  }
}

boost::property_tree::ptree TftpOptionsConfiguration::toProperties() const
{
  boost::property_tree::ptree properties{};

  if ( handleTransferSizeOption )
  {
    properties.add( "transferSize", handleTransferSizeOption );
  }

  if ( blockSizeOption )
  {
    properties.add( "blockSize", blockSizeOption );
  }

  if ( timeoutOption )
  {
    properties.add( "timeout", timeoutOption->count() );
  }

  return properties;
}

boost::program_options::options_description TftpOptionsConfiguration::options()
{
  boost::program_options::options_description options{ "TFTP Option Negotiation Options" };

  options.add_options()
  (
    "block-size-option",
    boost::program_options::value( &blockSizeOption )->value_name( "block size" ),
    "block size of transfers to use."
  )
  (
    "timeout-option",
    boost::program_options::value< std::chrono::seconds::rep >()->value_name(
      "timeout" )->notifier(
        [this]( const auto timeoutOptionInt )->void
          {
            timeoutOption = std::chrono::seconds{ timeoutOptionInt };
          } ),
    "If set handles the timeout option negotiation (seconds)."
  )
  (
    "handle-transfer-size-option",
    boost::program_options::bool_switch( &handleTransferSizeOption ),
    "If set handles the transfer size option negotiation."
  );

  return options;
}

}
