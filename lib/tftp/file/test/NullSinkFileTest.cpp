// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Implementation of Unit Tests of Class Tftp::File::NullSinkFile.
 **/

#include <tftp/file/NullSinkFile.hpp>

#include <boost/test/unit_test.hpp>

namespace Tftp::File {

BOOST_AUTO_TEST_SUITE( TftpTest)
BOOST_AUTO_TEST_SUITE( FileTest)
BOOST_AUTO_TEST_SUITE( NullSinkFileTest )

//! Constructor test
BOOST_AUTO_TEST_CASE( constructor1 )
{
  NullSinkFile file1{};
  BOOST_CHECK( file1.receivedTransferSize( 0 ) );
  BOOST_CHECK( file1.receivedTransferSize( UINT64_MAX ) );
  BOOST_CHECK_NO_THROW( file1.receivedData( {} ) );
  BOOST_CHECK_NO_THROW( file1.receivedData( { { 0x01, 0x02, 0x03 } } ) );

  NullSinkFile file2{ 1000 };
  BOOST_CHECK( file2.receivedTransferSize( 0 ) );
  BOOST_CHECK( file2.receivedTransferSize( 999 ) );
  BOOST_CHECK( file2.receivedTransferSize( 1000 ) );
  BOOST_CHECK( !file2.receivedTransferSize( 1001 ) );
  BOOST_CHECK( !file2.receivedTransferSize( UINT64_MAX ) );
  BOOST_CHECK_NO_THROW( file2.receivedData( {} ) );
  BOOST_CHECK_NO_THROW( file2.receivedData( { { 0x01, 0x02, 0x03 } } ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

}
