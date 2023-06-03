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
 * @brief Declaration of Class Tftp::Packets::ReadRequestPacket.
 **/

#ifndef TFTP_PACKETS_READREQUESTPACKET_HPP
#define TFTP_PACKETS_READREQUESTPACKET_HPP

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/ReadWriteRequestPacket.hpp>

#include <string_view>
#include <string>

namespace Tftp::Packets {

/**
 * @brief TFTP Read Request %Packet (RRQ).
 **/
class TFTP_EXPORT ReadRequestPacket : public ReadWriteRequestPacket
{
  public:
    /**
     * @brief Creates a TFTP Read Request Packet.
     *
     * @param[in] filename
     *   filename, which will be requested
     * @param[in] mode
     *   The transfer mode
     * @param[in] options
     *   Options, which are set
     **/
    ReadRequestPacket(
      std::string filename,
      TransferMode mode,
      Options options ) noexcept;

    /**
     * @brief Generates a TFTP Read Request packet from a data buffer
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When rawPacket is not an valid packet.
     **/
    explicit ReadRequestPacket( ConstRawTftpPacketSpan rawPacket );

    /**
     * @brief Assigns a Raw Packet to this Packet.
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @return *this
     **/
    ReadRequestPacket& operator=( ConstRawTftpPacketSpan rawPacket );
};

}

#endif
