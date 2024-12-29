// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests of class Tftp::Packets::DataPacket.
 **/

#include <boost/test/unit_test.hpp>

#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/PacketException.hpp>

#include <helper/Dump.hpp>
#include <helper/Endianess.hpp>

#include <iostream>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpTest )
BOOST_AUTO_TEST_SUITE( PacketsTest )
BOOST_AUTO_TEST_SUITE( DataPacketTest )

//! Raw Dta Packet
static const uint8_t rawDataPacket[]{
  // Opcode
  0x00, 0x03,
  // block number
  0x01, 0x02,
  // data
  'D', 'A', 'T', 'A', '_', 'T', 'E', 'S', 'T'
};

//! invalid Opcode
static const uint8_t rawDataPacket2[]{
  // Opcode
  0x00, 0x04,
  // block number
  0x01, 0x02,
  // data
  'D', 'A', 'T', 'A', '_', 'T', 'E', 'S', 'T'
};

//! Constructor test
BOOST_AUTO_TEST_CASE( constructor1 )
{
  using Helper::operator ""_b;

  DataPacket dp1;

  BOOST_CHECK( dp1.packetType() == PacketType::Data );
  BOOST_CHECK( dp1.blockNumber() == BlockNumber() );
  BOOST_CHECK( dp1.dataSize() == 0 );
  BOOST_CHECK( dp1.data().empty() );

  DataPacket dp2( BlockNumber(), { 'H'_b, 'E'_b, 'L'_b, 'L'_b, 'O'_b } );
  BOOST_CHECK( dp2.packetType() == PacketType::Data);
  BOOST_CHECK( dp2.blockNumber() == BlockNumber());
  BOOST_CHECK( dp2.dataSize() == 5);
  BOOST_CHECK( !dp2.data().empty() );
  BOOST_CHECK( std::ranges::equal( dp2.data(), ( std::byte[] ){ 'H'_b, 'E'_b, 'L'_b, 'L'_b, 'O'_b } ) );
  BOOST_CHECK(
    std::ranges::equal(
      ( const_cast< const DataPacket & >( dp2 ).data() ),
      (std::byte []){ 'H'_b, 'E'_b, 'L'_b, 'L'_b, 'O'_b } ) );

  DataPacket dp3( BlockNumber{55}, { 'H'_b, 'E'_b, 'L'_b, 'L'_b, 'O'_b } );
  BOOST_CHECK( dp3.packetType() == PacketType::Data );
  BOOST_CHECK( dp3.blockNumber() == BlockNumber(55 ) );
  BOOST_CHECK( dp3.dataSize() == 5 );
  BOOST_CHECK( !dp3.data().empty() );

  DataPacket data(
    BlockNumber{ 10 },
    {
      'H'_b,
      'E'_b,
      'L'_b,
      'L'_b,
      'O'_b,
      ' '_b,
      'W'_b,
      'O'_b,
      'R'_b,
      'L'_b,
      'D'_b,
      '!'_b
    });

  RawData raw( data );

  std::cout << Helper::Dump( &(*raw.begin()), raw.size());
  std::cout << static_cast< std::string>( data) << "\n";

  DataPacket data2( raw );

  BOOST_CHECK( data.packetType()  == data2.packetType() );
  BOOST_CHECK( data.blockNumber() == data2.blockNumber() );
  BOOST_CHECK( data.dataSize()    == data2.dataSize() );
  BOOST_CHECK( data.data()        == data2.data() );
}

//! Constructor test
BOOST_AUTO_TEST_CASE( constructor2 )
{
  using Helper::operator ""_b;

  DataPacket dataPacket{ std::as_bytes( std::span( rawDataPacket ) ) };

  BOOST_CHECK( dataPacket.packetType()  == PacketType::Data);
  BOOST_CHECK( dataPacket.blockNumber() == BlockNumber{ 0x0102});
  BOOST_CHECK( dataPacket.dataSize()    == 9);
  BOOST_CHECK( (dataPacket.data()       == DataPacket::Data{ 'D'_b, 'A'_b, 'T'_b, 'A'_b, '_'_b, 'T'_b, 'E'_b, 'S'_b, 'T'_b } ) );

  // invalid opcode
  BOOST_CHECK_THROW( DataPacket{ std::as_bytes( std::span{ rawDataPacket2 } ) }, Packets::InvalidPacketException );
}

//! set block number test
BOOST_AUTO_TEST_CASE( setBlockNumber )
{
  DataPacket dp1;
  const DataPacket &dp1Const( dp1);

  BOOST_CHECK( dp1.blockNumber() == BlockNumber());
  BOOST_CHECK( dp1Const.blockNumber() == BlockNumber());

  dp1.blockNumber( BlockNumber{10});
  BOOST_CHECK( dp1.blockNumber() == BlockNumber{ 10});
  BOOST_CHECK( dp1Const.blockNumber() == BlockNumber{ 10});

  dp1.blockNumber()++;
  BOOST_CHECK( dp1.blockNumber() == BlockNumber{ 11});
  BOOST_CHECK( dp1Const.blockNumber() == BlockNumber{ 11});
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
