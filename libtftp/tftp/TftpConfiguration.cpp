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
  tftpTimeout( properties.get( "timeout", DEFAULT_TFTP_RECEIVE_TIMEOUT)),
  tftpRetries( properties.get( "retries", DEFAULT_TFTP_RETRIES)),
  tftpServerPort( properties.get( "port", DEFAULT_TFTP_PORT)),

  handleTransferSizeOption( properties.get( "option.transferSize", false)),

  blockSizeOption( properties.get_optional< uint16_t>( "option.blockSize.value")),

  timeoutOption( properties.get_optional< uint16_t>( "option.timeout.value"))
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

TftpConfiguration::options_description TftpConfiguration::getOptions()
{
  //! @todo for boost::options: https://github.com/boostorg/program_options/pull/18
  //! should be in source on boost 1.64

  options_description options( "TFTP options");

  options.add_options()
  ("server-port",
    boost::program_options::value( &tftpServerPort)->default_value(
      tftpServerPort),
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
    //boost::program_options::value( &timoutOption),
    boost::program_options::value< uint16_t>()->notifier(
      [this]( const uint16_t &value){
        this->timeoutOption = value;
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
    options.addBlocksizeOptionClient( blockSizeOption.get());
  }

  if ( timeoutOption)
  {
    options.addTimeoutOptionClient( timeoutOption.get());
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
    options.addBlocksizeOptionServer(
      TFTP_OPTION_BLOCKSIZE_MIN,
      blockSizeOption.get());
  }

  if ( timeoutOption)
  {
    options.addTimeoutOptionServer(
      TFTP_OPTION_TIMEOUT_MIN,
      timeoutOption.get());
  }

  return options;
}

}
