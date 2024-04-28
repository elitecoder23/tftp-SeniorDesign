// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Packets::ErrorCodeDescription.
 **/

#ifndef TFTP_PACKETS_ERRORCODEDESCRIPTION_HPP
#define TFTP_PACKETS_ERRORCODEDESCRIPTION_HPP

#include "tftp/packets/Packets.hpp"

#include "helper/Description.hpp"

#include <iosfwd>

namespace Tftp::Packets {

//! Description of TFTP Error Codes (ErrorCode)
class TFTP_EXPORT ErrorCodeDescription :
  public Helper::Description< ErrorCodeDescription, ErrorCode >
{
  public:
    //! Initialises the Instance
    ErrorCodeDescription();
};

/**
 * @brief Outputs the error code to the stream.
 *
 * @param[in] stream
 *   Output stream.
 * @param[out] errorCode
 *   Error code.
 *
 * @return @p stream for chaining.
 **/
TFTP_EXPORT std::ostream&
operator<<( std::ostream& stream, ErrorCode errorCode );

}

#endif
