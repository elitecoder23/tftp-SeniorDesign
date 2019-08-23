/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Implementation of unit tests of class Tftp::Options::IntegerOption.
 **/

#include <tftp/options/IntegerOption.hpp>

#include <boost/test/unit_test.hpp>
#include <cstdint>

namespace Tftp::Options {

BOOST_AUTO_TEST_SUITE( IntegerOptionTest)

//! constructor test
BOOST_AUTO_TEST_CASE( constructor8u)
{
  using namespace std::literals::string_view_literals;
  IntegerOption< uint8_t, NegotiateAlwaysPass< uint8_t>> entry( "test"sv, 50, NegotiateAlwaysPass< uint8_t>());

  BOOST_CHECK( "50" == static_cast< std::string>( entry));
  BOOST_CHECK( 50 == entry);
  BOOST_CHECK( "test" == entry.name());
}

//! constructor test
BOOST_AUTO_TEST_CASE( constructor16u)
{
  using namespace std::literals::string_view_literals;
  IntegerOption< uint16_t, NegotiateAlwaysPass< uint16_t>> entry( "test"sv, 50, NegotiateAlwaysPass< uint16_t>());

  BOOST_CHECK( "50" == static_cast< std::string>( entry));
  BOOST_CHECK( 50 == entry);
  BOOST_CHECK( "test" == entry.name());
}

//! assignment test
BOOST_AUTO_TEST_CASE( assign)
{
  using namespace std::literals::string_view_literals;
  IntegerOption< uint8_t, NegotiateAlwaysPass< uint8_t>> entry( "test"sv, 50, NegotiateAlwaysPass< uint8_t>());

  BOOST_CHECK( 50 == entry);
  entry = uint8_t( 100);
  BOOST_CHECK( 100 == entry);
}

//! negotiation min max test
BOOST_AUTO_TEST_CASE( negotiateNegotiateMinMaxRange)
{
  using namespace std::literals::string_view_literals;
  BlockSizeOptionClient entry( "test"sv, 50, NegotiateMinMaxRange< uint16_t>(10,50));
  OptionPtr negEntry;

  negEntry = entry.negotiate( "101");
  BOOST_CHECK( !negEntry);

  negEntry = entry.negotiate( "100");
  BOOST_CHECK( !negEntry);

  negEntry = entry.negotiate( "99");
  BOOST_CHECK( !negEntry);


  negEntry = entry.negotiate( "51");
  BOOST_CHECK( !negEntry);

  negEntry = entry.negotiate( "50");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "50" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 50 == *std::dynamic_pointer_cast< BlockSizeOptionBase >( negEntry));

  negEntry = entry.negotiate( "49");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "49" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 49 == *std::dynamic_pointer_cast< BlockSizeOptionBase >( negEntry));


  negEntry = entry.negotiate( "11");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "11" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 11 == *std::dynamic_pointer_cast< BlockSizeOptionBase >( negEntry));

  negEntry = entry.negotiate( "10");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "10" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 10 == *std::dynamic_pointer_cast< BlockSizeOptionBase >( negEntry));

  negEntry = entry.negotiate( "9");
  BOOST_CHECK( !negEntry);
}

//! negotiation min max smaller test
BOOST_AUTO_TEST_CASE( negotiateMinMaxSmaller)
{
  using namespace std::literals::string_view_literals;
  BlockSizeOptionServer entry( "test"sv, 50, NegotiateMinMaxSmaller< uint16_t>(10, 100));
  BOOST_CHECK( 50 == entry);

  OptionPtr negEntry;

  negEntry = entry.negotiate( "101");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "100" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 100 == *std::dynamic_pointer_cast< BlockSizeOptionBase >( negEntry));

  negEntry = entry.negotiate( "100");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "100" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 100 == *std::dynamic_pointer_cast< BlockSizeOptionBase >( negEntry));

  negEntry = entry.negotiate( "99");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "99" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 99 == *std::dynamic_pointer_cast< BlockSizeOptionBase >( negEntry));


  negEntry = entry.negotiate( "51");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "51" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 51 == *std::dynamic_pointer_cast< BlockSizeOptionBase >( negEntry));

  negEntry = entry.negotiate( "50");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "50" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 50 == *std::dynamic_pointer_cast< BlockSizeOptionBase >( negEntry));

  negEntry = entry.negotiate( "49");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "49" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 49 == *std::dynamic_pointer_cast< BlockSizeOptionBase >( negEntry));


  negEntry = entry.negotiate( "11");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "11" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 11 == *std::dynamic_pointer_cast< BlockSizeOptionBase >( negEntry));

  negEntry = entry.negotiate( "10");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "10" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 10 == *std::dynamic_pointer_cast< BlockSizeOptionBase >( negEntry));

  negEntry = entry.negotiate( "9");
  BOOST_CHECK( !negEntry);
}

//! negotiation exact value test
BOOST_AUTO_TEST_CASE( negotiateExactValue)
{
  using namespace std::literals::string_view_literals;
  TimeoutOptionClient entry( "test"sv, 50, NegotiateExactValue< uint8_t>(50));
  BOOST_CHECK( 50 == entry);

  OptionPtr negEntry;

  negEntry = entry.negotiate( "0");
  BOOST_CHECK( !negEntry);

  negEntry = entry.negotiate( "1");
  BOOST_CHECK( !negEntry);

  negEntry = entry.negotiate( "49");
  BOOST_CHECK( !negEntry);

  negEntry = entry.negotiate( "50");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "50" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 50 == *std::dynamic_pointer_cast< TimeoutOptionBase >( negEntry));

  negEntry = entry.negotiate( "51");
  BOOST_CHECK( !negEntry);
}

//! negotiation always pass test
BOOST_AUTO_TEST_CASE( negotiateAlwaysPass)
{
  using namespace std::literals::string_view_literals;
  TransferSizeOptionServerClient entry( "test"sv, 50);
  BOOST_CHECK( 50 == entry);

  OptionPtr negEntry;

  negEntry = entry.negotiate( "0");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "0" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 0 == *std::dynamic_pointer_cast< TransferSizeOptionBase >( negEntry));

  negEntry = entry.negotiate( "1");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "1" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 1 == *std::dynamic_pointer_cast< TransferSizeOptionBase >( negEntry));

  negEntry = entry.negotiate( "49");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "49" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 49 == *std::dynamic_pointer_cast< TransferSizeOptionBase >( negEntry));

  negEntry = entry.negotiate( "50");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "50" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 50 == *std::dynamic_pointer_cast< TransferSizeOptionBase >( negEntry));

  negEntry = entry.negotiate( "51");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "51" == static_cast< std::string>( *negEntry));
  BOOST_CHECK( 51 == *std::dynamic_pointer_cast< TransferSizeOptionBase >( negEntry));
}

BOOST_AUTO_TEST_SUITE_END()

}
