/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Implementation of Unit Tests of Class Tftp::Options::OptionNegotiation.
 **/

#include <tftp/options/OptionNegotiation.hpp>

#include <boost/test/unit_test.hpp>
#include <cstdint>

namespace Tftp::Options {

BOOST_AUTO_TEST_SUITE( TftpTest)
BOOST_AUTO_TEST_SUITE( OptionsTest)
BOOST_AUTO_TEST_SUITE( OptionNegotiationTest)

BOOST_AUTO_TEST_CASE( toString)
{
  BOOST_CHECK( "0" == OptionNegotiation::toString( 0));
  BOOST_CHECK( "99" == OptionNegotiation::toString( 99));
}

BOOST_AUTO_TEST_CASE( toInt)
{
  BOOST_CHECK( 0 == OptionNegotiation::toInt( "0"));
  BOOST_CHECK( 99 == OptionNegotiation::toInt( "99"));
  BOOST_CHECK_THROW( OptionNegotiation::toInt( "XXX"), TftpException);
}

//! negotiation min max test
BOOST_AUTO_TEST_CASE( negotiateNegotiateMinMaxRange)
{
  NegotiateMinMaxRange negotiation{ 10,100};

  BOOST_CHECK( "" == negotiation.negotiate( "101"));
  BOOST_CHECK( "100" == negotiation.negotiate( "100"));
  BOOST_CHECK( "99" == negotiation.negotiate( "99"));
  BOOST_CHECK( "50" == negotiation.negotiate( "50"));
  BOOST_CHECK( "10" == negotiation.negotiate( "10"));
  BOOST_CHECK( "" == negotiation.negotiate( "9"));
}

//! negotiation min max smaller test
BOOST_AUTO_TEST_CASE( negotiateMinMaxSmaller)
{
  NegotiateMinMaxSmaller negotiation{10, 100};

  BOOST_CHECK( "100" == negotiation.negotiate( "101"));
  BOOST_CHECK( "100" == negotiation.negotiate( "100"));
  BOOST_CHECK( "99" == negotiation.negotiate( "99"));
  BOOST_CHECK( "50" == negotiation.negotiate( "50"));
  BOOST_CHECK( "10" == negotiation.negotiate( "10"));
  BOOST_CHECK( "" == negotiation.negotiate( "9"));
}

//! negotiation exact value test
BOOST_AUTO_TEST_CASE( negotiateExactValue)
{
  NegotiateExactValue negotiation{ 50};

  BOOST_CHECK( "" == negotiation.negotiate( "0"));
  BOOST_CHECK( "" == negotiation.negotiate( "1"));
  BOOST_CHECK( "" == negotiation.negotiate( "49"));
  BOOST_CHECK( "50" == negotiation.negotiate( "50"));
  BOOST_CHECK( "" == negotiation.negotiate( "51"));
}

//! negotiation always pass test
BOOST_AUTO_TEST_CASE( negotiateAlwaysPass)
{
  NegotiateAlwaysPass negotiation{};

  BOOST_CHECK( "0" == negotiation.negotiate( "0"));
  BOOST_CHECK( "1" == negotiation.negotiate( "1"));
  BOOST_CHECK( "49" == negotiation.negotiate( "49"));
  BOOST_CHECK( "50" == negotiation.negotiate( "50"));
  BOOST_CHECK( "51" == negotiation.negotiate( "51"));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
