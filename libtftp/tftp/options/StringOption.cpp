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
 * @brief Definition of class StringOption.
 **/

#include "StringOption.hpp"

#include <helper/Logger.hpp>

namespace Tftp {
namespace Options {

StringOption::StringOption(const string &name, const string &value):
	Option( name),
	value( value)
{
}

string StringOption::getValueString( void) const
{
	return value;
}

void StringOption::setValue( const string &value)
{
	this->value = value;
}

OptionPointer StringOption::negotiateServer( const string &) const
{
	BOOST_LOG_TRIVIAL( error) <<
		"Its not possible to use StringOption for negotiation";
	return OptionPointer();
}

OptionPointer StringOption::negotiateClient( const string &) const
{
	BOOST_LOG_TRIVIAL( error) <<
		"Its not possible to use StringOption for negotiation";
	return OptionPointer();
}

}
}
