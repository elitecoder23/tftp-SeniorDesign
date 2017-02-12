/*
 * $Date$
 * $Revision$
 */
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::Packets::AcknowledgementPacket.
 **/

#ifndef TFTP_PACKETS_ACKNOWLEDGEMENTPACKET_HPP
#define TFTP_PACKETS_ACKNOWLEDGEMENTPACKET_HPP

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/Packet.hpp>
#include <tftp/packets/BlockNumber.hpp>

namespace Tftp {
namespace Packets {

/**
 * @brief TFTP Acknowledgement package (ACK).
 *
 * The TFTP Acknowledgement packet acknowledges a received WRQ packet without
 * options negotiation or an data packet.
 *
 * | ACK | Block # |
 * |:---:|:-------:|
 * | 2 B |   2 B   |
 **/
class AcknowledgementPacket: public Packet
{
  public:
    /**
     * @brief Creates the packet with the given block number.
     *
     * If the blockNumber parameter is suppressed, a default block number is
     * chosen.
     *
     * @param[in] blockNumber
     *   Block number of packet. Defaults to the BlockNumber default
     *   constructor.
     **/
    AcknowledgementPacket(
      const BlockNumber blockNumber = BlockNumber()) noexcept;

    /**
     * @brief Generates a TFTP acknowledgement packet from a data buffer.
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When rawPacket is not an valid packet.
     **/
    AcknowledgementPacket( const RawTftpPacketType &rawPacket);

    /**
     * @brief Returns the block number.
     *
     * @return The block number.
     **/
    BlockNumber getBlockNumber() const;

    /**
     * @brief Sets the block number of the packet.
     *
     * @param[in] blockBumber
     *   Block number of packet.
     **/
    void setBlockNumber( const BlockNumber blockBumber);

    //! @copydoc Packet::encode()
    virtual RawTftpPacketType encode() const override final;

    //! @copydoc Packet::toString()
    virtual string toString() const override final;

  private:
    //! Block number of the packet
    BlockNumber blockNumber;
};

}
}

#endif
