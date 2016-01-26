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
 * @brief Definition of class Tftp::Options::Option.
 **/

#include "Option.hpp"

namespace Tftp {
namespace Options {

string Option::getOptionName( const TftpOptions option) noexcept
{
	switch (option)
	{
		case TftpOptions::BLOCKSIZE:
			return "blksize";

		case TftpOptions::TIMEOUT:
			return "timeout";

		case TftpOptions::TRANSFER_SIZE:
			return "tsize";

		default:
			break;
	}

	return string();
}

Option::Option(const std::string &name):
	name( name)
{
}

std::string Option::getName( void) const
{
	return name;
}

void Option::setName( const std::string &name)
{
	this->name = name;
}

string Option::toString( void) const
{
	return name + ":" + getValueString();
}

}
}
