/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests of class Tftp::Packets::ReadRequestPacket.
 **/

#include <boost/test/unit_test.hpp>

#include <tftp/packets/ReadRequestPacket.hpp>

#include <helper/Dump.hpp>

#include <iostream>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpReadRequestPacket)

BOOST_AUTO_TEST_CASE( constructor )
{
  Tftp::Options::OptionList options{};

  using namespace std::literals::string_view_literals;
  options.set( "blocksize"sv, "4096"sv);

  ReadRequestPacket rrq{ "testfile.bin", TransferMode::OCTET, options};

  RawTftpPacket raw{ rrq};

  std::cout << Dump( &(*raw.begin()), raw.size());

  ReadRequestPacket rrq2{ raw};

  BOOST_CHECK( rrq.packetType() == rrq2.packetType());
  BOOST_CHECK( rrq.filename() == rrq2.filename());
  BOOST_CHECK( rrq.mode() == rrq2.mode());
  BOOST_CHECK( rrq.option( "blocksize") == rrq2.option( "blocksize"));
  BOOST_CHECK( rrq.option( "XXX") == "");
}

BOOST_AUTO_TEST_SUITE_END()

}
