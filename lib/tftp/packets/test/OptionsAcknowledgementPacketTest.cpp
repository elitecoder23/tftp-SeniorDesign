// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests of Class Tftp::Packets::TftpOptionsAcknowledgementPacket.
 **/

#include <boost/test/unit_test.hpp>

#include <tftp/packets/OptionsAcknowledgementPacket.hpp>
#include <tftp/packets/PacketException.hpp>

#include <helper/Dump.hpp>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpTest )
BOOST_AUTO_TEST_SUITE( PacketsTest )
BOOST_AUTO_TEST_SUITE( TftpOptionsAcknowledgementPacket )

//! Raw options acknowledgment packet
static const uint8_t rawOptionsAcknowledgementPacketOk[]{
  0x00U, 0x06U, // Opcode
  0x00U, 0x01U,
  'o', 'p', 't', '1', 0x00U, 'v', 'a', 'l', '1', 0x00U, 0x00U };

//! Raw options acknowledgment packet - invalid opcode
static const uint8_t rawOptionsAcknowledgementPacket1[]{
  0x00U, 0x05U, // Opcode
  0x00U, 0x01U,
  'o', 'p', 't', '1', 0x00U, 'v', 'a', 'l', '1', 0x00U, 0x00U };

//! Raw options acknowledgment packet - option string invalid 1
static const uint8_t rawOptionsAcknowledgementPacket2[]{
  0x00U, 0x06U, // Opcode
  0x00U, 0x01U,
  'o', 'p', 't', '1', 0x00U, 'v', 'a', 'l', '1', 0x00U };

//! Raw options acknowledgment packet - option string invalid 2
static const uint8_t rawOptionsAcknowledgementPacket3[]{
  0x00U, 0x06U, // Opcode
  0x00U, 0x01U,
  'o', 'p', 't', '1', 0x00U, 'v', 'a', 'l', '1', 0x00U, 0x00U, 0x00U };

//! Constructor Test
BOOST_AUTO_TEST_CASE( constructor )
{
  Options options{ { "blocksize", "4096" } };
  BOOST_REQUIRE( options.contains( "blocksize") );
  const auto blocksizeOption1{ options.find( "blocksize" ) };

  OptionsAcknowledgementPacket oack{ options };

  Helper::RawData raw{ oack };
  BOOST_TEST_MESSAGE( "OACK:\n" << Helper::Dump( std::data( raw ), raw.size() ) );

  OptionsAcknowledgementPacket oack2( raw );

  BOOST_CHECK( oack.packetType() == oack2.packetType() );

  auto options2{ oack.options() };
  BOOST_CHECK( options2.size() == 1 );
  BOOST_REQUIRE( options2.contains( "blocksize" ) );
  auto blocksizeOption2{ options2.find( "blocksize" ) };

  auto options3{ oack2.options() };
  BOOST_CHECK( options3.size() == 1 );
  BOOST_REQUIRE( options3.contains( "blocksize" ) );
  auto blocksizeOption3{ options3.find( "blocksize" ) };

  BOOST_CHECK( blocksizeOption1->first == blocksizeOption2->first );
  BOOST_CHECK( blocksizeOption1->first == blocksizeOption3->first );
  BOOST_CHECK( blocksizeOption1->second == blocksizeOption2->second );
  BOOST_CHECK( blocksizeOption1->second == blocksizeOption3->second );

  BOOST_CHECK( blocksizeOption1->second == "4096" );
  BOOST_CHECK( blocksizeOption2->second == "4096" );
  BOOST_CHECK( blocksizeOption3->second == "4096" );
}

//! Constructor Test
BOOST_AUTO_TEST_CASE( constructor2 )
{
  OptionsAcknowledgementPacket oack{ std::as_bytes( std::span{ rawOptionsAcknowledgementPacketOk } ) };
  BOOST_CHECK( oack.packetType() == PacketType::OptionsAcknowledgement );
  auto options{ oack.options() };
  BOOST_CHECK( options.size() == 2 );

  // Wrong Opcode
  BOOST_CHECK_THROW(
    OptionsAcknowledgementPacket{ std::as_bytes( std::span{ rawOptionsAcknowledgementPacket1 } ) },
    Tftp::Packets::InvalidPacketException );

  BOOST_CHECK_THROW(
    OptionsAcknowledgementPacket{ std::as_bytes( std::span{ rawOptionsAcknowledgementPacket2 } ) },
    Tftp::Packets::InvalidPacketException );

  BOOST_CHECK_THROW(
    OptionsAcknowledgementPacket{ std::as_bytes( std::span{ rawOptionsAcknowledgementPacket3 } ) },
    Tftp::Packets::InvalidPacketException );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
