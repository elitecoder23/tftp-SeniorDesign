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
 * @brief Implementation of unit tests of class Tftp::Options::OptionList.
 **/

#include <tftp/options/OptionList.hpp>
#include <tftp/options/IntegerOption.hpp>

#include <helper/Logger.hpp>

#include <boost/test/unit_test.hpp>

#include <cstdint>

namespace Tftp {
namespace Options {

BOOST_AUTO_TEST_SUITE( OptionNegotiationTest)

BOOST_AUTO_TEST_CASE( constructor)
{
  OptionList optionList;

  BOOST_CHECK( !optionList.hasOptions());
  BOOST_CHECK( optionList.getOptions().empty());
  BOOST_CHECK( optionList.getBlocksizeOption() == 0);
  BOOST_CHECK( optionList.getTimeoutOption() == 0);
  BOOST_CHECK( !optionList.hasTransferSizeOption());
}

BOOST_AUTO_TEST_CASE( serverNegotiation)
{
  OptionList serverOptions;

  serverOptions.addBlocksizeOptionServer( TFTP_OPTION_BLOCKSIZE_MIN, TFTP_OPTION_BLOCKSIZE_MAX);
  serverOptions.addTimeoutOptionServer( TFTP_OPTION_TIMEOUT_MIN, TFTP_OPTION_TIMEOUT_MAX);
  serverOptions.addTransferSizeOption();

  BOOST_CHECK( serverOptions.hasOptions());
  BOOST_CHECK( !serverOptions.getOptions().empty());
  BOOST_CHECK( serverOptions.getBlocksizeOption() == TFTP_OPTION_BLOCKSIZE_MAX);
  BOOST_CHECK( serverOptions.getTimeoutOption() == TFTP_OPTION_TIMEOUT_MAX);
  BOOST_CHECK( serverOptions.hasTransferSizeOption());

  OptionList clientOptions;

  OptionList negotiatedOptions( serverOptions.negotiateServer( clientOptions));
  BOOST_CHECK( !negotiatedOptions.hasOptions());
  BOOST_CHECK( negotiatedOptions.getOptions().empty());
  BOOST_CHECK( negotiatedOptions.getBlocksizeOption() == 0);
  BOOST_CHECK( negotiatedOptions.getTimeoutOption() == 0);
  BOOST_CHECK( !negotiatedOptions.hasTransferSizeOption());

  clientOptions.addBlocksizeOptionClient( TFTP_OPTION_BLOCKSIZE_MIN+10);
  clientOptions.addTimeoutOptionClient( TFTP_OPTION_TIMEOUT_MIN+10);
  clientOptions.addTransferSizeOption( 15);
  BOOST_CHECK( clientOptions.getBlocksizeOption() == TFTP_OPTION_BLOCKSIZE_MIN+10);
  BOOST_CHECK( clientOptions.getTimeoutOption() == TFTP_OPTION_TIMEOUT_MIN+10);
  BOOST_CHECK( clientOptions.getTransferSizeOption() == 15);

  negotiatedOptions= serverOptions.negotiateServer( clientOptions);
  BOOST_CHECK( negotiatedOptions.hasOptions());
  BOOST_CHECK( !negotiatedOptions.getOptions().empty());
  BOOST_CHECK( negotiatedOptions.getBlocksizeOption() == TFTP_OPTION_BLOCKSIZE_MIN+10);
  BOOST_CHECK( negotiatedOptions.getTimeoutOption() == TFTP_OPTION_TIMEOUT_MIN+10);
  BOOST_CHECK( negotiatedOptions.getTransferSizeOption() == 15);
}

BOOST_AUTO_TEST_CASE( clientNegotiation)
{
  OptionList clientOptions;

  clientOptions.addBlocksizeOptionClient( TFTP_OPTION_BLOCKSIZE_MIN+10);
  clientOptions.addTimeoutOptionClient( TFTP_OPTION_TIMEOUT_MIN+10);
  clientOptions.addTransferSizeOption( 15);

  OptionList serverOptions;

  serverOptions.addBlocksizeOptionClient( TFTP_OPTION_BLOCKSIZE_MIN+9);
  serverOptions.addTimeoutOptionClient( TFTP_OPTION_TIMEOUT_MIN+10);
  serverOptions.addTransferSizeOption( 99);

  OptionList negotiatedOptions( clientOptions.negotiateClient( serverOptions));
  BOOST_CHECK( negotiatedOptions.hasOptions());
  BOOST_CHECK( !negotiatedOptions.getOptions().empty());
  BOOST_CHECK( negotiatedOptions.getBlocksizeOption() == TFTP_OPTION_BLOCKSIZE_MIN+9);
  BOOST_CHECK( negotiatedOptions.getTimeoutOption() == TFTP_OPTION_TIMEOUT_MIN+10);
  BOOST_CHECK( negotiatedOptions.getTransferSizeOption() == 99);
}

BOOST_AUTO_TEST_SUITE_END()

}
}
