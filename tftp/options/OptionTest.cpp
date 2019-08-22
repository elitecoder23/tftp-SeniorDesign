/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Implementation of unit tests of class Tftp::Options::OptionList.
 **/

#include <tftp/options/Option.hpp>

#include <boost/test/unit_test.hpp>

#include <cstdint>

namespace Tftp::Options {

BOOST_AUTO_TEST_SUITE( OptionTest)

//! constructor test
BOOST_AUTO_TEST_CASE( optionName)
{
  BOOST_CHECK( Option::optionName( KnownOptions::BlockSize) == "blksize");
  BOOST_CHECK( Option::optionName( KnownOptions::Timeout) == "timeout");
  BOOST_CHECK( Option::optionName( KnownOptions::TransferSize) == "tsize");
  BOOST_CHECK( Option::optionName( static_cast< KnownOptions>( 99)) == std::string());
}

BOOST_AUTO_TEST_SUITE_END()

}
