/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Implementation of unit tests of class Tftp::Options::StringOption.
 **/

#include <tftp/options/StringOption.hpp>

#include <boost/test/unit_test.hpp>

#include <cstdint>

namespace Tftp {
namespace Options {

BOOST_AUTO_TEST_SUITE( StringOptionTest)

//! constructor test
BOOST_AUTO_TEST_CASE( constructor)
{
  StringOption entry( "test", "value");

  BOOST_CHECK( "test" == entry.getName());
  BOOST_CHECK( "value" == static_cast< std::string>( entry));
}

//! negotiation test
BOOST_AUTO_TEST_CASE( negotiate)
{
  StringOption entry( "test", "value");

  BOOST_CHECK( !entry.negotiate( "AAAAA"));
}

//! assignment test
BOOST_AUTO_TEST_CASE( assign)
{
  StringOption entry( "test", "value1");

  BOOST_CHECK( entry.getName() == "test");
  BOOST_CHECK( static_cast< std::string>( entry) == "value1");
  entry="value2";
  BOOST_CHECK( static_cast< std::string>( entry) == "value2");
}

BOOST_AUTO_TEST_SUITE_END()

}
}
