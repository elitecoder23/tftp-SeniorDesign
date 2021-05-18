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

#include <tftp/packets/Packets.hpp>

#include <tftp/packets/PacketException.hpp>

#include <boost/test/unit_test.hpp>

#include <cstdint>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpTest)
BOOST_AUTO_TEST_SUITE( PacketsTest)

BOOST_AUTO_TEST_CASE( toString)
{
  Options options{};
  BOOST_CHECK_NO_THROW( TftpOptions_toString( options ) );

  options.insert( {"name", "value"});
  BOOST_CHECK_NO_THROW( TftpOptions_toString( options ) );
}


BOOST_AUTO_TEST_CASE( optionsDecode)
{
  RawOptions emptyRawOptions{};
  BOOST_CHECK( TftpOptions_options( emptyRawOptions.begin(), emptyRawOptions.end()) == Options{});

  RawOptions rawOptions{
    'O', 'P', 'T', 'I', 'O', 'N', '1', 0x00, 'V', 'A', 'L', 'U', 'E', '1', 0x00,
    'O', 'P', 'T', 'I', 'O', 'N', '2', 0x00, 'V', 'A', 'L', 'U', 'E', '2', 0x00
  };
  Options options{ TftpOptions_options( rawOptions.begin(), rawOptions.end())};

  BOOST_CHECK( !options.empty());
  BOOST_CHECK( options.size() == 2);
  BOOST_CHECK( ( options == Options{ { "OPTION1", "VALUE1"}, { "OPTION2", "VALUE2"}}));

  BOOST_CHECK_THROW(
    TftpOptions_options( rawOptions.begin(), rawOptions.begin()+7U),
    TftpException);
  BOOST_CHECK_THROW(
    TftpOptions_options( rawOptions.begin(), rawOptions.begin()+8U),
    TftpException);
  BOOST_CHECK_THROW(
    TftpOptions_options( rawOptions.begin(), rawOptions.begin()+14U),
    TftpException);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
