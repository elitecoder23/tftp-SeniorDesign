/*
 * $Date$
 * $Revision$
 */
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

namespace Tftp {
namespace Packets {

BOOST_AUTO_TEST_SUITE( AcknowledgementPacketTest)

BOOST_AUTO_TEST_CASE( constructor )
{
  AcknowledgementPacket ack( 10);
  BOOST_CHECK( PacketType::Acknowledgement == ack.getPacketType());
  BOOST_CHECK( 10 == ack.getBlockNumber());

  RawTftpPacketType raw = ack.encode();

  std::cout << Dump( &(*raw.begin()), raw.size());

  AcknowledgementPacket ack2( raw);

  BOOST_CHECK( ack.getPacketType()  == ack2.getPacketType());
  BOOST_CHECK( ack.getBlockNumber() == ack2.getBlockNumber());
}

BOOST_AUTO_TEST_SUITE_END()

}
}
