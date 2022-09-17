/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Tftp::Packets.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Tftp::Packets.
 **/

#ifndef TFTP_PACKETS_PACKETS_HPP
#define TFTP_PACKETS_PACKETS_HPP

#include <tftp/Tftp.hpp>

#include <vector>
#include <span>
#include <cstdint>
#include <optional>
#include <concepts>
#include <limits>

/**
 * @brief TFTP %Packets.
 *
 * The base for the packets is the class TftpPacket.
 * For raw-data to TFTP packet handling, the class PacketFactory with its
 * static methods can be used.
 **/
namespace Tftp::Packets {

class Packet;
class ReadRequestPacket;
class WriteRequestPacket;
class DataPacket;
class AcknowledgementPacket;
class ErrorPacket;
class OptionsAcknowledgementPacket;

class BlockNumber;

class PacketHandler;

//! Raw TFTP Packet.
using RawTftpPacket = std::vector< uint8_t>;
//! Raw TFTP Packet as std::span
using RawTftpPacketSpan = std::span< uint8_t>;
//! Constant Raw TFTP Packet as std::span
using ConstRawTftpPacketSpan = std::span< const uint8_t>;

//! Raw Options.
using RawOptions = std::vector< uint8_t>;
//! Constant Raw TFTP Options as std::span
using RawOptionsSpan = std::span< const uint8_t>;

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
 * @param[in] options
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
  const Options &options,
  std::string_view name,
  IntT min = std::numeric_limits< IntT >::min(),
  IntT max = std::numeric_limits< IntT >::max() );

}

#include <tftp/packets/Packets.ipp>

#endif
