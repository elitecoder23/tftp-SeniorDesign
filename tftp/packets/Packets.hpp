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

//! Raw Options.
using RawOptions = std::vector< uint8_t>;
using RawOptionsSpan = std::span< const uint8_t>;


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

}

#endif
