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
 * @brief Definition of Class Tftp::TftpOptionsConfiguration.
 **/

#include "TftpOptionsConfiguration.hpp"

#include <tftp/packets/Packets.hpp>

#include <boost/property_tree/ptree.hpp>

namespace Tftp {

TftpOptionsConfiguration::TftpOptionsConfiguration(
  const boost::property_tree::ptree &properties )
{
  fromProperties( properties );
}

void TftpOptionsConfiguration::fromProperties(
  const boost::property_tree::ptree &properties )
{
  handleTransferSizeOption = properties.get( "transfer_size", false );
  blockSizeOption = properties.get_optional< uint16_t>( "block_size" );
  // convert to std::chrono (is similar to std::optional::transform)
  timeoutOption =
    properties.get_optional< std::chrono::seconds::rep >( "timeout" )
      .map(
        []( const auto timeout ) { return std::chrono::seconds{ timeout }; } );
}

boost::property_tree::ptree TftpOptionsConfiguration::toProperties(
  const bool full ) const
{
  boost::property_tree::ptree properties{};

  if ( full || handleTransferSizeOption )
  {
    properties.add( "transfer_size", handleTransferSizeOption );
  }

  if ( full || blockSizeOption )
  {
    properties.add( "block_size", blockSizeOption );
  }

  if ( full || timeoutOption )
  {
    // like std::optional::transform
    properties.add(
      "timeout",
      timeoutOption.map( []( const auto &timeOut ) { return timeOut.count(); } ) );
  }

  return properties;
}

boost::program_options::options_description TftpOptionsConfiguration::options()
{
  boost::program_options::options_description options{
    "TFTP Option Negotiation Options" };

  options.add_options()
  (
    "block-size-option",
    boost::program_options::value( &blockSizeOption )
      ->value_name( "block-size" )
      ->implicit_value( Packets::BlockSizeOptionDefault ),
    "Negotiates the TFTP block size for transfers"
  )
  (
    "timeout-option",
    boost::program_options::value< std::chrono::seconds::rep >()
      ->value_name( "timeout" )
      ->implicit_value( DefaultTftpReceiveTimeout.count() )
      ->notifier(
        [this]( const auto timeoutOptionInt )
          {
            timeoutOption = std::chrono::seconds{ timeoutOptionInt };
          } ),
    "Handles the TFTP timeout option negotiation with the given timeout in seconds"
  )
  (
    "handle-transfer-size-option",
    boost::program_options::bool_switch( &handleTransferSizeOption ),
    "Handles the TFTP transfer size option negotiation"
  );

  return options;
}

}
