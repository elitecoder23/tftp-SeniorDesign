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

#include <tftp/packets/OptionsAcknowledgementPacket.hpp>

#include <helper/Dump.hpp>

#include <iostream>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpOptionsAcknowledgementPacket)

BOOST_AUTO_TEST_CASE( constructor )
{
  Tftp::Options::OptionList options{};

  using namespace std::literals::string_view_literals;
  options.set( "blocksize"sv, "4096"sv);

  OptionsAcknowledgementPacket oack{ options};

  RawTftpPacket raw{ oack};

  std::cout << "OACK:\n" << Dump( &(*raw.begin()), raw.size()) << std::endl;

  OptionsAcknowledgementPacket oack2( raw);

  BOOST_CHECK( oack.packetType()         == oack2.packetType());
  BOOST_CHECK( oack.option( "blocksize") == oack2.option( "blocksize"));
  BOOST_CHECK( oack.option( "XXX")       == "");
}

BOOST_AUTO_TEST_SUITE_END()

}
