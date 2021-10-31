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
#include <limits>
#include <cstdint>
#include <optional>
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
 * @brief Returns the option string for the given option.
 *
 * @param[in] option
 *   The TFTP option.
 *
 * @return Returns the option name.
 **/
std::string_view TftpOptions_name( KnownOptions option ) noexcept;

/**
 * @brief Returns a string, which describes the option list.
 *
 * This operation is used for debugging and information purposes.
 *
 * @param[in] options
 *   TFTP Options.
 *
 * @return Option list description.
 **/
std::string TftpOptions_toString( const Options &options );

/**
 * @brief Decodes Options from the given raw data.
 *
 * @param[in] rawOptions
 *   Raw Options
 *
 * @return Decoded Options.
 *
 * @throw InvalidPacketException
 *   On invalid input data
 **/
Options TftpOptions_options( RawOptionsSpan rawOptions );

/**
 * @brief Returns the option list as raw data
 *
 * The raw option date is used to generate the option data within the
 * TFTP packages.
 *
 * @param[in] options
 *   The TFTP Options to convert.
 *
 * @return TFTP Options as raw data
 **/
RawOptions TftpOptions_rawOptions( const Options &options );

/**
 * @brief Get the Named Option with given Value.
 *
 * @tparam IntT
 *   Integer Type.
 *
 * @param[in] option
 *   Option Name
 * @param value
 *   Option Value
 *
 * @return Named Option
 **/
template< typename IntT >
Options::value_type TftpOptions_setOption(
  KnownOptions option,
  IntT value );

/**
 * @brief Decodes the Named Option.
 *
 * @tparam IntT
 *   Integer Type.
 *
 * @param[in] options
 *   TFTP Options
 * @param[in] option
 *   Option Name
 *
 * @return std::pair Option was valid (not present or decoded correctly) and
 *   Option Value
 */
template< typename IntT >
std::pair< bool, std::optional< IntT > > TftpOptions_getOption(
  const Options &options,
  KnownOptions option );

template< typename IntT >
Options::value_type TftpOptions_setOption(
  KnownOptions option,
  IntT value )
{
  return {
    std::string{ TftpOptions_name( option ) },
    std::to_string( value ) };
}

template< typename IntT >
std::pair< bool, std::optional< IntT > > TftpOptions_getOption(
  const Options &options,
  KnownOptions option )
{
  auto optionIt{ options.find( TftpOptions_name( option ) ) };

  // option not set
  if ( optionIt == options.end() )
  {
    return { true, {} };
  }

  const auto optionValue{ std::stoull( optionIt->second ) };

  if ( optionValue > std::numeric_limits< IntT>::max() )
  {
    return { false, {} };
  }

  return { true, static_cast< IntT >( optionValue ) };
}

}

#endif
