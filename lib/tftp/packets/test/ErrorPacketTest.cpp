// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests of TFTP Packet Classes.
 **/

#include <tftp/packets/ErrorPacket.hpp>
#include <tftp/packets/PacketException.hpp>

#include <helper/Dump.hpp>

#include <boost/test/unit_test.hpp>

#include <array>
#include <iostream>

//! Raw Error Packet
auto rawError{ std::to_array< uint8_t >( {
  0x00U, 0x05U, // Opcode
  0x00U, 0x00U, // Error Code
  'E', 'R', 'R', 'O', 'R', ' ', 'M', 'E', 'S', 'S', 'A', 'G', 'E', 0x00U
} ) };

//! Raw Error Packet - empty error text
auto rawError2{ std::to_array< uint8_t >( {
  0x00U, 0x05U, // Opcode
  0x00U, 0x01U, // Error Code
  0x00U
} ) };

//! Raw Error Packet - Wrong Opcode
auto rawError3{ std::to_array< uint8_t >( {
  0x00U, 0x04U, // Opcode
  0x00U, 0x01U, // Error Code
  'E', 'R', 'R', 'O', 'R', 0x00U
} ) };

//! Raw Error Packet - too few data
auto rawError4{ std::to_array< uint8_t >( {
  0x00, 0x05,
  0x00, 0x00
} ) };

//! Raw Error Packet - too much data
auto rawError5{ std::to_array< uint8_t >( {
  0x00U, 0x05U,
  0x00U, 0x01U,
  'E', 'R', 'R', 'O', 'R', 0x00U, 0xFF
} ) };

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpTest )
BOOST_AUTO_TEST_SUITE( PacketsTest )
BOOST_AUTO_TEST_SUITE( TftpErrorPacket )

//! ErrorPacket Constructor Test
BOOST_AUTO_TEST_CASE( constructor1 )
{
  ErrorPacket error{ ErrorCode::NotDefined, "ERROR MESSAGE" };

  RawData raw{ error };

  std::cout << Helper::Dump( std::data( raw ), raw.size() );

  BOOST_CHECK( error.packetType() == PacketType::Error );
  BOOST_CHECK( error.errorCode() == ErrorCode::NotDefined );
  BOOST_CHECK( error.errorMessage() == "ERROR MESSAGE" );

  BOOST_CHECK( std::ranges::equal( raw, std::as_bytes( std::span< uint8_t >{ rawError } ) ) );
}

//! ErrorPacket Constructor Test
BOOST_AUTO_TEST_CASE( constructor2)
{
  using namespace std::literals;
  ErrorPacket error{ ErrorCode::NotDefined, "ERROR MESSAGE"s };

  RawData raw{ error };

  std::cout << Helper::Dump( std::data( raw ), raw.size() );

  BOOST_CHECK( error.packetType() == PacketType::Error );
  BOOST_CHECK( error.errorCode() == ErrorCode::NotDefined );
  BOOST_CHECK( error.errorMessage() == "ERROR MESSAGE" );

  BOOST_CHECK( std::ranges::equal( raw, std::as_bytes( std::span< uint8_t >{ rawError } ) ) );
}

//! ErrorPacket Constructor Test
BOOST_AUTO_TEST_CASE( constructor3 )
{
  ErrorPacket error2{ std::as_bytes( std::span{ rawError2 } ) };
  BOOST_CHECK( error2.packetType() == PacketType::Error );
  BOOST_CHECK( error2.errorCode() == ErrorCode::FileNotFound );
  BOOST_CHECK( error2.errorMessage().empty() );

  // Wrong Opcode
  BOOST_CHECK_THROW(
    ( ErrorPacket{ std::as_bytes( std::span< const uint8_t >{ rawError3 } ) } ),
    Tftp::Packets::InvalidPacketException );

  // too few data
  BOOST_CHECK_THROW(
    ( ErrorPacket{ std::as_bytes( std::span< const uint8_t >{ rawError4 } ) } ),
    Tftp::Packets::InvalidPacketException );

  // too much data
  BOOST_CHECK_THROW(
    ( ErrorPacket{ std::as_bytes( std::span< const uint8_t >{ rawError5 } ) } ),
    Tftp::Packets::InvalidPacketException );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
