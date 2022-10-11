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
using RawTftpPacket = std::vector< uint8_t >;
//! Raw TFTP Packet as std::span
using RawTftpPacketSpan = std::span< uint8_t >;
//! Constant Raw TFTP Packet as std::span
using ConstRawTftpPacketSpan = std::span< const uint8_t >;

//! Raw Options.
using RawOptions = std::vector< uint8_t >;
//! Constant Raw TFTP Options as std::span
using RawOptionsSpan = std::span< const uint8_t >;

}

#endif
