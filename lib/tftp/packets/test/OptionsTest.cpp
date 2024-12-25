// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Implementation of Unit Tests of Tftp::Packets.
 **/

#include <tftp/packets/Options.hpp>

#include <tftp/packets/PacketException.hpp>

#include <boost/test/unit_test.hpp>

#include <cstdint>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpTest )
BOOST_AUTO_TEST_SUITE( PacketsTest )
BOOST_AUTO_TEST_SUITE( OptionsTest )

//! Option String
//                              00000000 0111111 11112222 2222223
//                              12345678 9012345 67890123 4567890
static const char optionStr[]{ "OPTION1\0VALUE1\0OPTION2\0VALUE2" }; // auto null-terminate
static_assert( sizeof( optionStr ) == 30U );

//! Whole Option String
static const std::string_view optionStr1{ optionStr, 30 };
//! Option String - missing Null-character after name
static const std::string_view optionStr2{ optionStr, 7 };
//! Option String - missing option value after name
static const std::string_view optionStr3{ optionStr, 8 };
//! Option String - missing Null-character after option value
static const std::string_view optionStr4{ optionStr, 14 };

//! Options_options tests
BOOST_AUTO_TEST_CASE( toString )
{
  Options options{};
  BOOST_CHECK_NO_THROW( boost::ignore_unused( Options_toString( options ) ) );

  options.try_emplace( "name", "value" );
  BOOST_CHECK_NO_THROW( boost::ignore_unused( Options_toString( options ) ) );
}

//! Options_options tests
BOOST_AUTO_TEST_CASE( optionsDecode )
{
  BOOST_CHECK( Options_options( {} ).empty() ) ;

  auto options{ Options_options( optionStr1 ) };

  BOOST_CHECK( !options.empty() );
  BOOST_CHECK( options.size() == 2 );
  BOOST_CHECK( ( options == Options{ { "OPTION1", "VALUE1"}, { "OPTION2", "VALUE2" } } ) );

  BOOST_CHECK_THROW( boost::ignore_unused( Options_options( optionStr2 ) ), TftpException );
  BOOST_CHECK_THROW( boost::ignore_unused( Options_options( optionStr3 ) ), TftpException );
  BOOST_CHECK_THROW( boost::ignore_unused( Options_options( optionStr4 ) ), TftpException );
}

//! TftpOptions_getOption tests
BOOST_AUTO_TEST_CASE( getOptions )
{
  const Options options{
    { "opt1", "" },
    { "opt2", "65535" },
    { "opt3", "65536" },
    { "opt4", "0" },
    { "opt5", "100" },
  };

  auto opt1{ options };
  BOOST_CHECK( Options_getOption< uint16_t >( opt1, "optX" ) == std::make_pair( true, std::optional< uint16_t >{} ) );

  auto opt2{ options };
  BOOST_CHECK( Options_getOption< uint16_t >( opt2, "opt1" ) == std::make_pair( false, std::optional< uint16_t >{} ) );

  auto opt3{ options };
  BOOST_CHECK( Options_getOption< uint16_t >( opt3, "opt2" ) == std::make_pair( true, std::optional< uint16_t >{ 65535 } ) );

  auto opt4{ options };
  BOOST_CHECK( Options_getOption< uint16_t >( opt4, "opt3" ) == std::make_pair( false, std::optional< uint16_t >{} ) );

  auto opt5a{ options };
  BOOST_CHECK( Options_getOption< uint16_t >( opt5a, "opt4", 0, 100 ) == std::make_pair( true, std::optional< uint16_t >{ 0 } ) );
  auto opt5b{ options };
  BOOST_CHECK( Options_getOption< uint16_t >( opt5b, "opt4", 1, 100 ) == std::make_pair( false, std::optional< uint16_t >{} ) );

  auto opt6a{ options };
  BOOST_CHECK( Options_getOption< uint16_t >( opt6a, "opt5", 0, 100 ) == std::make_pair( true, std::optional< uint16_t >{ 100 } ) );
  auto opt6b{ options };
  BOOST_CHECK( Options_getOption< uint16_t >( opt6b, "opt5", 0, 99 ) == std::make_pair( false, std::optional< uint16_t >{} ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
