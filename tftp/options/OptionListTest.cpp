/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Implementation of unit tests of class Tftp::Options::OptionList.
 **/

#include <tftp/options/OptionList.hpp>
#include <tftp/options/OptionNegotiation.hpp>

#include <boost/test/unit_test.hpp>

#include <cstdint>

namespace Tftp::Options {

BOOST_AUTO_TEST_SUITE( OptionListTest)

//! optionName test
BOOST_AUTO_TEST_CASE( optionName)
{
  BOOST_CHECK( OptionList::optionName( KnownOptions::BlockSize) == "blksize");
  BOOST_CHECK( OptionList::optionName( KnownOptions::Timeout) == "timeout");
  BOOST_CHECK( OptionList::optionName( KnownOptions::TransferSize) == "tsize");
  BOOST_CHECK( OptionList::optionName( static_cast< KnownOptions>( 99)) == std::string());
}

BOOST_AUTO_TEST_CASE( optionsDecode)
{
  OptionList::RawOptions emptyRawOptions{};
  BOOST_CHECK( OptionList::options( emptyRawOptions.begin(), emptyRawOptions.end()) == Options{});

  OptionList::RawOptions rawOptions{
    'O', 'P', 'T', 'I', 'O', 'N', '1', 0x00, 'V', 'A', 'L', 'U', 'E', '1', 0x00,
    'O', 'P', 'T', 'I', 'O', 'N', '2', 0x00, 'V', 'A', 'L', 'U', 'E', '2', 0x00
  };
  Options options{ OptionList::options( rawOptions.begin(), rawOptions.end())};

  BOOST_CHECK( !options.empty());
  BOOST_CHECK( options.size() == 2);
  BOOST_CHECK( ( options == Options{ { "OPTION1", "VALUE1"}, { "OPTION2", "VALUE2"}}));
}

//! constructor test
BOOST_AUTO_TEST_CASE( constructor)
{
  OptionList optionList{};

  BOOST_CHECK( optionList.empty());
  BOOST_CHECK( optionList.options().empty());
  BOOST_CHECK( !optionList.blocksize());
  BOOST_CHECK( !optionList.timeoutOption());
  BOOST_CHECK( !optionList.transferSizeOption());
  BOOST_CHECK( !optionList.transferSizeOption());
}

//! server negotiation test
BOOST_AUTO_TEST_CASE( serverNegotiation)
{
  OptionList serverOptions;

  serverOptions.blocksizeServer( BlocksizeOptionMin, BlocksizeOptionMax);
  serverOptions.timeoutOptionServer( TimeoutOptionMin, TimeoutOptionMax);
  serverOptions.transferSizeOption( 0);

  BOOST_CHECK( !serverOptions.empty());
  BOOST_CHECK( !serverOptions.options().empty());
  BOOST_CHECK( serverOptions.blocksize() == BlocksizeOptionMax);
  BOOST_CHECK( serverOptions.timeoutOption() == TimeoutOptionMax);
  BOOST_CHECK( serverOptions.transferSizeOption());

  OptionList clientOptions;

  OptionList negotiatedOptions( serverOptions.negotiateServer( clientOptions.options()));
  BOOST_CHECK( negotiatedOptions.empty());
  BOOST_CHECK( negotiatedOptions.options().empty());
  BOOST_CHECK( !negotiatedOptions.blocksize());
  BOOST_CHECK( !negotiatedOptions.timeoutOption());
  BOOST_CHECK( !negotiatedOptions.transferSizeOption());

  clientOptions.blocksizeClient( BlocksizeOptionMin+10);
  clientOptions.timeoutOptionClient( TimeoutOptionMin+10);
  clientOptions.transferSizeOption( 15);
  BOOST_CHECK( clientOptions.blocksize() == BlocksizeOptionMin+10);
  BOOST_CHECK( clientOptions.timeoutOption() == TimeoutOptionMin+10);
  BOOST_CHECK( clientOptions.transferSizeOption() == 15);

  negotiatedOptions= serverOptions.negotiateServer( clientOptions.options());
  BOOST_CHECK( !negotiatedOptions.empty());
  BOOST_CHECK( !negotiatedOptions.options().empty());
  BOOST_CHECK( negotiatedOptions.blocksize() == BlocksizeOptionMin+10);
  BOOST_CHECK( negotiatedOptions.timeoutOption() == TimeoutOptionMin+10);
  BOOST_CHECK( negotiatedOptions.transferSizeOption() == 15);
}

//! client negotiation test
BOOST_AUTO_TEST_CASE( clientNegotiation)
{
  OptionList clientOptions;

  clientOptions.blocksizeClient( BlocksizeOptionMin+10);
  clientOptions.timeoutOptionClient( TimeoutOptionMin+10);
  clientOptions.transferSizeOption( 15);

  OptionList serverOptions;

  serverOptions.blocksizeClient( BlocksizeOptionMin+9);
  serverOptions.timeoutOptionClient( TimeoutOptionMin+10);
  serverOptions.transferSizeOption( 99);

  OptionList negotiatedOptions( clientOptions.negotiateClient( serverOptions.options()));
  BOOST_CHECK( !negotiatedOptions.empty());
  BOOST_CHECK( !negotiatedOptions.options().empty());
  BOOST_CHECK( negotiatedOptions.blocksize() == BlocksizeOptionMin+9);
  BOOST_CHECK( negotiatedOptions.timeoutOption() == TimeoutOptionMin+10);
  BOOST_CHECK( negotiatedOptions.transferSizeOption() == 99);
}

BOOST_AUTO_TEST_SUITE_END()

}
