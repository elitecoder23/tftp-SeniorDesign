/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::Packets::ReadRequestPacket.
 **/

#ifndef TFTP_PACKETS_READREQUESTPACKET_HPP
#define TFTP_PACKETS_READREQUESTPACKET_HPP

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/ReadWriteRequestPacket.hpp>

namespace Tftp {
namespace Packets {

/**
 * @brief TFTP Read Request packet (RRQ).
 **/
class ReadRequestPacket: public ReadWriteRequestPacket
{
  public:
    /**
     * @brief Creates a TFTP Read Request packet.
     *
     * @param[in] filename
     * @param[in] mode
     * @param[in] options
     */
    ReadRequestPacket(
      const string &filename,
      TransferMode mode,
      const Options::OptionList &options) noexcept;

    /**
     * @brief Generates a TFTP Read Request packet from a data buffer
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When rawPacket is not an valid packet.
     **/
    ReadRequestPacket( const RawTftpPacketType &rawPacket);
};

}
}

#endif
