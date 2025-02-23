// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests of class Tftp::Packets::Packet.
 **/

#include <boost/test/unit_test.hpp>

#include <tftp/packets/Packet.hpp>
#include <tftp/packets/PacketException.hpp>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpTest )
BOOST_AUTO_TEST_SUITE( PacketsTest )
BOOST_AUTO_TEST_SUITE( PacketTest )

//! Constructor test
BOOST_AUTO_TEST_CASE( packetType )
{
  using Helper::operator ""_b;

  BOOST_CHECK( Packet::packetType( Helper::RawData{ 0x00_b, 0x01_b } ) == PacketType::ReadRequest );
  BOOST_CHECK( Packet::packetType( Helper::RawData{ 0x00_b, 0x02_b } ) == PacketType::WriteRequest );
  BOOST_CHECK( Packet::packetType( Helper::RawData{ 0x00_b, 0x03_b } ) == PacketType::Data );
  BOOST_CHECK( Packet::packetType( Helper::RawData{ 0x00_b, 0x04_b } ) == PacketType::Acknowledgement );
  BOOST_CHECK( Packet::packetType( Helper::RawData{ 0x00_b, 0x05_b } ) == PacketType::Error );
  BOOST_CHECK( Packet::packetType( Helper::RawData{ 0x00_b, 0x06_b } ) == PacketType::OptionsAcknowledgement );

  BOOST_CHECK( Packet::packetType( Helper::RawData{ 0x00_b, 0x00_b } ) == PacketType::Invalid );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
