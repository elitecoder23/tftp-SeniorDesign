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
 * @brief Definition of unit tests of class Tftp::Packets::ReadRequestPacket.
 **/

#include <boost/test/unit_test.hpp>

#include <tftp/packets/ReadRequestPacket.hpp>

#include <helper/Dump.hpp>

#include <iostream>

namespace Tftp {
namespace Packets {

BOOST_AUTO_TEST_SUITE( TftpReadRequestPacket)

BOOST_AUTO_TEST_CASE( constructor )
{
  Tftp::Options::OptionList options;

  options.setOption( "blocksize", "4096");

  ReadRequestPacket rrq( "testfile.bin", TransferMode::OCTET, options);

  RawTftpPacketType raw = rrq.encode();

  std::cout << Dump( &(*raw.begin()), raw.size());

  ReadRequestPacket rrq2( raw);

  BOOST_CHECK( rrq.getPacketType() == rrq2.getPacketType());
  BOOST_CHECK( rrq.getFilename() == rrq2.getFilename());
  BOOST_CHECK( rrq.getMode() == rrq2.getMode());
  BOOST_CHECK( rrq.getOption( "blocksize") == rrq2.getOption( "blocksize"));
  BOOST_CHECK( rrq.getOption( "XXX") == "");
}

BOOST_AUTO_TEST_SUITE_END()

}
}
