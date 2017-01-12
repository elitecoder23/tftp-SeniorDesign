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

#include <tftp/packet/ErrorPacket.hpp>

#include <helper/Dump.hpp>
#include <helper/Logger.hpp>

#include <boost/test/unit_test.hpp>

#include <iostream>

namespace Tftp {
namespace Packet {

BOOST_AUTO_TEST_SUITE( TftpErrorPacket)

BOOST_AUTO_TEST_CASE( constructor )
{
  ErrorPacket error( ErrorCode::NOT_DEFINED, "ERROR MESSAGE");

  RawTftpPacketType raw = error.encode();

  std::cout << Dump( &(*raw.begin()), raw.size());

  ErrorPacket error2( raw);

  BOOST_CHECK( error.getPacketType() == error2.getPacketType());
  BOOST_CHECK( error.getErrorCode() == error2.getErrorCode());
  BOOST_CHECK( error.getErrorMessage() == error2.getErrorMessage());
}

BOOST_AUTO_TEST_SUITE_END()

}
}
