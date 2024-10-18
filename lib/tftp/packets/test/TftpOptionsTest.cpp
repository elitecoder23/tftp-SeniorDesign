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
 * @brief Implementation of Unit Tests of Tftp::Packets.
 **/

#include <tftp/packets/TftpOptions.hpp>

#include <tftp/packets/PacketException.hpp>

#include <boost/test/unit_test.hpp>

#include <cstdint>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpTest )
BOOST_AUTO_TEST_SUITE( PacketsTest )
BOOST_AUTO_TEST_SUITE( TftpOptionsTest )

//! TftpOptions_name tests
BOOST_AUTO_TEST_CASE( name )
{
  BOOST_CHECK( TftpOptions_name( KnownOptions::BlockSize ) == "blksize" );
  BOOST_CHECK( TftpOptions_name( KnownOptions::Timeout ) == "timeout" );
  BOOST_CHECK( TftpOptions_name( KnownOptions::TransferSize ) == "tsize" );
  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Test
  BOOST_CHECK( TftpOptions_name( KnownOptions{ 100 } ).empty() );
}

//! TftpOptions_toString tests
BOOST_AUTO_TEST_CASE( toString )
{
  TftpOptions options{};
  BOOST_CHECK_NO_THROW(
    boost::ignore_unused( TftpOptions_toString( options ) ) );

  options.transferSize=1000;
  BOOST_CHECK_NO_THROW(
    boost::ignore_unused( TftpOptions_toString( options ) ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
