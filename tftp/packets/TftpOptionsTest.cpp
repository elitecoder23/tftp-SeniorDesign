/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Implementation of Unit Tests of Tftp::Packets.
 **/

#include <tftp/packets/TftpOptions.hpp>

#include <tftp/packets/PacketException.hpp>

#include <boost/test/unit_test.hpp>

#include <cstdint>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpTest )
BOOST_AUTO_TEST_SUITE( PacketsTest )

//! TftpOptions_name tests
BOOST_AUTO_TEST_CASE( name )
{
  BOOST_CHECK( TftpOptions_name( KnownOptions::BlockSize ) == "blksize" );
  BOOST_CHECK( TftpOptions_name( KnownOptions::Timeout ) == "timeout" );
  BOOST_CHECK( TftpOptions_name( KnownOptions::TransferSize ) == "tsize" );

  BOOST_CHECK( TftpOptions_name( KnownOptions{ 100 } ).empty() );
}

//! TftpOptions_options tests
BOOST_AUTO_TEST_CASE( toString )
{
  Options options{};
  BOOST_CHECK_NO_THROW( TftpOptions_toString( options ) );

  options.try_emplace( "name", "value" );
  BOOST_CHECK_NO_THROW( TftpOptions_toString( options ) );
}

//! TftpOptions_options tests
BOOST_AUTO_TEST_CASE( optionsDecode )
{
  RawOptions emptyRawOptions{};
  BOOST_CHECK( TftpOptions_options( RawOptions{} ).empty() ) ;

  RawOptions rawOptions{
    'O', 'P', 'T', 'I', 'O', 'N', '1', 0x00, 'V', 'A', 'L', 'U', 'E', '1', 0x00,
    'O', 'P', 'T', 'I', 'O', 'N', '2', 0x00, 'V', 'A', 'L', 'U', 'E', '2', 0x00
  };
  auto options{ TftpOptions_options( RawOptionsSpan{ rawOptions.begin(), rawOptions.end() } ) };

  BOOST_CHECK( !options.empty() );
  BOOST_CHECK( options.size() == 2 );
  BOOST_CHECK( ( options == Options{ { "OPTION1", "VALUE1"}, { "OPTION2", "VALUE2" } } ) );

  BOOST_CHECK_THROW(
    TftpOptions_options( RawOptions{ rawOptions.begin(), rawOptions.begin()+7U } ),
    TftpException );
  BOOST_CHECK_THROW(
    TftpOptions_options( RawOptions{ rawOptions.begin(), rawOptions.begin()+8U } ),
    TftpException );
  BOOST_CHECK_THROW(
    TftpOptions_options( RawOptions{ rawOptions.begin(), rawOptions.begin()+14U } ),
    TftpException );
}

//! TftpOptions_getOption tests
BOOST_AUTO_TEST_CASE( getOptions )
{
  Options options{
    { "opt1", "" },
    { "opt2", "65535" },
    { "opt3", "65536" },
    { "opt4", "0" },
    { "opt5", "100" },
  };

  BOOST_CHECK( TftpOptions_getOption< uint16_t >( options, "optX" ) == std::make_pair( true, std::optional< uint16_t >{} ) );

  BOOST_CHECK_NO_THROW( TftpOptions_getOption< uint16_t >( options, "opt1" ) );// == std::make_pair( false, std::optional< uint16_t >{} ) );

  BOOST_CHECK( TftpOptions_getOption< uint16_t >( options, "opt2" ) == std::make_pair( true, std::optional< uint16_t >{ 65535 } ) );

  BOOST_CHECK( TftpOptions_getOption< uint16_t >( options, "opt3" ) == std::make_pair( false, std::optional< uint16_t >{} ) );

  BOOST_CHECK( TftpOptions_getOption< uint16_t >( options, "opt4", 0, 100 ) == std::make_pair( true, std::optional< uint16_t >{ 0 } ) );
  BOOST_CHECK( TftpOptions_getOption< uint16_t >( options, "opt4", 1, 100 ) == std::make_pair( false, std::optional< uint16_t >{} ) );

  BOOST_CHECK( TftpOptions_getOption< uint16_t >( options, "opt5", 0, 100 ) == std::make_pair( true, std::optional< uint16_t >{ 100 } ) );
  BOOST_CHECK( TftpOptions_getOption< uint16_t >( options, "opt5", 0, 99 ) == std::make_pair( false, std::optional< uint16_t >{} ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
