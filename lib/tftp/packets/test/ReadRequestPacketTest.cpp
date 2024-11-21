// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests of Class Tftp::Packets::ReadRequestPacket.
 **/

#include <boost/test/unit_test.hpp>

#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/PacketException.hpp>

#include <helper/Dump.hpp>

#include <iostream>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpTest )
BOOST_AUTO_TEST_SUITE( PacketsTest )
BOOST_AUTO_TEST_SUITE( TftpReadRequestPacket )

//! Constructor Test
BOOST_AUTO_TEST_CASE( constructor1 )
{
  Options options{ { "blocksize", "4096" } };
  BOOST_REQUIRE( options.contains( "blocksize") );
  auto blocksizeOption1{ options.find( "blocksize" ) };

  ReadRequestPacket rrq{ "testfile.bin", TransferMode::OCTET, options };

  RawTftpPacket raw{ rrq };
  std::cout << "RRQ:\n" << Helper::Dump( std::data( raw ), raw.size() ) << "\n";

  ReadRequestPacket rrq2{ raw };

  BOOST_CHECK( rrq.packetType() == rrq2.packetType() );
  BOOST_CHECK( rrq.filename() == rrq2.filename() );
  BOOST_CHECK( rrq.mode() == rrq2.mode() );

  auto options2{ rrq.options() };
  BOOST_CHECK( options2.size() == 1 );
  BOOST_REQUIRE( options2.contains( "blocksize" ) );
  auto blocksizeOption2{ options2.find( "blocksize" ) };

  auto options3{ rrq2.options() };
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
  ReadRequestPacket rrq1{ RawTftpPacket{
    0x00, 0x01,
    'f', 'i', 'l', 'e', 0x00,
    'o', 'c', 't', 'e', 't', 0x00 } };

  BOOST_CHECK( PacketType::ReadRequest == rrq1.packetType() );
  BOOST_CHECK( rrq1.filename() == "file" );
  BOOST_CHECK( TransferMode::OCTET == rrq1.mode() );
  BOOST_CHECK( rrq1.options().empty() );

  ReadRequestPacket rrq2{ RawTftpPacket{
    0x00, 0x01,
    'f', 'i', 'l', 'e', 0x00,
    'o', 'c', 't', 'e', 't', 0x00,
    'o', 'p', 't', '1', 0x00, 'v', 'a', 'l', '1', 0x00,
    'o', 'p', 't', '2', 0x00, 'v', 'a', 'l', '2', 0x00 } };

  BOOST_CHECK( PacketType::ReadRequest == rrq2.packetType() );
  BOOST_CHECK( rrq2.filename() == "file" );
  BOOST_CHECK( TransferMode::OCTET == rrq2.mode() );
  BOOST_CHECK( 2 == rrq2.options().size() );
  BOOST_CHECK( ( *( rrq2.options().begin() ) == Options::value_type{ "opt1", "val1" } ) );
  BOOST_CHECK( ( *( std::next( rrq2.options().begin() ) ) == Options::value_type{ "opt2", "val2" } ) );

  // Wrong Opcode
  BOOST_CHECK_THROW(
    ( ReadRequestPacket{ RawTftpPacket{
      0x00, 0x02,
      'f', 'i', 'l', 'e', 0x00,
      'o', 'c', 't', 'e', 't', 0x00 } } ),
    InvalidPacketException );

  // Wrong Length
  BOOST_CHECK_THROW(
    ( ReadRequestPacket{ RawTftpPacket{
      0x00, 0x01,
      'f', 'i', 'l', 'e', 0x00 } } ),
    InvalidPacketException );

  BOOST_CHECK_THROW(
    ( ReadRequestPacket{ RawTftpPacket{
      0x00, 0x01,
      'f', 'i', 'l', 'e', 0x00,
      'o', 'c', 't', 'e', 't', 0x00,
      'o', 'p', 't', '1', 0x00, 'v', 'a', 'l', '1', 0x00,
      'o', 'p', 't', '2', 0x00 } } ),
    InvalidPacketException );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
