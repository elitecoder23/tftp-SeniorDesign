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
 * @brief Implementation of unit tests of the OptionNegotiation class
 **/

#include <tftp/options/OptionList.hpp>
#include <tftp/options/IntegerOption.hpp>

#include <helper/Logger.hpp>

#include <boost/test/unit_test.hpp>
#include <cstdint>

using namespace Tftp::Options;

#if 0
static void testCtor( void)
{
	OptionNegotiation optionNegotiation;

	BOOST_CHECK( optionNegotiation.getDefaultOptions().getOptions().empty());
}

static void testClientNegotiation( void)
{
	OptionNegotiation optionNegotiation;
	OptionList optionList;

	optionNegotiation.addOption(
		"a",
		OptionNegotiation::NegotiationEntryPointer(
			new IntegerOptionNegotiationEntry<uint16_t>( 10, 100, 50)));

	optionList.setOption( "a", "99");
	BOOST_CHECK( optionNegotiation.negotiateClient( optionList));
	BOOST_CHECK( optionNegotiation.getOption( "a")->getNegotiated() == "99");
	optionList.getOptions().clear();

	optionList.setOption( "a", "100");
	BOOST_CHECK( optionNegotiation.negotiateClient( optionList));
	BOOST_CHECK( optionNegotiation.getOption( "a")->getNegotiated() == "100");
	optionList.getOptions().clear();

	optionList.setOption( "a", "101");
	BOOST_CHECK( !optionNegotiation.negotiateClient( optionList));
	BOOST_CHECK( optionNegotiation.getOption( "a")->getNegotiated() == "");
	optionList.getOptions().clear();

	optionList.setOption( "a", "51");
	BOOST_CHECK( optionNegotiation.negotiateClient( optionList));
	BOOST_CHECK( optionNegotiation.getOption( "a")->getNegotiated() == "51");
	optionList.getOptions().clear();

	optionList.setOption( "a", "50");
	BOOST_CHECK( optionNegotiation.negotiateClient( optionList));
	BOOST_CHECK( optionNegotiation.getOption( "a")->getNegotiated() == "50");
	optionList.getOptions().clear();

	optionList.setOption( "a", "49");
	BOOST_CHECK( optionNegotiation.negotiateClient( optionList));
	BOOST_CHECK( optionNegotiation.getOption( "a")->getNegotiated() == "49");
	optionList.getOptions().clear();

	optionList.setOption( "a", "11");
	BOOST_CHECK( optionNegotiation.negotiateClient( optionList));
	BOOST_CHECK( optionNegotiation.getOption( "a")->getNegotiated() == "11");
	optionList.getOptions().clear();

	optionList.setOption( "a", "10");
	BOOST_CHECK( optionNegotiation.negotiateClient( optionList));
	BOOST_CHECK( optionNegotiation.getOption( "a")->getNegotiated() == "10");
	optionList.getOptions().clear();

	optionList.setOption( "a", "9");
	BOOST_CHECK( !optionNegotiation.negotiateClient( optionList));
	BOOST_CHECK( optionNegotiation.getOption( "a")->getNegotiated() == "");
	optionList.getOptions().clear();

	optionList.setOption( "a", "66");
	BOOST_CHECK( optionNegotiation.negotiateClient( optionList));
	BOOST_CHECK( optionNegotiation.getOption( "a")->getNegotiated() == "66");

	optionList.setOption( "b", "55");
	BOOST_CHECK( !optionNegotiation.negotiateClient( optionList));
}

static void testServerNegotiation( void)
{

}
#endif
void initOptionNegotiationEntryTest( void)
{
#if 0
	boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &testCtor));
	boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &testClientNegotiation));
	boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &testServerNegotiation));
#endif
}
