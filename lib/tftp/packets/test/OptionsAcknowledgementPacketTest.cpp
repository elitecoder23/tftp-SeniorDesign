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
 * @brief Definition of Unit Tests of Class
 *   Tftp::Packets::TftpOptionsAcknowledgementPacket.
 **/

#include <boost/test/unit_test.hpp>

#include "tftp/packets/OptionsAcknowledgementPacket.hpp"
#include "tftp/packets/PacketException.hpp"

#include "helper/Dump.hpp"

#include <iostream>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpTest)
BOOST_AUTO_TEST_SUITE( PacketsTest)
BOOST_AUTO_TEST_SUITE( TftpOptionsAcknowledgementPacket)

//! Constructor Test
BOOST_AUTO_TEST_CASE( constructor )
{
  Options options{  {"blocksize", "4096"} };
  OptionsAcknowledgementPacket oack{ options};

  RawTftpPacket raw{ oack};

  std::cout << "OACK:\n" << Helper::Dump( std::data( raw ), raw.size() ) << "\n";

  OptionsAcknowledgementPacket oack2( raw );

  BOOST_CHECK( oack.packetType()         == oack2.packetType() );

  auto options2{ oack.options() };
  BOOST_CHECK( options2.size() == 1);
  auto blocksizeOption1{ options.find( "blocksize" ) };
  BOOST_CHECK( blocksizeOption1 != options2.end() );

  auto options3{ oack2.options() };
  BOOST_CHECK( options3.size() == 1 );
  auto blocksizeOption2{ options.find( "blocksize" ) };
  BOOST_CHECK( blocksizeOption2 != options3.end() );

  BOOST_CHECK( blocksizeOption1->first == blocksizeOption2->first );
  BOOST_CHECK( blocksizeOption1->second == blocksizeOption2->second );
  BOOST_CHECK( blocksizeOption1->second == "4096" );
  BOOST_CHECK( blocksizeOption2->second == "4096" );
}

//! Constructor Test
BOOST_AUTO_TEST_CASE( constructor2 )
{
  // Wrong Opcode
  BOOST_CHECK_THROW(
    ( OptionsAcknowledgementPacket{ RawTftpPacket{
      0x00U, 0x05U,
      0x00U, 0x01U,
      'o', 'p', 't', '1', 0x00U, 'v', 'a', 'l', '1', 0x00U, 0x00U } } ),
    Tftp::Packets::InvalidPacketException );

  BOOST_CHECK_THROW(
    ( OptionsAcknowledgementPacket{ RawTftpPacket{
      0x00U, 0x06U,
      0x00U, 0x01U,
      'o', 'p', 't', '1', 0x00U, 'v', 'a', 'l', '1', 0x00U } } ),
    Tftp::Packets::InvalidPacketException );
  BOOST_CHECK_THROW(
    ( OptionsAcknowledgementPacket{ RawTftpPacket{
      0x00U, 0x06U,
      0x00U, 0x01U,
      'o', 'p', 't', '1', 0x00U, 'v', 'a', 'l', '1', 0x00U, 0x00U, 0x00U } } ),
    Tftp::Packets::InvalidPacketException );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
