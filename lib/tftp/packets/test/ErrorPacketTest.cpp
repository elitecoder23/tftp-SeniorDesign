// SPDX-License-Identifier: MPL-2.0
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

#include "tftp/packets/ErrorPacket.hpp"
#include "tftp/packets/PacketException.hpp"

#include "helper/Dump.hpp"

#include <boost/test/unit_test.hpp>

#include <iostream>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpTest)
BOOST_AUTO_TEST_SUITE( PacketsTest)
BOOST_AUTO_TEST_SUITE( TftpErrorPacket)

//! ErrorPacket Constructor Test
BOOST_AUTO_TEST_CASE( constructor1)
{
  ErrorPacket error{ ErrorCode::NotDefined, "ERROR MESSAGE" };

  RawTftpPacket raw{ error };

  std::cout << Helper::Dump( std::data( raw ), raw.size() );


  BOOST_CHECK( error.packetType() == PacketType::Error );
  BOOST_CHECK( error.errorCode() == ErrorCode::NotDefined );
  BOOST_CHECK( error.errorMessage() == "ERROR MESSAGE" );

  BOOST_CHECK((raw == RawTftpPacket{
    0x00U, 0x05U,
    0x00U, 0x00U,
    'E', 'R', 'R', 'O', 'R', ' ', 'M', 'E', 'S', 'S', 'A', 'G', 'E', 0x00U
  } ) );
}

//! ErrorPacket Constructor Test
BOOST_AUTO_TEST_CASE( constructor2)
{
  using namespace std::literals;
  ErrorPacket error{ ErrorCode::NotDefined, "ERROR MESSAGE"s};

  RawTftpPacket raw{ error };

  std::cout << Helper::Dump( std::data( raw ), raw.size() );


  BOOST_CHECK( error.packetType() == PacketType::Error);
  BOOST_CHECK( error.errorCode() == ErrorCode::NotDefined);
  BOOST_CHECK( error.errorMessage() == "ERROR MESSAGE");

  BOOST_CHECK( (raw == RawTftpPacket{
    0x00U, 0x05U,
    0x00U, 0x00U,
    'E', 'R', 'R', 'O', 'R', ' ', 'M', 'E', 'S', 'S', 'A', 'G', 'E', 0x00U
  } ) );
}

//! ErrorPacket Constructor Test
BOOST_AUTO_TEST_CASE( constructor3 )
{
  RawTftpPacket raw{
    0x00U, 0x05U,
    0x00U, 0x01U,
    'E', 'R', 'R', 'O', 'R', 0x00U
  };

  std::cout << Helper::Dump( std::data( raw ), raw.size() );

  ErrorPacket error{ raw};

  BOOST_CHECK( error.packetType() == PacketType::Error);
  BOOST_CHECK( error.errorCode() == ErrorCode::FileNotFound);
  BOOST_CHECK( error.errorMessage() == "ERROR");

  ErrorPacket error2{ RawTftpPacket{
    0x00U, 0x05U,
    0x00U, 0x01U,
    0x00U
  } };
  BOOST_CHECK( error2.packetType() == PacketType::Error);
  BOOST_CHECK( error2.errorCode() == ErrorCode::FileNotFound );
  BOOST_CHECK( error2.errorMessage().empty() );

  // Wrong Opcode
  BOOST_CHECK_THROW(
    ( ErrorPacket{ RawTftpPacket{
      0x00U, 0x04U,
      0x00U, 0x01U,
      'E', 'R', 'R', 'O', 'R', 0x00U } } ),
    Tftp::Packets::InvalidPacketException );

  // too few data
  BOOST_CHECK_THROW(
    ( ErrorPacket{ RawTftpPacket{ 0x00, 0x05, 0x00, 0x00 } } ),
    Tftp::Packets::InvalidPacketException );

  // too much data
  BOOST_CHECK_THROW(
    ( ErrorPacket{ RawTftpPacket{
      0x00U, 0x05U,
      0x00U, 0x01U,
      'E', 'R', 'R', 'O', 'R', 0x00U, 0xFF } } ),
    Tftp::Packets::InvalidPacketException );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
