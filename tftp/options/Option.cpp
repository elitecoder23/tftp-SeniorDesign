/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class Tftp::Options::Option.
 **/

#include "Option.hpp"

namespace Tftp {
namespace Options {

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
  nameValue( name)
{
  // Validate option name
  assert( !name.empty());
}

Option::Option( std::string &&name):
  nameValue( std::move( name))
{
  // Validate option name
  assert( !nameValue.empty());
}

std::string Option::name() const
{
  return nameValue;
}

void Option::name( const std::string &name)
{
  // Validate option name
  assert( !name.empty());

  nameValue = name;
}

std::string Option::toString() const
{
  return nameValue + ":" + static_cast< std::string>( *this);
}

}
}
