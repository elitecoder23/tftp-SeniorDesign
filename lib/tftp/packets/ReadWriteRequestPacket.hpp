// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Packets::ReadWriteRequestPacket.
 **/

#ifndef TFTP_PACKETS_READWRITEREQUESTPACKET_HPP
#define TFTP_PACKETS_READWRITEREQUESTPACKET_HPP

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/Packet.hpp>

#include <map>
#include <string>
#include <string_view>

namespace Tftp::Packets {

/**
 * @brief TFTP Read-/ Write- Request %Packet.
 *
 * | RRQ/WRQ | FName |  0  | Mode |  0  | Opt 1 |  0  | OptV 1 |  0  | ... | Opt N |  0  | OptV N |  0  |
 * |:-------:|:-----:|:---:|:----:|:---:|:-----:|:---:|:------:|:---:|:---:|:-----:|:---:|:------:|:---:|
 * |   2 B   |  str  | 1 B | str  | 1 B |  str  | 1 B |   str  | 1 B |     |  str  | 1 B |  str   | 1 B |
 **/
class TFTP_EXPORT ReadWriteRequestPacket : public Packet
{
  public:
    /**
     * @brief Converts the mode enumeration to the corresponding packet
     *   string.
     *
     * @param[in] mode
     *   Transfer mode.
     *
     * @return The corresponding string representation.
     * @retval string()
     *   When [mode] is not a valid transfer mode.
     **/
    [[nodiscard]] static std::string_view decodeMode( TransferMode mode );

    /**
     * @brief Converts the Mode String to the Corresponding Enumeration.
     *
     * @param[in] mode
     *   Transfer Mode.
     *
     * @return The corresponding mode.
     * @retval TransferMode::Invalid
     *   When @p mode is not a valid transfer mode.
     **/
    [[nodiscard]] static TransferMode decodeMode( std::string_view mode );

    /**
     * @name Filename
     *
     * @{
     **/

    /**
     * @brief Returns the request filename.
     *
     * @return Filename
     **/
    [[nodiscard]] std::string_view filename() const;

    /**
     * @brief Sets the filename
     *
     * @param[in] filename
     *   New filename.
     **/
    void filename( std::string filename );

    /** @} **/

    /**
     * @name Transfer Mode
     * @{
     **/

    /**
     * @brief Returns the Transfer Mode.
     *
     * @return Transfer Mode.
     **/
    [[nodiscard]] TransferMode mode() const;

    /**
     * @brief Sets the Transfer Mode.
     *
     * @param[in] mode
     *   New transfer mode.
     *
     * @throw TftpPacketException
     *   When mode is not a valid transfer mode.
     **/
    void mode( TransferMode mode );

    /** @} **/

    /**
     * @name TFTP Options
     * @{
     **/

    /**
     * @brief Returns the set TFTP options.
     *
     * @return The TFTP options.
     **/
    [[nodiscard]] const Options& options() const;

    /**
     * @brief Returns the set TFTP options.
     *
     * @return TFTP options.
     **/
    [[nodiscard]] Options& options();

    /**
     * @brief Sets the TFTP options.
     *
     * @param[in] options
     *   TFTP options.
     **/
    void options( Options options );

    /** @} **/

    // @copydoc Packet::operator std::string() const
    explicit operator std::string() const override;

  protected:
    /**
     * @brief Creates a read/ write request packet with the given data.
     *
     * @param[in] packetType
     *   Packet type of the packet. Valid are RRQ and WRQ
     * @param[in] filename
     *   Filename, which will be requested
     * @param[in] mode
     *   Transfer mode
     * @param[in] options
     *   Options, which are set
     *
     * @throw InvalidPacketException
     *   When packetType is not valid.
     **/
    ReadWriteRequestPacket( PacketType packetType, std::string filename, TransferMode mode, Options options );

    /**
     * @brief Generates a TFTP Read/ Write Request packet from a data buffer
     *
     * @param[in] packetType
     *   The type of the packet. Only READ_REQUEST or
     *   WRITE_REQUEST is allowed.
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When rawPacket is not a valid packet.
     **/
    ReadWriteRequestPacket( PacketType packetType, Helper::ConstRawDataSpan rawPacket );

    /**
     * @brief Decodes the TFTP body.
     *
     * @param[in] rawPacket
     *   Raw TFP packet
     *
     * @throw InvalidPacketException
     *   If data or packet is invalid.
     **/
    void decodeBody( Helper::ConstRawDataSpan rawPacket );

  private:
    /**
     * @copydoc Packet::encode()
     **/
    [[nodiscard]] Helper::RawData encode() const final;

    //! Filename
    std::string filenameV;
    //! Transfer Mode
    TransferMode modeV;
    //! Stored Options.
    Options optionsV;
};

}

#endif
