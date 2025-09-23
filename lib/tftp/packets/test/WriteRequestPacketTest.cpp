// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests of Class Tftp::Packets::WriteRequestPacket.
 **/

#include <boost/test/unit_test.hpp>

#include <tftp/packets/WriteRequestPacket.hpp>
#include <tftp/packets/PacketException.hpp>

#include <helper/Dump.hpp>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpTest )
BOOST_AUTO_TEST_SUITE( PacketsTest )
BOOST_AUTO_TEST_SUITE( TftpWriteRequestPacket )

//! Raw Write Request Packet w.o. options
static const uint8_t rawWritePacket1[]{
  0x00, 0x02,
  'f', 'i', 'l', 'e', 0x00,
  'o', 'c', 't', 'e', 't', 0x00 };

//! Raw Write Request Packet with options
static const uint8_t rawWritePacket2[]{
  0x00, 0x02,
  'f', 'i', 'l', 'e', 0x00,
  'o', 'c', 't', 'e', 't', 0x00,
  'o', 'p', 't', '1', 0x00, 'v', 'a', 'l', '1', 0x00,
  'o', 'p', 't', '2', 0x00, 'v', 'a', 'l', '2', 0x00 };

//! Raw Write Request Packet - Wrong Opcode
static const uint8_t rawWritePacketInv1[]{
  0x00, 0x01,
  'f', 'i', 'l', 'e', 0x00,
  'o', 'c', 't', 'e', 't', 0x00 };

//! Raw Write Request Packet - Wrong Length
static const uint8_t rawWritePacketInv2[]{
    0x00, 0x02,
    'f', 'i', 'l', 'e', 0x00 };

//! Raw Write Request Packet - Invalid Options
static const uint8_t rawWritePacketInv3[]{
  0x00, 0x02,
  'f', 'i', 'l', 'e', 0x00,
  'o', 'c', 't', 'e', 't', 0x00,
  'o', 'p', 't', '1', 0x00, 'v', 'a', 'l', '1', 0x00,
  'o', 'p', 't', '2', 0x00 };

//! Constructor Test
BOOST_AUTO_TEST_CASE( constructor1 )
{
  Options options{ { "blocksize", "4096" } };
  BOOST_REQUIRE( options.contains( "blocksize") );
  auto blocksizeOption1{ options.find( "blocksize" ) };

  WriteRequestPacket wrq{ "testfile.bin", TransferMode::OCTET, options };

  Helper::RawData raw{ wrq };
  BOOST_TEST_MESSAGE( "RRQ:\n" << Helper::Dump( std::data( raw ), raw.size() ) );

  WriteRequestPacket wrq2{ raw };

  BOOST_CHECK( wrq.packetType() == wrq2.packetType() );
  BOOST_CHECK( wrq.filename() == wrq2.filename() );
  BOOST_CHECK( wrq.mode() == wrq2.mode() );

  auto options2{ wrq.options() };
  BOOST_CHECK( options2.size() == 1 );
  BOOST_REQUIRE( options2.contains( "blocksize" ) );
  auto blocksizeOption2{ options2.find( "blocksize" ) };

  auto options3{ wrq2.options() };
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
  WriteRequestPacket wrq1{ std::as_bytes( std::span{ rawWritePacket1 } ) };

  BOOST_CHECK( PacketType::WriteRequest == wrq1.packetType() );
  BOOST_CHECK( wrq1.filename() == "file" );
  BOOST_CHECK( TransferMode::OCTET == wrq1.mode() );
  BOOST_CHECK( wrq1.options().empty() );

  WriteRequestPacket wrq2{ std::as_bytes( std::span{ rawWritePacket2 } ) };

  BOOST_CHECK( PacketType::WriteRequest == wrq2.packetType() );
  BOOST_CHECK( wrq2.filename() == "file" );
  BOOST_CHECK( TransferMode::OCTET == wrq2.mode() );
  BOOST_CHECK( 2 == wrq2.options().size() );
  BOOST_CHECK( ( *( wrq2.options().begin() ) == Options::value_type{ "opt1", "val1" } ) );
  BOOST_CHECK( ( *( std::next( wrq2.options().begin() ) ) == Options::value_type{ "opt2", "val2" } ) );

  // Wrong Opcode
  BOOST_CHECK_THROW( WriteRequestPacket{ std::as_bytes( std::span{ rawWritePacketInv1 } ) }, InvalidPacketException );

  // Wrong Length
  BOOST_CHECK_THROW( WriteRequestPacket{ std::as_bytes( std::span{ rawWritePacketInv2 } ) }, InvalidPacketException );

  // invalid option string
  BOOST_CHECK_THROW( WriteRequestPacket{ std::as_bytes( std::span{ rawWritePacketInv3 } ) }, InvalidPacketException );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
