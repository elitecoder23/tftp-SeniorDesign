/*
 * $Date$
 * $Revision$
 */
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

Option::string Option::getOptionName( const KnownOptions option) noexcept
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

  return string();
}

Option::Option( const string &name):
  name( name)
{
  // Validate option name
  assert( !name.empty());
}

Option::string Option::getName() const
{
  return name;
}

void Option::setName( const std::string &name)
{
  // Validate option name
  assert( !name.empty());

  this->name = name;
}

Option::string Option::toString() const
{
  return name + ":" + static_cast< std::string>( *this);
}

}
}
