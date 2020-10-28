/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::BlockNumber.
 **/

#include "BlockNumber.hpp"

namespace Tftp::Packets {

BlockNumber::BlockNumber() noexcept: blockNumberV{ 0U}
{
}

BlockNumber::BlockNumber( uint16_t blockNumber) noexcept:
  blockNumberV{ blockNumber}
{
}

uint16_t BlockNumber::next() const
{
  // roll-over handling
  if ( blockNumberV == 0xFFFFU )
  {
    return 1U;
  }

  return blockNumberV + 1U;
}

uint16_t BlockNumber::previous() const
{
  // roll-over handling
  if ( blockNumberV == 1U )
  {
    return 0xFFFFU;
  }

  return blockNumberV - 1U;
}

BlockNumber::operator uint16_t() const
{
  return blockNumberV;
}

BlockNumber::operator uint16_t&()
{
  return blockNumberV;
}

BlockNumber& BlockNumber::operator=( const uint16_t blockNumber )
{
  blockNumberV = blockNumber;
  return *this;
}

BlockNumber& BlockNumber::operator++()
{
  blockNumberV = next();
  return *this;
}

const BlockNumber BlockNumber::operator++( int )
{
  const BlockNumber old{ blockNumberV };
  blockNumberV = next();
  return old;
}

BlockNumber& BlockNumber::operator--()
{
  blockNumberV = previous();
  return *this;
}

const BlockNumber BlockNumber::operator--( int )
{
  const BlockNumber old{ blockNumberV };
  blockNumberV = previous();
  return old;
}

bool BlockNumber::operator ==( const BlockNumber &rhs ) const
{
  return blockNumberV == rhs.blockNumberV;
}

bool BlockNumber::operator ==( const uint16_t rhs ) const
{
  return blockNumberV == rhs;
}

bool BlockNumber::operator !=( const BlockNumber &rhs ) const
{
  return blockNumberV != rhs.blockNumberV;
}

bool BlockNumber::operator !=( const uint16_t rhs ) const
{
  return blockNumberV != rhs;
}

bool operator==( uint16_t lhs, const BlockNumber &rhs )
{
  return lhs == static_cast< uint16_t>( rhs);
}

bool operator!=( uint16_t lhs, const BlockNumber &rhs )
{
  return lhs != static_cast< uint16_t>( rhs);
}

}
