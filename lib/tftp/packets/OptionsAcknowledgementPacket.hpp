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
 * @brief Declaration of Class Tftp::Packets::OptionsAcknowledgementPacket.
 **/

#ifndef TFTP_PACKETS_OPTIONSACKNOWLEDGEMENTPACKET_HPP
#define TFTP_PACKETS_OPTIONSACKNOWLEDGEMENTPACKET_HPP

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/Packet.hpp>

#include <map>

namespace Tftp::Packets {


/**
 * @brief TFTP Option Acknowledgement %Packet (ACK).
 *
 * This package is only sent from the TFTP server in response to an RRQ or
 * WRQ packet from the client, a TFTP option is transmitted within the request
 * packet and the server accepts at least one option.
 *
 * | OACK | Opt 1 |  0  | OptV 1 |  0  | ... | Opt N |  0  | OptV N |  0  |
 * |:----:|:-----:|:---:|:------:|:---:|:---:|:-----:|:---:|:------:|:---:|
 * | 2 B  |  str  | 1 B |  str   | 1 B |     |  str  | 1 B |  str   | 1 B |
 *
 **/
class TFTP_EXPORT OptionsAcknowledgementPacket final : public Packet
{
  public:
    /**
     * @brief Initialises TFTP Options Acknowledgement Packet with the given
     *   Options List.
     *
     * @param[in] options
     *   TFTP Options List.
     **/
    explicit OptionsAcknowledgementPacket( Options options ) noexcept;

    /**
     * @brief Generates a TFTP Options Acknowledgement packet from a data buffer
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When rawPacket is not an valid packet.
     **/
    explicit OptionsAcknowledgementPacket( ConstRawTftpPacketSpan rawPacket );

    /**
     * @brief Assigns a Raw Packet to this Packet.
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @return *this
     **/
    OptionsAcknowledgementPacket& operator=( ConstRawTftpPacketSpan rawPacket );

    /**
     * @name TFTP Options
     * @{
     **/

    /**
     * @brief Returns the options within the packet (const reference)
     *
     * @return Option list (as const reference)
     **/
    [[nodiscard]] const Options& options() const;

    /**
     * @brief Returns the options within the packet (as reference)
     *
     * @return Option list (as reference)
     **/
    [[nodiscard]] Options& options();

    /**
     * @brief Overwrite the options of this packet with the new one.
     *
     * @param[in] options
     *   New option list, which shall be used.
     **/
    void options( Options options );

    /** @} **/

    // @copydoc Packet::operator std::string() const
    explicit operator std::string() const override;

  private:
    /**
     * @copydoc Packet::encode()
     **/
    [[nodiscard]] RawTftpPacket encode() const override;

    /**
     * @brief Decodes the TFTP body.
     *
     * @param[in] rawPacket
     *   Raw TFP packet
     *
     * @throw InvalidPacketException
     *   If data or packet is invalid.
     **/
    void decodeBody( ConstRawTftpPacketSpan rawPacket );

    //! Stored Options.
    Options optionsV;
};

}

#endif
