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

namespace Tftp::Packets {

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
    explicit AcknowledgementPacket( BlockNumber blockNumber = {}) noexcept;

    /**
     * @brief Generates a TFTP acknowledgement packet from a data buffer.
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When rawPacket is not an valid packet.
     **/
    explicit AcknowledgementPacket( const RawTftpPacket &rawPacket);

    // @copydoc Packet::operator=(const RawTftpPacket&)
    AcknowledgementPacket& operator=( const RawTftpPacket &rawPacket) final;

    /**
     * @brief Returns the block number.
     *
     * @return The block number.
     **/
    BlockNumber blockNumber() const;

    /**
     * @brief Sets the block number of the packet.
     *
     * @param[in] blockBumber
     *   Block number of packet.
     **/
    void blockNumber( BlockNumber blockBumber);

    // @copydoc Packet::operator std::string() const
    operator std::string() const final;

  private:
    //! @copydoc Packet::encode() const
    RawTftpPacket encode() const final;

    /**
     * @brief Decodes the TFTP body.
     *
     * @param[in] rawPacket
     *   Raw TFP packet
     *
     * @throw InvalidPacketException
     *   If data or packet is invalid.
     **/
    void decodeBody( const RawTftpPacket &rawPacket);

    //! Block number of the packet
    BlockNumber blockNumberValue;
};

}

#endif
