/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of unit tests of TFTP packet classes.
 **/

#include <tftp/packets/ErrorPacket.hpp>

#include <helper/Dump.hpp>

#include <boost/test/unit_test.hpp>

#include <iostream>

namespace Tftp {
namespace Packets {

BOOST_AUTO_TEST_SUITE( TftpErrorPacket)

BOOST_AUTO_TEST_CASE( constructor1)
{
  ErrorPacket error( ErrorCode::NotDefined, "ERROR MESSAGE");

  RawTftpPacketType raw( error);

  std::cout << Dump( &(*raw.begin()), raw.size());


  BOOST_CHECK( error.getPacketType() == PacketType::Error);
  BOOST_CHECK( error.getErrorCode() == ErrorCode::NotDefined);
  BOOST_CHECK( error.getErrorMessage() == "ERROR MESSAGE");
}

BOOST_AUTO_TEST_CASE( constructor2)
{
  RawTftpPacketType raw{
    0x00U, 0x05U,
    0x00U, 0x01U,
    'E', 'R', 'R', 'O', 'R', 0x00U
  };

  std::cout << Dump( &(*raw.begin()), raw.size());

  ErrorPacket error( raw);

  BOOST_CHECK( error.getPacketType() == PacketType::Error);
  BOOST_CHECK( error.getErrorCode() == ErrorCode::FileNotFound);
  BOOST_CHECK( error.getErrorMessage() == "ERROR");
}

BOOST_AUTO_TEST_SUITE_END()

}
}
