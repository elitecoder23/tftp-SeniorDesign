/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Implementation of Unit Tests of Class Tftp::OptionList.
 **/

#include <tftp/TftpOptionsConfiguration.hpp>

#include <boost/test/unit_test.hpp>

namespace Tftp {

BOOST_AUTO_TEST_SUITE( TftpTest )
BOOST_AUTO_TEST_SUITE( TftpOptionsConfigurationTest )

//! optionName test
BOOST_AUTO_TEST_CASE( optionName )
{
  BOOST_CHECK( TftpOptionsConfiguration::optionName( KnownOptions::BlockSize ) == "blksize" );
  BOOST_CHECK( TftpOptionsConfiguration::optionName( KnownOptions::Timeout ) == "timeout" );
  BOOST_CHECK( TftpOptionsConfiguration::optionName( KnownOptions::TransferSize ) == "tsize" );
  BOOST_CHECK( TftpOptionsConfiguration::optionName( static_cast< KnownOptions >( 99 ) )
    == std::string{} );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
