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

BlockNumber::BlockNumber() noexcept:
  blockNumberValue{ 0U}
{
}

BlockNumber::BlockNumber( uint16_t blockNumber) noexcept:
  blockNumberValue{ blockNumber}
{
}

uint16_t BlockNumber::next() const
{
  if ( blockNumberValue == 0xFFFFU)
  {
    return 1U;
  }

  return blockNumberValue + 1U;
}

uint16_t BlockNumber::previous() const
{
  if ( blockNumberValue == 1U)
  {
    return 0xFFFFU;
  }

  return blockNumberValue - 1U;
}

BlockNumber::operator uint16_t() const
{
  return blockNumberValue;
}

BlockNumber::operator uint16_t&()
{
  return blockNumberValue;
}

BlockNumber& BlockNumber::operator=( const uint16_t blockNumber)
{
  blockNumberValue = blockNumber;
  return *this;
}

BlockNumber& BlockNumber::operator++()
{
  blockNumberValue = next();
  return *this;
}

const BlockNumber BlockNumber::operator++( int)
{
  const BlockNumber old{ blockNumberValue};
  blockNumberValue = next();
  return old;
}

BlockNumber& BlockNumber::operator--()
{
  blockNumberValue = previous();
  return *this;
}

const BlockNumber BlockNumber::operator--( int)
{
  const BlockNumber old{ blockNumberValue};
  blockNumberValue = previous();
  return old;
}

bool BlockNumber::operator ==( const BlockNumber &rhs) const
{
  return blockNumberValue == rhs.blockNumberValue;
}

bool BlockNumber::operator ==( uint16_t rhs) const
{
  return blockNumberValue == rhs;
}

bool BlockNumber::operator !=( const BlockNumber &rhs) const
{
  return blockNumberValue != rhs.blockNumberValue;
}

bool BlockNumber::operator !=( uint16_t rhs) const
{
  return blockNumberValue != rhs;
}

}
