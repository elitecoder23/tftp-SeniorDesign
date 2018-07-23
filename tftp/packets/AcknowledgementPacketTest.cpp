/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of unit tests of TFTP packet classes
 **/

#include <tftp/packets/AcknowledgementPacket.hpp>

#include <helper/Dump.hpp>

#include <boost/test/unit_test.hpp>

#include <iostream>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( AcknowledgementPacketTest)

//! Constructor test
BOOST_AUTO_TEST_CASE( constructor1)
{
  AcknowledgementPacket ack( BlockNumber{ 10U});

  RawTftpPacket raw( ack);

  std::cout << Dump( &(*raw.begin()), raw.size());

  BOOST_CHECK( ack.packetType() == PacketType::Acknowledgement);
  BOOST_CHECK( ack.blockNumber() == BlockNumber{ 10U});
}

//! Constructor test
BOOST_AUTO_TEST_CASE( constructor2)
{
  RawTftpPacket raw{
    0x00, 0x04,
    0x10, 0x01
  };

  AcknowledgementPacket ack( raw);
  std::cout << Dump( &(*raw.begin()), raw.size());

  BOOST_CHECK( ack.packetType() == PacketType::Acknowledgement);
  BOOST_CHECK( ack.blockNumber() == BlockNumber( 4097U));
}

BOOST_AUTO_TEST_SUITE_END()

}
