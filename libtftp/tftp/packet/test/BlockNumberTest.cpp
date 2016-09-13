/*
 * $Date$
 * $Revision$
 */
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of unit tests of class TFTP::Packet::BlockNumber
 **/

#include <tftp/packet/BlockNumber.hpp>

#include <helper/Logger.hpp>

#include <boost/test/unit_test.hpp>

using namespace Tftp::Packet;

static void testCtor( void)
{
  BOOST_CHECK( 0 == BlockNumber());
  BOOST_CHECK( 1 == BlockNumber( 1));
  BOOST_CHECK( 0xFFFF == BlockNumber( 0xFFFF));
}

static void testIncrement( void)
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

static void testDecrement( void)
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

static void testCompare( void)
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

void initBlockNumberTest( void)
{
  boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &testCtor));
  boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &testIncrement));
  boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &testDecrement));
  boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &testCompare));
}
