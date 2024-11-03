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
 * @brief Definition of Module Tftp::Packets Options.
 **/

#ifndef TFTP_PACKETS_OPTIONS_HPP
#define TFTP_PACKETS_OPTIONS_HPP

#include <tftp/packets/Packets.hpp>

#include <concepts>
#include <limits>
#include <optional>
#include <string_view>

namespace Tftp::Packets {

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
[[nodiscard]] TFTP_EXPORT std::string Options_toString(
  const Options &options );

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
[[nodiscard]] TFTP_EXPORT Options Options_options( RawOptionsSpan rawOptions );

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
[[nodiscard]] TFTP_EXPORT RawOptions Options_rawOptions(
  const Options &options );

/**
 * @brief Decodes the Named Option.
 *
 * It extracts the option namen @p name from @p options and tries to convert it
 * to the given @p IntT.
 * Finally, the converted value is checked against the allowed ranges @p min and
 * @p max.
 *
 * The operation returns the option and if the *basic* option negotiation was
 * successful.
 *
 * - Option not found -> @p { true, {} }
 * - Option value empty or invalid -> @p { false, {} }
 * - Option value not in range of @p min and @p max -> @p { false, {} }
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
 **/
template< std::unsigned_integral IntT >
[[nodiscard]] std::pair< bool, std::optional< IntT > > Options_getOption(
  Options &options,
  std::string_view name,
  IntT min = std::numeric_limits< IntT >::min(),
  IntT max = std::numeric_limits< IntT >::max() );

}

#include <tftp/packets/Options.ipp>

#endif
