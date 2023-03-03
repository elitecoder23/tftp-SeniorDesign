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

#include <tftp/packets/Packets.hpp>

#include <string>
#include <optional>
#include <iosfwd>

namespace Tftp::Packets {

/**
 * @brief Decoded TFTP Options
 *
 * Used to store all known TFTP Options like:
 * - blocksize,
 * - timeout, and
 * - transfer size.
 **/
struct TFTP_EXPORT TftpOptions
{
  //! Block size option
  std::optional< uint16_t > blockSize;
  //! Timeout option
  std::optional< uint8_t > timeout;
  //! Transfer size option
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
std::string TFTP_EXPORT TftpOptions_name( KnownOptions option ) noexcept;

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
std::string TFTP_EXPORT TftpOptions_toString( const TftpOptions &options );

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
TFTP_EXPORT std::ostream&
operator<<( std::ostream &stream, const TftpOptions &options );

}

#endif
