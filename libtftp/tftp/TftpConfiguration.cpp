/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * $Date$
 * $Revision$
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class Tftp::TftpConfiguration.
 **/

#include "TftpConfiguration.hpp"

namespace Tftp {

TftpConfiguration::TftpConfiguration( void) :
  tftpTimeout( DEFAULT_TFTP_RECEIVE_TIMEOUT),
  tftpRetries( DEFAULT_TFTP_RETRIES),
  tftpServerPort( DEFAULT_TFTP_PORT),
  handleTransferSizeOption( false),
  handleBlockSizeOption( false),
  blockSizeOptionValue( DEFAULT_DATA_SIZE),
  handleTimeoutOption( false),
  timoutOptionValue( DEFAULT_TFTP_RECEIVE_TIMEOUT)
{
}

TftpConfiguration::TftpConfiguration(
  const boost::property_tree::ptree &properties) :
  tftpTimeout( properties.get( "tftp.timeout", DEFAULT_TFTP_RECEIVE_TIMEOUT)),
  tftpRetries( properties.get( "tftp.retries", DEFAULT_TFTP_RETRIES)),
  tftpServerPort( properties.get( "tftp.port", DEFAULT_TFTP_PORT)),

  handleTransferSizeOption( properties.get( "tftp.option.transferSize", false)),

  handleBlockSizeOption( properties.get( "tftp.option.blockSize", false)),
  blockSizeOptionValue(
    properties.get( "tftp.option.blockSize.value", DEFAULT_DATA_SIZE)),

  handleTimeoutOption( properties.get( "tftp.option.tiemout", false)),
  timoutOptionValue(
    properties.get( "tftp.option.timeout.value", DEFAULT_TFTP_RECEIVE_TIMEOUT))
{
}

boost::property_tree::ptree TftpConfiguration::toProperties( void) const
{
  boost::property_tree::ptree properties;

  properties.add( "tftp.timeout", tftpTimeout);
  properties.add( "tftp.retries", tftpRetries);
  properties.add( "tftp.port", tftpServerPort);

  properties.add( "tftp.option.transferSize", handleTransferSizeOption);

  properties.add( "tftp.option.blockSize", handleBlockSizeOption);
  properties.add( "tftp.option.blockSize.value", blockSizeOptionValue);

  properties.add( "tftp.option.timeout", handleTimeoutOption);
  properties.add( "tftp.option.timeout.value", timoutOptionValue);

  return properties;
}

OptionList TftpConfiguration::getClientOptions(
  const OptionList &baseOptions) const
{
  OptionList options = baseOptions;

  if ( handleTransferSizeOption)
  {
    options.addTransferSizeOption();
  }

  if ( handleBlockSizeOption)
  {
    options.addBlocksizeOption( blockSizeOptionValue);
  }

  if ( handleTimeoutOption)
  {
    options.addTimeoutOption( timoutOptionValue);
  }

  return options;
}

OptionList TftpConfiguration::getServerOptions(
  const OptionList &baseOptions) const
{
  OptionList options = baseOptions;

  if ( handleTransferSizeOption)
  {
    options.addTransferSizeOption();
  }

  if ( handleBlockSizeOption)
  {
    options.addBlocksizeOption( blockSizeOptionValue);
  }

  if ( handleTimeoutOption)
  {
    options.addTimeoutOption( TFTP_OPTION_TIMEOUT_MIN, timoutOptionValue);
  }

  return options;
}

}
