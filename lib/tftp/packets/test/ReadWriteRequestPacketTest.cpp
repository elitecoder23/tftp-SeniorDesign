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
 * @brief Unit Tests of class Tftp::Packets::TftpReadWriteRequestPacket
 **/

#include <boost/test/unit_test.hpp>

#include <tftp/packets/ReadWriteRequestPacket.hpp>

#include <helper/Dump.hpp>

#include <iostream>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpTest)
BOOST_AUTO_TEST_SUITE( PacketsTest)
BOOST_AUTO_TEST_SUITE( TftpReadWriteRequestPacketTest)

//! Decode string test
BOOST_AUTO_TEST_CASE( decodeModeString)
{
  BOOST_CHECK( ReadWriteRequestPacket::decodeMode( "") == TransferMode::Invalid);
  BOOST_CHECK( ReadWriteRequestPacket::decodeMode( "XX") == TransferMode::Invalid);
  BOOST_CHECK( ReadWriteRequestPacket::decodeMode( "OCTET") == TransferMode::OCTET);
  BOOST_CHECK( ReadWriteRequestPacket::decodeMode( "NETASCII") == TransferMode::NETASCII);
  BOOST_CHECK( ReadWriteRequestPacket::decodeMode( "MAIL") == TransferMode::MAIL);
}

//! Decode enumeration test
BOOST_AUTO_TEST_CASE( decodeModeEnum)
{
  BOOST_CHECK( ReadWriteRequestPacket::decodeMode( TransferMode::Invalid) == std::string_view{});
  BOOST_CHECK( ReadWriteRequestPacket::decodeMode( TransferMode::OCTET) == "OCTET");
  BOOST_CHECK( ReadWriteRequestPacket::decodeMode( TransferMode::NETASCII) == "NETASCII");
  BOOST_CHECK( ReadWriteRequestPacket::decodeMode( TransferMode::MAIL) == "MAIL");
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
