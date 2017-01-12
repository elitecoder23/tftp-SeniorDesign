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
 * @brief Declaration of class Tftp::Packet::TftpPacket.
 **/

#ifndef TFTP_PACKET_TFTPPACKET_HPP
#define TFTP_PACKET_TFTPPACKET_HPP

#include <tftp/packet/Packet.hpp>

#include <string>

namespace Tftp {
namespace Packet {

/**
 * @brief Base-class of all TFTP packets.
 **/
class TftpPacket
{
  public:
    using string = std::string;

    //! The minimum size is the Opcode field.
    static constexpr std::size_t TFTP_PACKET_HEADER_SIZE = sizeof(uint16_t);

    /**
     * @brief Decodes the packet type of a raw buffer.
     *
     * @param[in] rawPacket
     *   Raw buffer of received packet.
     *
     * @return The TFTP packet type of the raw buffer.
     * @retval PacketType::INVALID
     *   If packet is to small or invalid opcode value.
     **/
    static PacketType getPacketType(
      const RawTftpPacketType &rawPacket) noexcept;

    /**
     * @brief Return the packet type of the TFTP packet.
     *
     * @return The packet type.
     **/
    PacketType getPacketType() const;

    // virtual void decode( const RawTftpPacketType &rawPacket);

    /**
     * @brief Get the binary representation of the packet.
     *
     * The data is used to transmit the package over the network.
     *
     * @return Binary packet data
     **/
    virtual RawTftpPacketType encode() const = 0;

    //! default copy constructor
    TftpPacket( const TftpPacket &other) = default;

    //! default move constructor
    TftpPacket( TftpPacket &&other) = default;

    //! default copy assignment operator
    TftpPacket& operator=( const TftpPacket &other) = default;

    //! default copy move operator
    TftpPacket& operator=( TftpPacket &&other) = default;

    //! Default destructor
    virtual ~TftpPacket() noexcept = default;

    /**
     * @brief Returns a string, which describes the packet.
     *
     * This operation is used for debugging and information purposes.
     *
     * @return Packet description.
     **/
    virtual string toString() const;

  protected:
    /**
     * @brief Constructs a TFTP package with the given packet type.
     *
     * @param[in] packetType
     *   The packet type of the packet.
     **/
    TftpPacket( PacketType packetType) noexcept;

    /**
     * @brief Generates a basic TFTP packet from a data buffer
     *
     * Only decodes TFTP header.
     *
     * @param[in] expectedPacketType
     *   The expected packet type of the packet.
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When rawPacket is not an valid packet.
     **/
    TftpPacket(
      PacketType expectedPacketType,
      const RawTftpPacketType &rawPacket);

    /**
     * @brief Set the packet type of the TFTP packet
     *
     * This operation can be dangerous if used for derived classes, e.g. set the
     * packet type of an TFTP Write Request Packet to TFTP error packet.
     *
     * @param[in] packetType
     *   The new packet type
     **/
    void setPacketType( PacketType packetType);

    /**
     * @brief Insert the header data to the raw packet.
     *
     * Can be used by child classes to assemble a raw packet.
     *
     * @param[in,out] rawPacket
     *   The raw packet, which will be filled with the approbate data.
     **/
    void insertHeader( RawTftpPacketType &rawPacket) const;

  private:
    //! The TFTP Packet type
    PacketType packetType;
};

}
}

#endif
