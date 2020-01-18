/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Implementation of Unit Tests of Class Tftp::Options::OptionList.
 **/

#include <tftp/options/OptionList.hpp>
#include <tftp/options/OptionNegotiation.hpp>

#include <boost/test/unit_test.hpp>

#include <cstdint>

namespace Tftp::Options {

BOOST_AUTO_TEST_SUITE( TftpTest)
BOOST_AUTO_TEST_SUITE( OptionsTest)
BOOST_AUTO_TEST_SUITE( OptionListTest)

//! optionName test
BOOST_AUTO_TEST_CASE( optionName)
{
  BOOST_CHECK( OptionList::optionName( KnownOptions::BlockSize) == "blksize");
  BOOST_CHECK( OptionList::optionName( KnownOptions::Timeout) == "timeout");
  BOOST_CHECK( OptionList::optionName( KnownOptions::TransferSize) == "tsize");
  BOOST_CHECK( OptionList::optionName( static_cast< KnownOptions>( 99)) == std::string{});
}

BOOST_AUTO_TEST_CASE( toString)
{
  Options options{};
  BOOST_CHECK_NO_THROW( OptionList::toString( options));

  options.insert( {"name", "value"});
  BOOST_CHECK_NO_THROW( OptionList::toString( options));
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

  BOOST_CHECK_THROW(
    OptionList::options( rawOptions.begin(), rawOptions.begin()+7U),
    TftpException);
  BOOST_CHECK_THROW(
    OptionList::options( rawOptions.begin(), rawOptions.begin()+8U),
    TftpException);
  BOOST_CHECK_THROW(
    OptionList::options( rawOptions.begin(), rawOptions.begin()+14U),
    TftpException);
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

BOOST_AUTO_TEST_CASE( various)
{
  OptionList optionList{};

  BOOST_CHECK( optionList.empty());
  BOOST_CHECK( !optionList.has( "xxx"));
  BOOST_CHECK( optionList.option( "xxx").empty());
  BOOST_CHECK( !optionList.has( KnownOptions::TransferSize));
  BOOST_CHECK( !optionList.has( KnownOptions{ 99}));

  BOOST_CHECK_NO_THROW( optionList.remove( "xxx"));
  BOOST_CHECK_NO_THROW( optionList.remove( KnownOptions{ 99}));

  optionList.option( "xxx","val1", {});
  BOOST_CHECK( optionList.has( "xxx"));
  BOOST_CHECK( optionList.option( "xxx") == "val1");

  optionList.option( "xxx", "val2", {});
  BOOST_CHECK( optionList.has( "xxx"));
  BOOST_CHECK( optionList.option( "xxx") == "val2");

  BOOST_CHECK_NO_THROW( optionList.remove( "xxx"));
  BOOST_CHECK( !optionList.has( "xxx"));
}

BOOST_AUTO_TEST_CASE( blockSizeClient)
{
  OptionList optionList{};

  BOOST_CHECK( !optionList.has( KnownOptions::BlockSize));
  BOOST_CHECK( !optionList.has( "blksize"));
  BOOST_CHECK( !optionList.blocksize());

  BOOST_CHECK_NO_THROW( optionList.blocksizeClient( 1000));
  BOOST_CHECK( optionList.has( KnownOptions::BlockSize));
  BOOST_CHECK( optionList.has( "blksize"));
  BOOST_CHECK( optionList.blocksize() == 1000);

  BOOST_CHECK_THROW( optionList.blocksizeClient( 5), TftpException);
  BOOST_CHECK_THROW(
    optionList.blocksizeClient( BlocksizeOptionMax, BlocksizeOptionMax + 1U),
    TftpException);
  BOOST_CHECK_THROW( optionList.blocksizeClient( 999, 1000), TftpException);
}

BOOST_AUTO_TEST_CASE( blockSizeServer)
{
  OptionList optionList{};

  BOOST_CHECK( !optionList.blocksize());
  BOOST_CHECK( !optionList.has( "blksize"));
  BOOST_CHECK( !optionList.blocksize());

  BOOST_CHECK_NO_THROW( optionList.blocksizeServer( 1000, 1001));
  BOOST_CHECK( optionList.has( KnownOptions::BlockSize));
  BOOST_CHECK( optionList.has( "blksize"));
  BOOST_CHECK( optionList.blocksize() == 1001);

  BOOST_CHECK_THROW( optionList.blocksizeServer( 5), TftpException);
  BOOST_CHECK_THROW(
    optionList.blocksizeServer( BlocksizeOptionMin, BlocksizeOptionMax + 1U),
    TftpException);
  BOOST_CHECK_THROW( optionList.blocksizeServer( 1000, 999), TftpException);
}

BOOST_AUTO_TEST_CASE( timeoutOptionClient)
{
  OptionList optionList{};

  BOOST_CHECK( !optionList.has( KnownOptions::Timeout));
  BOOST_CHECK( !optionList.has( "timeout"));
  BOOST_CHECK( !optionList.timeoutOption());

  BOOST_CHECK_NO_THROW( optionList.timeoutOptionClient( 5));
  BOOST_CHECK( optionList.has( KnownOptions::Timeout));
  BOOST_CHECK( optionList.has( "timeout"));
  BOOST_CHECK( optionList.timeoutOption() == 5);

  BOOST_CHECK_THROW( optionList.timeoutOptionClient( 0), TftpException);
}

BOOST_AUTO_TEST_CASE( timeoutOptionServer)
{
  OptionList optionList{};

  BOOST_CHECK( !optionList.has( KnownOptions::Timeout));
  BOOST_CHECK( !optionList.has( "timeout"));
  BOOST_CHECK( !optionList.timeoutOption());

  BOOST_CHECK_NO_THROW( optionList.timeoutOptionServer( 5, 10));
  BOOST_CHECK( optionList.has( KnownOptions::Timeout));
  BOOST_CHECK( optionList.has( "timeout"));
  BOOST_CHECK( optionList.timeoutOption() == 10);

  BOOST_CHECK_THROW( optionList.timeoutOptionServer( 0), TftpException);
  BOOST_CHECK_THROW( optionList.timeoutOptionServer( 1, 0), TftpException);
  BOOST_CHECK_THROW( optionList.timeoutOptionServer( 5, 4), TftpException);
}

BOOST_AUTO_TEST_CASE( transferSizeOption)
{
  OptionList optionList{};

  BOOST_CHECK( !optionList.transferSizeOption());
  BOOST_CHECK( !optionList.has( KnownOptions::TransferSize));
  BOOST_CHECK( !optionList.has( "tsize"));

  BOOST_CHECK_NO_THROW( optionList.transferSizeOption( 1000));
  BOOST_CHECK( optionList.has( KnownOptions::TransferSize));
  BOOST_CHECK( optionList.has( "tsize"));
  BOOST_CHECK( optionList.transferSizeOption() = 1000);

  BOOST_CHECK_NO_THROW( optionList.removeTransferSizeOption());
  BOOST_CHECK( !optionList.transferSizeOption());
}

//! server negotiation test
BOOST_AUTO_TEST_CASE( serverNegotiation)
{
  OptionList serverOptions{};

  serverOptions.blocksizeServer( BlocksizeOptionMin, BlocksizeOptionMax);
  serverOptions.timeoutOptionServer( TimeoutOptionMin, TimeoutOptionMax);
  serverOptions.transferSizeOption( 0);

  BOOST_CHECK( !serverOptions.empty());
  BOOST_CHECK( !serverOptions.options().empty());
  BOOST_CHECK( serverOptions.blocksize() == BlocksizeOptionMax);
  BOOST_CHECK( serverOptions.timeoutOption() == TimeoutOptionMax);
  BOOST_CHECK( serverOptions.transferSizeOption());

  OptionList clientOptions;

  OptionList negotiatedOptions{
    serverOptions.negotiateServer( clientOptions.options())};
  BOOST_CHECK( negotiatedOptions.empty());
  BOOST_CHECK( negotiatedOptions.options().empty());
  BOOST_CHECK( !negotiatedOptions.blocksize());
  BOOST_CHECK( !negotiatedOptions.timeoutOption());
  BOOST_CHECK( !negotiatedOptions.transferSizeOption());

  clientOptions.blocksizeClient( BlocksizeOptionMin+10);
  clientOptions.timeoutOptionClient( TimeoutOptionMin+10);
  clientOptions.transferSizeOption( 15);
  clientOptions.option( "option1", "value", {});
  BOOST_CHECK( clientOptions.blocksize() == BlocksizeOptionMin+10);
  BOOST_CHECK( clientOptions.timeoutOption() == TimeoutOptionMin+10);
  BOOST_CHECK( clientOptions.transferSizeOption() == 15);

  negotiatedOptions= serverOptions.negotiateServer( clientOptions.options());
  BOOST_CHECK( !negotiatedOptions.empty());
  BOOST_CHECK( !negotiatedOptions.options().empty());
  BOOST_CHECK( negotiatedOptions.blocksize() == BlocksizeOptionMin+10);
  BOOST_CHECK( negotiatedOptions.timeoutOption() == TimeoutOptionMin+10);
  BOOST_CHECK( negotiatedOptions.transferSizeOption() == 15);
  BOOST_CHECK( !negotiatedOptions.has( "option1"));
}

//! client negotiation test
BOOST_AUTO_TEST_CASE( clientNegotiation)
{
  OptionList clientOptions;

  clientOptions.blocksizeClient( BlocksizeOptionMin+10U);
  clientOptions.timeoutOptionClient( TimeoutOptionMin+10U);
  clientOptions.transferSizeOption( 15U);

  BOOST_CHECK( clientOptions.negotiateClient( {}).empty());
  BOOST_CHECK( clientOptions.negotiateClient( {{ "option1", "value1"}}).empty());

  OptionList serverOptions{};

  serverOptions.blocksizeClient( BlocksizeOptionMin+9U);
  serverOptions.timeoutOptionClient( TimeoutOptionMin+10U);
  serverOptions.transferSizeOption( 99);

  OptionList negotiatedOptions( clientOptions.negotiateClient( serverOptions.options()));
  BOOST_CHECK( !negotiatedOptions.empty());
  BOOST_CHECK( !negotiatedOptions.options().empty());
  BOOST_CHECK( negotiatedOptions.blocksize() == BlocksizeOptionMin+9U);
  BOOST_CHECK( negotiatedOptions.timeoutOption() == TimeoutOptionMin+10U);
  BOOST_CHECK( negotiatedOptions.transferSizeOption() == 99U);

  serverOptions.timeoutOptionClient( TimeoutOptionMin);
  BOOST_CHECK( clientOptions.negotiateClient( serverOptions.options()).empty());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
