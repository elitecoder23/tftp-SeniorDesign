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

TftpConfiguration::TftpConfiguration( void) :
  tftpTimeout( DEFAULT_TFTP_RECEIVE_TIMEOUT),
  tftpRetries( DEFAULT_TFTP_RETRIES),
  tftpServerPort( DEFAULT_TFTP_PORT),
  handleTransferSizeOption( false)
{
}

TftpConfiguration::TftpConfiguration(
  const boost::property_tree::ptree &properties) :
  tftpTimeout( properties.get( "tftp.timeout", DEFAULT_TFTP_RECEIVE_TIMEOUT)),
  tftpRetries( properties.get( "tftp.retries", DEFAULT_TFTP_RETRIES)),
  tftpServerPort( properties.get( "tftp.port", DEFAULT_TFTP_PORT)),

  handleTransferSizeOption( properties.get( "tftp.option.transferSize", false)),

  blockSizeOption( properties.get_optional< uint16_t>( "tftp.option.blockSize.value")),

  timoutOption( properties.get_optional< uint16_t>( "tftp.option.timeout.value"))
{
}

boost::property_tree::ptree TftpConfiguration::toProperties( void) const
{
  boost::property_tree::ptree properties;

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

TftpConfiguration::options_description TftpConfiguration::getOptions( void)
{
  //! @todo for boost::options: https://github.com/boostorg/program_options/pull/18

  boost::program_options::options_description options(
    "TFTP options");

  options.add_options()
  ("server-port",
    boost::program_options::value( &tftpServerPort)->default_value(
      Tftp::DEFAULT_TFTP_PORT),
    "UDP port, where the server is listen"
  )
  (
    "blocksize-option",
    //boost::program_options::value( &blockSizeOption),
    boost::program_options::value< uint16_t>()->notifier(
      [this]( const uint16_t &value){
        this->blockSizeOption = value;
    }),
    "blocksize of transfers to use"
  )
  (
    "timeout-option",
//    boost::program_options::value( &timoutOption),
    boost::program_options::value< uint16_t>()->notifier(
      [this]( const uint16_t &value){
        this->timoutOption = value;
    }),
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
