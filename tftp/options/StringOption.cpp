/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class Tftp::Options::StringOption.
 **/

#include "StringOption.hpp"

#include <tftp/TftpLogger.hpp>

namespace Tftp {
namespace Options {

StringOption::StringOption(const string &name, const string &value):
  Option( name),
  value( value)
{
}

StringOption::operator StringOption::string() const
{
  return value;
}

StringOption& StringOption::operator=( const string &value)
{
  this->value = value;
  return *this;
}

OptionPtr StringOption::negotiate( const string &) const noexcept
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
    "Its not possible to use StringOption for negotiation";

  return OptionPtr();
}

}
}
