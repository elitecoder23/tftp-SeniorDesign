// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::TransferStatusDescription.
 **/

#ifndef TFTP_TRANSFERSTATUSDESCRIPTION_HPP
#define TFTP_TRANSFERSTATUSDESCRIPTION_HPP

#include <tftp/Tftp.hpp>

#include <helper/Description.hpp>

#include <iosfwd>

namespace Tftp {

/**
 * @name TFTP Transfer Status
 * @{
 **/

//! Description of TFTP Transfer Status
class TFTP_EXPORT TransferStatusDescription final : public Helper::Description<
  TransferStatusDescription,
  TransferStatus >
{
  public:
    //! Initialises the Instance.
    TransferStatusDescription();
};

/**
 * @brief Outputs the Error Code to the stream.
 *
 * @param[in] stream
 *   The output stream.
 * @param[out] transferStatus
 *   Transfer Status.
 *
 * @return The output stream.
 **/
TFTP_EXPORT std::ostream &operator<<( std::ostream &stream, TransferStatus transferStatus );

/** @} **/

}

#endif
