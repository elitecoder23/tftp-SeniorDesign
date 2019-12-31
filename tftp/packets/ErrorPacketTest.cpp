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

#include <tftp/packets/ErrorPacket.hpp>

#include <helper/Dump.hpp>

#include <boost/test/unit_test.hpp>

#include <iostream>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpTest)
BOOST_AUTO_TEST_SUITE( PacketsTest)
BOOST_AUTO_TEST_SUITE( TftpErrorPacket)

BOOST_AUTO_TEST_CASE( constructor1)
{
  ErrorPacket error{ ErrorCode::NotDefined, "ERROR MESSAGE"};

  RawTftpPacket raw{ error};

  std::cout << Dump( &(*raw.begin()), raw.size());


  BOOST_CHECK( error.packetType() == PacketType::Error);
  BOOST_CHECK( error.errorCode() == ErrorCode::NotDefined);
  BOOST_CHECK( error.errorMessage() == "ERROR MESSAGE");

  BOOST_CHECK((raw == RawTftpPacket{
    0x00U, 0x05U,
    0x00U, 0x00U,
    'E', 'R', 'R', 'O', 'R', ' ', 'M', 'E', 'S', 'S', 'A', 'G', 'E', 0x00U
  }));
}

BOOST_AUTO_TEST_CASE( constructor2)
{
  using namespace std::literals::string_literals;
  ErrorPacket error{ ErrorCode::NotDefined, "ERROR MESSAGE"s};

  RawTftpPacket raw{ error};

  std::cout << Dump( &(*raw.begin()), raw.size());


  BOOST_CHECK( error.packetType() == PacketType::Error);
  BOOST_CHECK( error.errorCode() == ErrorCode::NotDefined);
  BOOST_CHECK( error.errorMessage() == "ERROR MESSAGE");

  BOOST_CHECK((raw == RawTftpPacket{
    0x00U, 0x05U,
    0x00U, 0x00U,
    'E', 'R', 'R', 'O', 'R', ' ', 'M', 'E', 'S', 'S', 'A', 'G', 'E', 0x00U
  }));
}

BOOST_AUTO_TEST_CASE( constructor3)
{
  RawTftpPacket raw{
    0x00U, 0x05U,
    0x00U, 0x01U,
    'E', 'R', 'R', 'O', 'R', 0x00U
  };

  std::cout << Dump( &(*raw.begin()), raw.size());

  ErrorPacket error{ raw};

  BOOST_CHECK( error.packetType() == PacketType::Error);
  BOOST_CHECK( error.errorCode() == ErrorCode::FileNotFound);
  BOOST_CHECK( error.errorMessage() == "ERROR");
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
