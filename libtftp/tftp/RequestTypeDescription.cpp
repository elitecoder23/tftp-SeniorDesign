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
 * @brief Definition of class Tftp::RequestTypeDescription.
 **/

#include "RequestTypeDescription.hpp"

#include <boost/program_options.hpp>

#include <istream>

namespace Tftp {

RequestTypeDescription::RequestTypeDescription():
  Description(
  {
    {RequestType::Read,    {"Read",    RequestType::Read}},
    {RequestType::Write,   {"Write",   RequestType::Write}},

    {RequestType::Invalid, {"Invalid", RequestType::Invalid}}
  })
{
}

std::istream& operator>>( std::istream& st, RequestType& requestType)
{
  using std::string;

  string requestTypeStr;

  st >> requestTypeStr;

  // decode BHM area
  requestType = RequestTypeDescription::getInstance().findEnum( requestTypeStr);

  if ( RequestType::Invalid == requestType)
  {
    BOOST_THROW_EXCEPTION(
      boost::program_options::invalid_option_value( requestTypeStr));
  }

  return st;
}

}


