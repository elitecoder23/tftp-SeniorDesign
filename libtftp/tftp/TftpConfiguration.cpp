/*
 * $Date$
 * $Revision$
 */
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

namespace Tftp {

TftpConfiguration::TftpConfiguration() :
  tftpTimeout( DEFAULT_TFTP_RECEIVE_TIMEOUT),
  tftpRetries( DEFAULT_TFTP_RETRIES),
  tftpServerPort( DEFAULT_TFTP_PORT),
  handleTransferSizeOption( false)
{
}

TftpConfiguration::TftpConfiguration( const ptree &properties) :
  tftpTimeout( properties.get( "tftp.timeout", DEFAULT_TFTP_RECEIVE_TIMEOUT)),
  tftpRetries( properties.get( "tftp.retries", DEFAULT_TFTP_RETRIES)),
  tftpServerPort( properties.get( "tftp.port", DEFAULT_TFTP_PORT)),

  handleTransferSizeOption( properties.get( "tftp.option.transferSize", false)),

  blockSizeOption( properties.get_optional< uint16_t>( "tftp.option.blockSize.value")),

  timoutOption( properties.get_optional< uint16_t>( "tftp.option.timeout.value"))
{
}

TftpConfiguration::ptree TftpConfiguration::toProperties() const
{
  ptree properties;

  properties.add( "tftp.timeout", tftpTimeout);
  properties.add( "tftp.retries", tftpRetries);
  properties.add( "tftp.port", tftpServerPort);

  properties.add( "tftp.option.transferSize", handleTransferSizeOption);

  if (blockSizeOption)
  {
    properties.add( "tftp.option.blockSize.value", blockSizeOption);
  }

  if (timoutOption)
  {
    properties.add( "tftp.option.timeout.value", timoutOption);
  }

  return properties;
}

TftpConfiguration::options_description TftpConfiguration::getOptions()
{
  options_description options( "TFTP options");

  options.add_options()
  ("server-port",
    boost::program_options::value( &tftpServerPort)->default_value(
      Tftp::DEFAULT_TFTP_PORT),
    "UDP port, where the server is listen"
  )
  (
    "blocksize-option",
    boost::program_options::value( &blockSizeOption),
    "blocksize of transfers to use"
  )
  (
    "timeout-option",
    boost::program_options::value( &timoutOption),
    "If set handles the timeout option negotiation"
  )
  (
    "handle-transfer-size-option",
    boost::program_options::bool_switch( &handleTransferSizeOption),
    "If set handles the transfer size option negotiation"
  );

  return options;
}

TftpConfiguration::OptionList TftpConfiguration::getClientOptions(
  const OptionList &baseOptions) const
{
  OptionList options = baseOptions;

  if ( handleTransferSizeOption)
  {
    options.addTransferSizeOption();
  }

  if ( blockSizeOption)
  {
    options.addBlocksizeOption( blockSizeOption.get());
  }

  if ( timoutOption)
  {
    options.addTimeoutOption( timoutOption.get());
  }

  return options;
}

TftpConfiguration::OptionList TftpConfiguration::getServerOptions(
  const OptionList &baseOptions) const
{
  OptionList options = baseOptions;

  if ( handleTransferSizeOption)
  {
    options.addTransferSizeOption();
  }

  if ( blockSizeOption)
  {
    options.addBlocksizeOption( blockSizeOption.get());
  }

  if ( timoutOption)
  {
    options.addTimeoutOption( timoutOption.get());
  }

  return options;
}

}
