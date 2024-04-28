// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests of Class Tftp::Packets::WriteRequestPacket.
 **/

#include <boost/test/unit_test.hpp>

#include "tftp/packets/WriteRequestPacket.hpp"
#include "tftp/packets/PacketException.hpp"

#include "helper/Dump.hpp"

#include <iostream>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpTest)
BOOST_AUTO_TEST_SUITE( PacketsTest)
BOOST_AUTO_TEST_SUITE( TftpWriteRequestPacket)

//! Constructor test
BOOST_AUTO_TEST_CASE( constructor1 )
{
  Options options{ { "blocksize", "4096" } };
  WriteRequestPacket wrq{ "testfile.bin", TransferMode::OCTET, options };

  RawTftpPacket raw{ wrq };

  std::cout << Helper::Dump( &(*raw.begin()), raw.size());

  WriteRequestPacket wrq2{ raw };

  BOOST_CHECK( wrq.packetType() == wrq2.packetType() );
  BOOST_CHECK( wrq.filename() == wrq2.filename() );
  BOOST_CHECK( wrq.mode() == wrq2.mode() );

  auto options2{ wrq.options() };
  BOOST_CHECK( options2.size() == 1);
  auto blocksizeOption1{ options.find( "blocksize")};
  BOOST_CHECK( blocksizeOption1 != options2.end());

  auto options3{ wrq2.options()};
  BOOST_CHECK( options3.size() == 1);
  auto blocksizeOption2{ options.find( "blocksize")};
  BOOST_CHECK( blocksizeOption2 != options3.end());

  BOOST_CHECK( blocksizeOption1->first == blocksizeOption2->first);
  BOOST_CHECK( blocksizeOption1->second == blocksizeOption2->second);
  BOOST_CHECK( blocksizeOption1->second == "4096");
  BOOST_CHECK( blocksizeOption2->second == "4096");
}

BOOST_AUTO_TEST_CASE( constructor2 )
{
  WriteRequestPacket wrq1{ RawTftpPacket{
    0x00, 0x02,
    'f', 'i', 'l', 'e', 0x00,
    'o', 'c', 't', 'e', 't', 0x00 } };

  BOOST_CHECK( PacketType::WriteRequest == wrq1.packetType() );
  BOOST_CHECK( wrq1.filename() == "file" );
  BOOST_CHECK( TransferMode::OCTET == wrq1.mode() );
  BOOST_CHECK( wrq1.options().empty() );

  WriteRequestPacket wrq2{ RawTftpPacket{
    0x00, 0x02,
    'f', 'i', 'l', 'e', 0x00,
    'o', 'c', 't', 'e', 't', 0x00,
    'o', 'p', 't', '1', 0x00, 'v', 'a', 'l', '1', 0x00,
    'o', 'p', 't', '2', 0x00, 'v', 'a', 'l', '2', 0x00 } };

  BOOST_CHECK( PacketType::WriteRequest == wrq2.packetType() );
  BOOST_CHECK( wrq2.filename() == "file" );
  BOOST_CHECK( TransferMode::OCTET == wrq2.mode() );
  BOOST_CHECK( 2 == wrq2.options().size() );
  BOOST_CHECK( ( *( wrq2.options().begin() ) == Options::value_type{ "opt1", "val1" } ) );
  BOOST_CHECK( ( *( std::next( wrq2.options().begin() ) ) == Options::value_type{ "opt2", "val2" } ) );

  // Wrong Opcode
  BOOST_CHECK_THROW(
    ( WriteRequestPacket{ RawTftpPacket{
      0x00, 0x01,
      'f', 'i', 'l', 'e', 0x00,
      'o', 'c', 't', 'e', 't', 0x00 } } ),
    InvalidPacketException );

  // Wrong Length
  BOOST_CHECK_THROW(
    ( WriteRequestPacket{ RawTftpPacket{
      0x00, 0x02,
      'f', 'i', 'l', 'e', 0x00 } } ),
    InvalidPacketException );

  BOOST_CHECK_THROW(
    ( WriteRequestPacket{ RawTftpPacket{
      0x00, 0x02,
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
