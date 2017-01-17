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
 * @brief Definition of class Tftp::Options::StringOption.
 **/

#include "StringOption.hpp"

#include <helper/Logger.hpp>

namespace Tftp {
namespace Options {

StringOption::StringOption(const string &name, const string &value):
  Option( name),
  value( value)
{
}

StringOption::string StringOption::getValueString() const
{
  return value;
}

void StringOption::setValue( const string &value)
{
  this->value = value;
}

OptionPtr StringOption::negotiateServer( const string &) const
{
  BOOST_LOG_TRIVIAL( error) <<
    "Its not possible to use StringOption for negotiation";
  return OptionPtr();
}

OptionPtr StringOption::negotiateClient( const string &) const
{
  BOOST_LOG_TRIVIAL( error) <<
    "Its not possible to use StringOption for negotiation";
  return OptionPtr();
}

}
}
