/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Class Tftp::Version.
 **/

#include <tftp/Version.hpp>

#include <boost/test/unit_test.hpp>

#include <iostream>

namespace Tftp {

BOOST_AUTO_TEST_SUITE( TftpTest )
BOOST_AUTO_TEST_SUITE( VersionTest )

//! Version Test
BOOST_AUTO_TEST_CASE( version )
{
  std::cout
    << "TFTP Version: "
    << Version::Major << "."
    << Version::Minor << "."
    << Version::Patch << "-"
    << Version::Addition << "\n";

  std::cout
    << "TFTP Version Information: " << Version::VersionInformation << "\n";
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
