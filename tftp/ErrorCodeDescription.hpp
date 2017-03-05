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
 * @brief Declaration of class Tftp::ErrorCodeDescription.
 **/

#ifndef TFTP_ERRORCODEDESCRIPTION_HPP
#define TFTP_ERRORCODEDESCRIPTION_HPP

#include <tftp/Tftp.hpp>

#include <helper/Description.hpp>

#include <iosfwd>

namespace Tftp {

class ErrorCodeDescription : public Description<
  ErrorCodeDescription,
  ErrorCode,
  ErrorCode::Invalid>
{
  public:
    ErrorCodeDescription();
};

/**
 * @brief Parses the input stream as request type and returns them.
 *
 * @param[in] stream
 *   The input stream
 * @param[out] errorCode
 *   The decoded request type
 *
 * @return The input stream.
 **/
std::ostream& operator<<( std::ostream& stream, ErrorCode& errorCode);

}

#endif
