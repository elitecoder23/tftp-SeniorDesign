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

//! Description of TFTP Error Codes (ErrorCode)
class ErrorCodeDescription : public Description<
  ErrorCodeDescription,
  ErrorCode,
  ErrorCode::Invalid>
{
  public:
    //! Initialises the instance
    ErrorCodeDescription();
};

/**
 * @brief Outputs the error code to the stream.
 *
 * @param[in] stream
 *   The output stream.
 * @param[out] errorCode
 *   The error code.
 *
 * @return The output stream.
 **/
std::ostream& operator<<( std::ostream& stream, ErrorCode errorCode);

}

#endif
