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
#include <concepts>
#include <limits>

namespace Tftp::Packets {

/**
 * @brief Returns the Option Name String for the given Option.
 *
 * @param[in] option
 *   TFTP known Option.
 *
 * @return Returns the option name.
 **/
std::string TftpOptions_name( KnownOptions option ) noexcept;

/**
 * @brief Returns a string, which describes the option list.
 *
 * This operation is used for debugging and information purposes.
 *
 * The format is `{Name:Value};`.
 *
 * @param[in] options
 *   TFTP Options.
 *
 * @return Options Description.
 * @retval `(NONE)`
 *   When @p options is empty.
 **/
std::string TftpOptions_toString( const Options &options );

/**
 * @brief Decodes Options from the given Raw Data.
 *
 * @param[in] rawOptions
 *   Raw Options
 *
 * @return Decoded Options.
 *
 * @throw InvalidPacketException
 *   On invalid input data.
 *
 * @sa TftpOptions_rawOptions()
 **/
Options TftpOptions_options( RawOptionsSpan rawOptions );

/**
 * @brief Returns the Option List as Raw Data.
 *
 * The raw option date is used to generate the option data within the
 * TFTP packages.
 *
 * @param[in] options
 *   The TFTP Options to convert.
 *
 * @return TFTP Options as raw data
 *
 * @sa TftpOptions_options()
 **/
RawOptions TftpOptions_rawOptions( const Options &options );

/**
 * @brief Decodes the Named Option.
 *
 * @tparam IntT
 *   Unsigned Integer Type.
 *
 * @param[in,out] options
 *   TFTP Options
 * @param[in] name
 *   Option Name
 * @param[in] min
 *   Minimum allowed Value
 * @param[in] max
 *   Maximum allowed Value
 *
 * @return std::pair Option was valid (not present or decoded correctly) and
 *   Option Value
 */
template< std::unsigned_integral IntT >
std::pair< bool, std::optional< IntT > > TftpOptions_getOption(
  Options &options,
  const std::string &name,
  IntT min = std::numeric_limits< IntT >::min(),
  IntT max = std::numeric_limits< IntT >::max() );

}

#include "TftpOptions.ipp"

#endif
