/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Options::StringOption.
 **/

#include "StringOption.hpp"

#include <tftp/TftpLogger.hpp>

namespace Tftp::Options {

StringOption::StringOption( std::string_view name, std::string_view value):
  Option( name),
  value( value)
{
}

StringOption::StringOption( std::string &&name, std::string &&value):
  Option( std::move( name)),
  value( std::move( value))
{
}

StringOption::operator std::string() const
{
  return value;
}

StringOption& StringOption::operator=( std::string_view value)
{
  this->value = value;
  return *this;
}

StringOption& StringOption::operator=( std::string &&value)
{
  this->value = std::move( value);
  return *this;
}

OptionPtr StringOption::negotiate( std::string_view) const noexcept
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
    "Its not possible to use StringOption for negotiation";

  return {};
}

}
