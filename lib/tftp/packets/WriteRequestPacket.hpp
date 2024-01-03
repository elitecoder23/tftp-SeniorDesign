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
 * @brief Declaration of Class Tftp::Packets::WriteRequestPacket.
 **/

#ifndef TFTP_PACKETS_WRITEREQUESTPACKET_HPP
#define TFTP_PACKETS_WRITEREQUESTPACKET_HPP

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/ReadWriteRequestPacket.hpp>

namespace Tftp::Packets {

/**
 * @brief TFTP Write Request %Packet (WRQ).
 **/
class TFTP_EXPORT WriteRequestPacket final : public ReadWriteRequestPacket
{
  public:
    /**
     * @brief Creates a write request packet with the given data.
     *
     * @param[in] filename
     *   Filename, which will be requested
     * @param[in] mode
     *   Transfer mode
     * @param[in] options
     *   Options, which are set
     **/
    WriteRequestPacket(
      std::string filename,
      TransferMode mode,
      Options options ) noexcept;

    /**
     * @brief Generates a TFTP Write Request packet from a data buffer
     *
     * @param[in] rawPacket
     *   rawPacket, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When the raw data does not represent a valid write request packet
     **/
    explicit WriteRequestPacket( ConstRawTftpPacketSpan rawPacket );

    /**
     * @brief Assigns a Raw Packet to this Packet.
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @return *this
     **/
    WriteRequestPacket& operator=( ConstRawTftpPacketSpan rawPacket );
};

}

#endif
