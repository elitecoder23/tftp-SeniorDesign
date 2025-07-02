// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Packets::AcknowledgementPacket.
 **/

#ifndef TFTP_PACKETS_ACKNOWLEDGEMENTPACKET_HPP
#define TFTP_PACKETS_ACKNOWLEDGEMENTPACKET_HPP

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/BlockNumber.hpp>
#include <tftp/packets/Packet.hpp>

namespace Tftp::Packets {

/**
 * @brief TFTP Acknowledgement %Package (ACK).
 *
 * The TFTP Acknowledgement packet acknowledges a received WRQ packet without
 * options negotiation or a data packet.
 *
 * | ACK | Block # |
 * |:---:|:-------:|
 * | 2 B |   2 B   |
 **/
class TFTP_EXPORT AcknowledgementPacket final : public Packet
{
  public:
    //! Packet Size ( Opcode + Block Number)
    static constexpr auto PacketSize{ HeaderSize + 2UZ };

    /**
     * @brief Creates the packet with the given block number.
     *
     * If the @p blockNumber parameter is suppressed, a default block number is
     * chosen.
     *
     * @param[in] blockNumber
     *   Block number of packet.
     *   Defaults to the BlockNumber default constructor.
     **/
    explicit AcknowledgementPacket( BlockNumber blockNumber = {} ) noexcept;

    /**
     * @brief Generates a TFTP acknowledgement packet from a data buffer.
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When rawPacket is not a valid packet.
     **/
    explicit AcknowledgementPacket( Helper::ConstRawDataSpan rawPacket );

    /**
     * @brief Assigns a Raw Packet to this Packet.
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @return *this
     **/
    AcknowledgementPacket& operator=( Helper::ConstRawDataSpan rawPacket );

    /**
     * @name Block Number
     * @{
     **/

    /**
     * @brief Returns the Block Number.
     *
     * @return The block number.
     **/
    [[nodiscard]] BlockNumber blockNumber() const;

    /**
     * @brief Sets the block number of the packet.
     *
     * @param[in] blockNumber
     *   Block number of packet.
     **/
    void blockNumber( BlockNumber blockNumber );

    //! @}

    // @copydoc Packet::operator std::string() const
    explicit operator std::string() const override;

  private:
    //! @copydoc Packet::encode() const
    [[nodiscard]] Helper::RawData encode() const override;

    /**
     * @brief Decodes the TFTP Body.
     *
     * @param[in] rawPacket
     *   Raw TFP packet
     *
     * @throw InvalidPacketException
     *   If data or packet is invalid.
     * @throw InvalidPacketException
     *   When packet size is invalid
     **/
    void decodeBody( Helper::ConstRawDataSpan rawPacket );

    //! Block Number of Packet
    BlockNumber blockNumberV;
};

}

#endif
