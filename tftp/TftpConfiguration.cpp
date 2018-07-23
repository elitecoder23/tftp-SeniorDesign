/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class Tftp::TftpConfiguration.
 **/

#include "TftpConfiguration.hpp"

#include <boost/property_tree/ptree.hpp>

namespace Tftp {

TftpConfiguration::TftpConfiguration() :
  tftpTimeout( DefaultTftpReceiveTimeout),
  tftpRetries( DefaultTftpRetries),
  tftpServerPort( DefaultTftpPort),
  handleTransferSizeOption( false)
{
}

TftpConfiguration::TftpConfiguration( const ptree &config) :
  tftpTimeout( config.get( "timeout", DefaultTftpReceiveTimeout)),
  tftpRetries( config.get( "retries", DefaultTftpRetries)),
  tftpServerPort( config.get( "port", DefaultTftpPort)),

  handleTransferSizeOption( config.get( "option.transferSize", false)),

  blockSizeOption( config.get_optional< uint16_t>( "option.blockSize.value")),

  timeoutOption( config.get_optional< uint8_t>( "option.timeout.value"))
{
}

TftpConfiguration::ptree TftpConfiguration::toProperties() const
{
  ptree properties;

  properties.add( "timeout", tftpTimeout);
  properties.add( "retries", tftpRetries);
  properties.add( "port", tftpServerPort);

  properties.add( "option.transferSize", handleTransferSizeOption);

  if (blockSizeOption)
  {
    properties.add( "option.blockSize.value", blockSizeOption);
  }

  if (timeoutOption)
  {
    properties.add( "option.timeout.value", timeoutOption);
  }

  return properties;
}

TftpConfiguration::options_description TftpConfiguration::options()
{
  //! @todo for boost::options: https://github.com/boostorg/program_options/pull/18
  //! should be in source on boost 1.64

  options_description options( "TFTP options");

  options.add_options()
  ("server-port",
    boost::program_options::value( &tftpServerPort)->default_value(
      tftpServerPort)->value_name( "port"),
    "UDP port, where the server is listen."
  )
  (
    "blocksize-option",
    //boost::program_options::value( &blockSizeOption),
    boost::program_options::value< uint16_t>()->notifier(
      [this]( const uint16_t &value){
        this->blockSizeOption = value;
    })->value_name( "blocksize"),
    "blocksize of transfers to use."
  )
  (
    "timeout-option",
    //boost::program_options::value( &timoutOption),
    boost::program_options::value< uint16_t>()->implicit_value(
      DefaultTftpReceiveTimeout)->notifier(
      [this]( const uint16_t &value){
        this->timeoutOption = static_cast< uint8_t>( value);
    })->value_name( "timeout"),
    "If set handles the timeout option negotiation (seconds)."
  )
  (
    "handle-transfer-size-option",
    boost::program_options::bool_switch( &handleTransferSizeOption),
    "If set handles the transfer size option negotiation."
  );

  return options;
}

Options::OptionList TftpConfiguration::clientOptions(
  const Options::OptionList &baseOptions) const
{
  Options::OptionList options{ baseOptions};

  if ( blockSizeOption)
  {
    options.blocksizeClient( blockSizeOption.get());
  }

  if ( timeoutOption)
  {
    options.timeoutOptionClient( timeoutOption.get());
  }

  return options;
}

Options::OptionList TftpConfiguration::serverOptions(
  const Options::OptionList &baseOptions) const
{
  Options::OptionList options{ baseOptions};

  if ( blockSizeOption)
  {
    options.blocksizeServer(
      BlocksizeOptionMin,
      blockSizeOption.get());
  }

  if ( timeoutOption)
  {
    options.timeoutOptionServer(
      TimeoutOptionMin,
      timeoutOption.get());
  }

  return options;
}

}
