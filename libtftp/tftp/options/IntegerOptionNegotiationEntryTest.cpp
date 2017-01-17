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
 * @brief Implementation of unit tests of class Tftp::Options::IntegerOption.
 **/

#include <tftp/options/IntegerOption.hpp>

#include <helper/Logger.hpp>

#include <boost/test/unit_test.hpp>
#include <cstdint>

namespace Tftp {
namespace Options {

BOOST_AUTO_TEST_SUITE( IntegerOptionTest)

BOOST_AUTO_TEST_CASE( constructor)
{
  IntegerOption<uint16_t> entry( "test", 10, 100, 50);

  BOOST_CHECK( "50" == entry.getValueString());
  BOOST_CHECK( "test" == entry.getName());
}

BOOST_AUTO_TEST_CASE( negotiateClient)
{
  IntegerOption<uint16_t> entry( "test", 10, 100, 50);
  OptionPtr negEntry;

  negEntry = entry.negotiateClient( "101");
  BOOST_CHECK( !negEntry);

  negEntry = entry.negotiateClient( "100");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "100" == negEntry->getValueString());
  BOOST_CHECK( 100 == std::dynamic_pointer_cast< IntegerOption<uint16_t> >( negEntry)->getValue());

  negEntry = entry.negotiateClient( "99");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "99" == negEntry->getValueString());
  BOOST_CHECK( 99 == std::dynamic_pointer_cast< IntegerOption<uint16_t> >( negEntry)->getValue());


  negEntry = entry.negotiateClient( "51");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "51" == negEntry->getValueString());
  BOOST_CHECK( 51 == std::dynamic_pointer_cast< IntegerOption<uint16_t> >( negEntry)->getValue());

  negEntry = entry.negotiateClient( "50");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "50" == negEntry->getValueString());
  BOOST_CHECK( 50 == std::dynamic_pointer_cast< IntegerOption<uint16_t> >( negEntry)->getValue());

  negEntry = entry.negotiateClient( "49");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "49" == negEntry->getValueString());
  BOOST_CHECK( 49 == std::dynamic_pointer_cast< IntegerOption<uint16_t> >( negEntry)->getValue());


  negEntry = entry.negotiateClient( "11");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "11" == negEntry->getValueString());
  BOOST_CHECK( 11 == std::dynamic_pointer_cast< IntegerOption<uint16_t> >( negEntry)->getValue());

  negEntry = entry.negotiateClient( "10");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "10" == negEntry->getValueString());
  BOOST_CHECK( 10 == std::dynamic_pointer_cast< IntegerOption<uint16_t> >( negEntry)->getValue());

  negEntry = entry.negotiateClient( "9");
  BOOST_CHECK( !negEntry);
}

BOOST_AUTO_TEST_CASE( negotiateServer)
{
  IntegerOption<uint16_t> entry( "test", 10, 100, 50);
  BOOST_CHECK( 50 == entry.getValue());

  OptionPtr negEntry;

  negEntry = entry.negotiateServer( "101");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "100" == negEntry->getValueString());
  BOOST_CHECK( 100 == std::dynamic_pointer_cast< IntegerOption<uint16_t> >( negEntry)->getValue());

  negEntry = entry.negotiateServer( "100");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "100" == negEntry->getValueString());
  BOOST_CHECK( 100 == std::dynamic_pointer_cast< IntegerOption<uint16_t> >( negEntry)->getValue());

  negEntry = entry.negotiateServer( "99");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "99" == negEntry->getValueString());
  BOOST_CHECK( 99 == std::dynamic_pointer_cast< IntegerOption<uint16_t> >( negEntry)->getValue());


  negEntry = entry.negotiateServer( "51");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "51" == negEntry->getValueString());
  BOOST_CHECK( 51 == std::dynamic_pointer_cast< IntegerOption<uint16_t> >( negEntry)->getValue());

  negEntry = entry.negotiateServer( "50");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "50" == negEntry->getValueString());
  BOOST_CHECK( 50 == std::dynamic_pointer_cast< IntegerOption<uint16_t> >( negEntry)->getValue());

  negEntry = entry.negotiateServer( "49");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "49" == negEntry->getValueString());
  BOOST_CHECK( 49 == std::dynamic_pointer_cast< IntegerOption<uint16_t> >( negEntry)->getValue());


  negEntry = entry.negotiateServer( "11");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "11" == negEntry->getValueString());
  BOOST_CHECK( 11 == std::dynamic_pointer_cast< IntegerOption<uint16_t> >( negEntry)->getValue());

  negEntry = entry.negotiateServer( "10");
  BOOST_CHECK( negEntry);
  BOOST_CHECK( "10" == negEntry->getValueString());
  BOOST_CHECK( 10 == std::dynamic_pointer_cast< IntegerOption<uint16_t> >( negEntry)->getValue());

  negEntry = entry.negotiateServer( "9");
  BOOST_CHECK( !negEntry);
}

BOOST_AUTO_TEST_SUITE_END()

}
}
