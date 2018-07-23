/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of uint tests of TFTP packet classes
 **/

#include <boost/test/unit_test.hpp>

#include <tftp/packets/WriteRequestPacket.hpp>

#include <helper/Dump.hpp>

#include <iostream>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpWriteRequestPacket)

//! Constructor test
BOOST_AUTO_TEST_CASE( constructor )
{
  Options::OptionList options;

  options.set( "blocksize", "4096");

  WriteRequestPacket wrq( "testfile.bin", TransferMode::OCTET, options);

  RawTftpPacket raw( wrq);

  std::cout << Dump( &(*raw.begin()), raw.size());

  WriteRequestPacket wrq2( raw);

  BOOST_CHECK( wrq.packetType() == wrq2.packetType());
  BOOST_CHECK( wrq.filename() == wrq2.filename());
  BOOST_CHECK( wrq.mode() == wrq2.mode());
  BOOST_CHECK( wrq.option( "blocksize") == wrq2.option( "blocksize"));
  BOOST_CHECK( wrq.option( "XXX") == "");
}

BOOST_AUTO_TEST_SUITE_END()

}
