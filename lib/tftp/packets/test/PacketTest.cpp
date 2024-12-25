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
  BOOST_CHECK(
    Packet::packetType( std::as_bytes( std::span< const uint8_t >{ (uint8_t []){ 0x00, 0x01 } } ) )
      == PacketType::ReadRequest );
  BOOST_CHECK(
    Packet::packetType( std::as_bytes( std::span< const uint8_t >{ (uint8_t []){ 0x00, 0x02 } } ) )
      == PacketType::WriteRequest );
  BOOST_CHECK(
    Packet::packetType( std::as_bytes( std::span< const uint8_t >{ (uint8_t []){ 0x00, 0x03 } } ) )
      == PacketType::Data );
  BOOST_CHECK(
    Packet::packetType( std::as_bytes( std::span< const uint8_t >{ (uint8_t []){ 0x00, 0x04 } } ) )
      == PacketType::Acknowledgement );
  BOOST_CHECK(
    Packet::packetType( std::as_bytes( std::span< const uint8_t >{ (uint8_t []){ 0x00, 0x05 } } ) )
      == PacketType::Error );
  BOOST_CHECK(
    Packet::packetType( std::as_bytes( std::span< const uint8_t >{ (uint8_t[] ){0x00, 0x06 } } ) )
      == PacketType::OptionsAcknowledgement );

  BOOST_CHECK(
    Packet::packetType( std::as_bytes( std::span< const uint8_t >{ (uint8_t[] ){ 0x00, 0x00 } } ) )
      == PacketType::Invalid );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
