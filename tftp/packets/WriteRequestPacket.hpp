/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::Packets::WriteRequestPacket.
 **/

#ifndef TFTP_PACKETS_WRITEREQUESTPACKET_HPP
#define TFTP_PACKETS_WRITEREQUESTPACKET_HPP

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/ReadWriteRequestPacket.hpp>

namespace Tftp {
namespace Packets {

/**
 * @brief TFTP Write Request packet (WRQ).
 **/
class WriteRequestPacket: public ReadWriteRequestPacket
{
  public:
    /**
     * @brief Creates a write request packet with the given data.
     *
     * @param[in] filename
     *   The filename, which will be requested
     * @param[in] mode
     *   The transfer mode
     * @param[in] options
     *   The options, which are set
     **/
    WriteRequestPacket(
      const string &filename,
      TransferMode mode,
      const Options::OptionList &options) noexcept;

    /**
     * @brief Generates a TFTP Write Request packet from a data buffer
     *
     * @param[in] rawPacket
     *   rawPacket, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When the raw data does not represent a valid write request packet
     **/
    WriteRequestPacket( const RawTftpPacket &rawPacket);
};

}
}

#endif
