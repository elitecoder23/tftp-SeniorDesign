/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Options::Option.
 **/

#include "Option.hpp"

#include <cassert>

namespace Tftp::Options {

std::string Option::optionName( const KnownOptions option) noexcept
{
  switch (option)
  {
    case KnownOptions::BlockSize:
      return "blksize";

    case KnownOptions::Timeout:
      return "timeout";

    case KnownOptions::TransferSize:
      return "tsize";

    default:
      break;
  }

  return {};
}

Option::Option( const std::string &name):
  nameV( name)
{
  // Validate option name
  assert( !name.empty());
}

Option::Option( std::string &&name):
  nameV( std::move( name))
{
  // Validate option name
  assert( !nameV.empty());
}

const std::string& Option::name() const
{
  return nameV;
}

void Option::name( const std::string &name)
{
  // Validate option name
  assert( !name.empty());

  nameV = name;
}

std::string Option::toString() const
{
  return nameV + ":" + static_cast< std::string>( *this);
}

}
