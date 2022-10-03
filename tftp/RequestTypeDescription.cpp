/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::RequestTypeDescription.
 **/

#include "RequestTypeDescription.hpp"

#include <boost/program_options.hpp>

#include <istream>

namespace Tftp {

RequestTypeDescription::RequestTypeDescription() :
  Description{
    { "Read",  RequestType::Read },
    { "Write", RequestType::Write }
  }
{
}

std::istream& operator>>( std::istream &stream, RequestType& requestType )
{
  std::string requestTypeStr{};

  stream >> requestTypeStr;

  // decode Request Type
  requestType = RequestTypeDescription::instance().enumeration( requestTypeStr );

  if ( RequestType::Invalid == requestType)
  {
    BOOST_THROW_EXCEPTION(
      boost::program_options::invalid_option_value{ requestTypeStr } );
  }

  return stream;
}

}
