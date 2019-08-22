/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::RequestTypeDescription.
 **/

#ifndef TFTP_REQUESTTYPEDESCRIPTION_HPP
#define TFTP_REQUESTTYPEDESCRIPTION_HPP

#include <tftp/Tftp.hpp>

#include <helper/Description.hpp>

namespace Tftp {

//! Description of request type (RequestType)
class RequestTypeDescription : public Description<
  RequestTypeDescription,
  RequestType,
  RequestType::Invalid>
{
  public:
    //! Initialises the instance.
    RequestTypeDescription();
};

/**
 * @brief Parses the input stream as request type and returns them.
 *
 * @param[in] st
 *   The input stream
 * @param[out] requestType
 *   The decoded request type
 *
 * @return The input stream.
 **/
std::istream& operator>>( std::istream& st, RequestType& requestType);

}

#endif
