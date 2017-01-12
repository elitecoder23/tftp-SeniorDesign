/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * $Date$
 * $Revision$
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration/ definition of TFTP unit test main entry
 **/

#include <helper/Logger.hpp>

#define BOOST_TEST_MODULE test module name
#include <boost/test/included/unit_test.hpp>

#if 0
#include <boost/test/unit_test.hpp>

using namespace boost::unit_test;

void initBlockNumberTest( void);
void initIntegerOptionNegotiationEntryTest( void);
void initOptionNegotiationEntryTest( void);
void initTftpPacketTest( void);

boost::unit_test::test_suite* init_unit_test_suite( int, char*[])
{
  initBlockNumberTest();
  initIntegerOptionNegotiationEntryTest();
  initOptionNegotiationEntryTest();
  initTftpPacketTest();

  return 0;
}

#endif
