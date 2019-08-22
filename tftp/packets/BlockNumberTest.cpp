/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of unit tests of class Tftp::Packets::BlockNumber
 **/

#include <boost/test/unit_test.hpp>

#include <tftp/packets/BlockNumber.hpp>

namespace Tftp::Packets {

BOOST_AUTO_TEST_SUITE( TftpBlockNumber)

//! Constructor test
BOOST_AUTO_TEST_CASE( constructor1 )
{
  BOOST_CHECK( 0 == BlockNumber());
}

//! Constructor test
BOOST_AUTO_TEST_CASE( constructor2 )
{
  BOOST_CHECK( 1 == BlockNumber( 1));
  BOOST_CHECK( 0xFFFFU == BlockNumber( 0xFFFFU));
}

//! increment test
BOOST_AUTO_TEST_CASE( increment )
{
  BlockNumber blockNumber;

  BOOST_CHECK( 0 == blockNumber);

  BOOST_CHECK( 1 == blockNumber.next());
  BOOST_CHECK( 0 == blockNumber++);
  BOOST_CHECK( 1 == blockNumber);

  BOOST_CHECK( 2 == ++blockNumber);
  BOOST_CHECK( 2 == blockNumber);

  blockNumber = 0xFFFF;
  BOOST_CHECK( 0xFFFF == blockNumber);
  BOOST_CHECK( 1 == blockNumber.next());
}

//! decrement test
BOOST_AUTO_TEST_CASE( decrement )
{
  BlockNumber blockNumber( 10);

  BOOST_CHECK( 10 == blockNumber);

  BOOST_CHECK( 9 == blockNumber.previous());
  BOOST_CHECK( 10 == blockNumber--);
  BOOST_CHECK( 9 == blockNumber);

  BOOST_CHECK( 8 == --blockNumber);
  BOOST_CHECK( 8 == blockNumber);

  blockNumber = 1;
  BOOST_CHECK( 1 == blockNumber);
  BOOST_CHECK( 0xFFFF == blockNumber.previous());
}

//! Comparison test
BOOST_AUTO_TEST_CASE( compare )
{
  BlockNumber blockNumber5a( 5);
  BlockNumber blockNumber5b( 5);
  BlockNumber blockNumber6( 6);

  const uint16_t int5 = 5;
  const uint16_t int6 = 6;

  BOOST_CHECK( blockNumber5a != blockNumber6);
  BOOST_CHECK( !(blockNumber5a == blockNumber6));

  BOOST_CHECK( blockNumber5a != int6);
  BOOST_CHECK( !(blockNumber5a == int6));

  BOOST_CHECK( blockNumber5a == blockNumber5b);
  BOOST_CHECK( !(blockNumber5a != blockNumber5b));

  BOOST_CHECK( blockNumber5a == int5);
  BOOST_CHECK( !(blockNumber5a != int5));
}

BOOST_AUTO_TEST_SUITE_END()

}
