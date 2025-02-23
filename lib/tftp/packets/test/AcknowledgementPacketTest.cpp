// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests of TFTP Packet Classes.
 **/

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/PacketException.hpp>

#include <helper/Dump.hpp>

#include <boost/test/unit_test.hpp>

#include <iostream>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpTest )
BOOST_AUTO_TEST_SUITE( PacketsTest )
BOOST_AUTO_TEST_SUITE( AcknowledgementPacketTest )

//! Raw Acknowledgment Packet
static const uint8_t rawAckPacket[]{
  0x00, 0x04, // Opcode
  0x10, 0x01  // block number
};

//! Raw Acknowledgment Packet - too few data
static const uint8_t rawAckPacketInv1[]{
  0x00, 0x04, // Opcode
};

//! Raw Acknowledgment Packet - too much data
static const uint8_t rawAckPacketInv2[]{
  0x00, 0x04, // Opcode
  0x10, 0x01,  // block number
  0xff };

//! Raw Acknowledgment Packet - invalid opcode
static const uint8_t rawAckPacketInv3[]
{
  0x00, 0x03, // Opcode
  0x10, 0x01 };

//! Constructor Test
BOOST_AUTO_TEST_CASE( constructor1 )
{
  AcknowledgementPacket ack{ BlockNumber{ 10U } };

  Helper::RawData raw{ ack };

  std::cout << Helper::Dump( std::data( raw ), raw.size() );

  BOOST_CHECK( ack.packetType() == PacketType::Acknowledgement );
  BOOST_CHECK( ack.blockNumber() == BlockNumber{ 10U } );

  ack.blockNumber( BlockNumber{ 25U } );

  BOOST_CHECK( ack.packetType() == PacketType::Acknowledgement );
  BOOST_CHECK( ack.blockNumber() == BlockNumber{ 25U } );
}

//! Constructor Test - Raw decoding
BOOST_AUTO_TEST_CASE( constructor2 )
{
  AcknowledgementPacket ack{ std::as_bytes( std::span{ rawAckPacket } ) };
  std::cout << Helper::Dump( std::data( rawAckPacket ), std::size( rawAckPacket ) );

  BOOST_CHECK( ack.packetType() == PacketType::Acknowledgement );
  BOOST_CHECK( ack.blockNumber() == BlockNumber( 0x1001U ) );

  // too few data
  BOOST_CHECK_THROW(
    AcknowledgementPacket{ std::as_bytes( std::span{ rawAckPacketInv1 } ) },
    Tftp::Packets::InvalidPacketException );

  // too much data
  BOOST_CHECK_THROW(
    AcknowledgementPacket{ std::as_bytes( std::span{ rawAckPacketInv2 } ) },
    Tftp::Packets::InvalidPacketException );

  // Data Packet
  BOOST_CHECK_THROW(
    AcknowledgementPacket{ std::as_bytes( std::span{ rawAckPacketInv3 } ) },
    Tftp::Packets::InvalidPacketException );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
