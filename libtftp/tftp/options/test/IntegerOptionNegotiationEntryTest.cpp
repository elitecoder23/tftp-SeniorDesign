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
 * @brief Implementation of unit tests of the IntegerOption class
 **/

#include <tftp/options/IntegerOption.hpp>

#include <helper/Logger.hpp>

#include <boost/test/unit_test.hpp>
#include <cstdint>

using namespace Tftp::Options;

static void testCtor( void)
{
  IntegerOption<uint16_t> entry( "test", 10, 100, 50);

  BOOST_CHECK( "50" == entry.getValueString());
  BOOST_CHECK( "test" == entry.getName());
}

static void testClientNegotiation( void)
{
  IntegerOption<uint16_t> entry( "test", 10, 100, 50);
  OptionPtr negEntry;

  negEntry = entry.negotiateClient( "101");
  BOOST_CHECK( !negEntry);

  negEntry = entry.negotiateClient( "100");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "100" == negEntry->getValueString());
  // BOOST_CHECK( 100 == entry.getValue());

  negEntry = entry.negotiateClient( "99");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "99" == negEntry->getValueString());
  // BOOST_CHECK( 99 == entry.getValue());


  negEntry = entry.negotiateClient( "51");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "51" == negEntry->getValueString());
  // BOOST_CHECK( 51 == entry.getValue());

  negEntry = entry.negotiateClient( "50");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "50" == negEntry->getValueString());
  // BOOST_CHECK( 50 == entry.getValue());

  negEntry = entry.negotiateClient( "49");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "49" == negEntry->getValueString());
  // BOOST_CHECK( 49 == entry.getValue());


  negEntry = entry.negotiateClient( "11");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "11" == negEntry->getValueString());
  // BOOST_CHECK( 11 == entry.getValue());

  negEntry = entry.negotiateClient( "10");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "10" == negEntry->getValueString());
  // BOOST_CHECK( 10 == entry.getValue());

  negEntry = entry.negotiateClient( "9");
  BOOST_CHECK( !negEntry);
}

static void testServerNegotiation( void)
{
  IntegerOption<uint16_t> entry( "test", 10, 100, 50);
  OptionPtr negEntry;

  negEntry = entry.negotiateServer( "101");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "100" == negEntry->getValueString());
  // BOOST_CHECK( 100 == entry.getValue());

  negEntry = entry.negotiateServer( "100");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "100" == negEntry->getValueString());
  // BOOST_CHECK( 100 == entry.getValue());

  negEntry = entry.negotiateServer( "99");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "99" == negEntry->getValueString());
  // BOOST_CHECK( 99 == entry.getValue());


  negEntry = entry.negotiateServer( "51");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "51" == negEntry->getValueString());
  // BOOST_CHECK( 51 == entry.getValue());

  negEntry = entry.negotiateServer( "50");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "50" == negEntry->getValueString());
  // BOOST_CHECK( 50 == entry.getValue());

  negEntry = entry.negotiateServer( "49");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "49" == negEntry->getValueString());
  // BOOST_CHECK( 49 == entry.getValue());


  negEntry = entry.negotiateServer( "11");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "11" == negEntry->getValueString());
  // BOOST_CHECK( 11 == entry.getValue());

  negEntry = entry.negotiateServer( "10");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "10" == negEntry->getValueString());
  // BOOST_CHECK( 10 == entry.getValue());

  negEntry = entry.negotiateServer( "9");
  BOOST_CHECK( !negEntry);
}

void initIntegerOptionNegotiationEntryTest()
{
  boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &testCtor));
  boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &testClientNegotiation));
  boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &testServerNegotiation));
}
