// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Unit Tests for Class Tftp::Version.
 **/

#include <tftp/Version.hpp>

#include <boost/test/unit_test.hpp>

namespace Tftp {

BOOST_AUTO_TEST_SUITE( TftpTest )
BOOST_AUTO_TEST_SUITE( VersionTest )

//! Version Test
BOOST_AUTO_TEST_CASE( version )
{
  BOOST_TEST_MESSAGE( "Key " << Version::Key );
  BOOST_TEST_MESSAGE( "Name " <<Version::Name );
  BOOST_TEST_MESSAGE( "Number " << Version::Number );
  BOOST_TEST_MESSAGE( "SCM " << Version::Scm );
  BOOST_TEST_MESSAGE( "URL " << Version::Url );
  BOOST_TEST_MESSAGE( "License " << Version::License );

  BOOST_TEST_MESSAGE( "TFTP Version Information: " << Version::VersionInformation );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
