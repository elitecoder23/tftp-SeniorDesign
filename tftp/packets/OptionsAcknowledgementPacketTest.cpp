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
 * @brief Definition of uint tests of TFTP packet classes
 **/

#include <boost/test/unit_test.hpp>

#include <tftp/packets/OptionsAcknowledgementPacket.hpp>

#include <helper/Dump.hpp>

#include <iostream>

namespace Tftp {
namespace Packets {

BOOST_AUTO_TEST_SUITE( TftpOptionsAcknowledgementPacket)

BOOST_AUTO_TEST_CASE( constructor )
{
  Tftp::Options::OptionList options;

  options.setOption( "blocksize", "4096");

  OptionsAcknowledgementPacket oack( options);

  RawTftpPacketType raw = oack.encode();

  std::cout << "OACK:\n" << Dump( &(*raw.begin()), raw.size()) << std::endl;

  OptionsAcknowledgementPacket oack2( raw);

  BOOST_CHECK( oack.getPacketType()         == oack2.getPacketType());
  BOOST_CHECK( oack.getOption( "blocksize") == oack2.getOption( "blocksize"));
  BOOST_CHECK( oack.getOption( "XXX")       == "");
}

BOOST_AUTO_TEST_SUITE_END()

}
}
