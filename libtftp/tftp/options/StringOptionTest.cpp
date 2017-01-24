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
 * @brief Implementation of unit tests of class Tftp::Options::StringOption.
 **/

#include <tftp/options/StringOption.hpp>

#include <helper/Logger.hpp>

#include <boost/test/unit_test.hpp>
#include <cstdint>

namespace Tftp {
namespace Options {

BOOST_AUTO_TEST_SUITE( StringOptionTest)

BOOST_AUTO_TEST_CASE( constructor)
{
  StringOption entry( "test", "value");

  BOOST_CHECK( "test" == entry.getName());
  BOOST_CHECK( "value" == entry.getValueString());
}

BOOST_AUTO_TEST_CASE( negotiate)
{
  StringOption entry( "test", "value");

  BOOST_CHECK( !entry.negotiate( "AAAAA"));
}

BOOST_AUTO_TEST_SUITE_END()

}
}
