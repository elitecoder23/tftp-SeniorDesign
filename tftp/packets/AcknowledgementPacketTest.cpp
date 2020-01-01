/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

BOOST_AUTO_TEST_SUITE( TftpTest)
BOOST_AUTO_TEST_SUITE( PacketsTest)
BOOST_AUTO_TEST_SUITE( AcknowledgementPacketTest)

//! Constructor Test
BOOST_AUTO_TEST_CASE( constructor1)
{
  AcknowledgementPacket ack{ BlockNumber{ 10U}};

  RawTftpPacket raw{ ack};

  std::cout << Helper::Dump( &(*raw.begin()), raw.size());

  BOOST_CHECK( ack.packetType() == PacketType::Acknowledgement);
  BOOST_CHECK( ack.blockNumber() == BlockNumber{ 10U});

  ack.blockNumber( BlockNumber{ 25U});

  BOOST_CHECK( ack.packetType() == PacketType::Acknowledgement);
  BOOST_CHECK( ack.blockNumber() == BlockNumber{ 25U});
}

//! Constructor Test
BOOST_AUTO_TEST_CASE( constructor2)
{
  RawTftpPacket raw{
    // Opcode
    0x00, 0x04,
    0x10, 0x01
  };

  AcknowledgementPacket ack{ raw};
  std::cout << Helper::Dump( &(*raw.begin()), raw.size());

  BOOST_CHECK( ack.packetType() == PacketType::Acknowledgement);
  BOOST_CHECK( ack.blockNumber() == BlockNumber( 4097U));

  BOOST_CHECK_THROW(
    (AcknowledgementPacket{ RawTftpPacket{0x00, 0x04}}),
    Tftp::Packets::InvalidPacketException);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
