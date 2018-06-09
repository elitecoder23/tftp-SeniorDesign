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

StringOption::StringOption(const std::string &name, const std::string &value):
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

StringOption& StringOption::operator=( const std::string &value)
{
  this->value = value;
  return *this;
}

OptionPtr StringOption::negotiate( std::string_view) const noexcept
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
    "Its not possible to use StringOption for negotiation";

  return {};
}

}
}
