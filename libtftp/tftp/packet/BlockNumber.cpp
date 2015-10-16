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
 * @brief Definition of class BlockNumber.
 **/

#include "BlockNumber.hpp"

using namespace Tftp::Packet;

BlockNumber::BlockNumber( const uint16_t blockNumber) noexcept:
	blockNumber( blockNumber)
{
}

uint16_t BlockNumber::next( void) const
{
	if (blockNumber==0xFFFF)
	{
		return 1;
	}

	return blockNumber + 1;
}

uint16_t BlockNumber::previous( void) const
{
	if (blockNumber==1)
	{
		return 0xFFFF;
	}

	return blockNumber-1;
}

BlockNumber::operator uint16_t( void) const
{
	return blockNumber;
}

BlockNumber::operator uint16_t&( void)
{
	return blockNumber;
}

BlockNumber& BlockNumber::operator=( const uint16_t blockNumber)
{
	this->blockNumber = blockNumber;
	return *this;
}

BlockNumber& BlockNumber::operator++( void)
{
	blockNumber = next();
	return *this;
}

BlockNumber BlockNumber::operator++( int)
{
	BlockNumber old( blockNumber);
	blockNumber = next();
	return old;
}

BlockNumber& BlockNumber::operator--( void)
{
	blockNumber = previous();
	return *this;
}

BlockNumber BlockNumber::operator--( int)
{
	BlockNumber old( blockNumber);
	blockNumber = previous();
	return old;
}

bool BlockNumber::operator ==( const BlockNumber &rhs) const
{
	return blockNumber == rhs.blockNumber;
}

bool BlockNumber::operator ==( const uint16_t &rhs) const
{
	return blockNumber == rhs;
}

bool BlockNumber::operator !=( const BlockNumber &rhs) const
{
	return blockNumber != rhs.blockNumber;
}

bool BlockNumber::operator !=( const uint16_t &rhs) const
{
	return blockNumber != rhs;
}
