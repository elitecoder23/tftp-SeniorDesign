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
 * @brief Declaration of Module Tftp::Packets TftpOptions.
 **/

#ifndef TFTP_PACKETS_TFTPOPTIONS_HPP
#define TFTP_PACKETS_TFTPOPTIONS_HPP

#include "tftp/packets/Packets.hpp"

#include <iosfwd>
#include <optional>
#include <string>

namespace Tftp::Packets {

/**
 * @brief Decoded TFTP Options
 *
 * Used to store all known TFTP Options like:
 * - blocksize,
 * - timeout, and
 * - transfer size.
 **/
struct TFTP_EXPORT TftpOptions final
{
  /**
   * @brief Block size option (RFC 2348)
   *
   * The number of octets in a block.
   * Valid values range between "8" and "65464" octets, inclusive.
   * The *blocksize* refers to the number of data octets; it does not include
   * the four octets of TFTP header.
   **/
  std::optional< uint16_t > blockSize;
  /**
   * @brief Timeout option (RFC 2349)
   *
   * The number of seconds to wait before retransmitting.
   * Valid values range between "1" and "255" seconds, inclusive.
   **/
  std::optional< uint8_t > timeout;
  /**
   * @brief Transfer size option (RFC 2349)
   *
   * The size of the file to be transferred.
   * Allows the side receiving the file to determine the ultimate size of the
   * transfer before it begins.
   **/
  std::optional< uint64_t > transferSize;

  /**
   * @brief Returns if any option is set.
   *
   * @return If any option is set
   **/
  operator bool() const
  {
    return blockSize || timeout || transferSize;
  }
};

/**
 * @brief Returns the Option Name String for the given Option.
 *
 * @param[in] option
 *   TFTP known Option.
 *
 * @return Returns the option name.
 **/
[[nodiscard]] TFTP_EXPORT std::string TftpOptions_name(
  KnownOptions option ) noexcept;

/**
 * @brief Returns a string, which describes the TFTP Options.
 *
 * This operation is used for debugging and information purposes.
 *
 * @param[in] options
 *   TFTP Options.
 *
 * @return Options Description.
 * @retval `(NONE)`
 *   When @p options is empty.
 **/
[[nodiscard]] TFTP_EXPORT std::string TftpOptions_toString(
  const TftpOptions &options );

/**
 * @brief Outputs @p options to @p stream.
 *
 * @param[in,out] stream
 *   Output Stream.
 * @param[in] options
 *   TFTP Options.
 *
 * @return @p stream
 **/
TFTP_EXPORT std::ostream& operator<<(
  std::ostream &stream,
  const TftpOptions &options );

}

#endif
