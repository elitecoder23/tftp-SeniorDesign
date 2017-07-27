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
 * @brief Declaration of class Tftp::Packets::Packet.
 **/

#ifndef TFTP_PACKETS_PACKET_HPP
#define TFTP_PACKETS_PACKET_HPP

#include <tftp/packets/Packets.hpp>

#include <string>

namespace Tftp {
namespace Packets {

/**
 * @brief Base-class of all TFTP packets.
 **/
class Packet
{
  public:
    //! string type
    using string = std::string;

    //! The minimum size is the Opcode field.
    static constexpr std::size_t HeaderSize = sizeof( uint16_t);

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

    /**
     * @brief default copy constructor
     *
     * @param[in] other
     *   Other
     *
     * @return *this
     **/
    Packet( const Packet &other) = default;

    /**
     * @brief default move constructor
     *
     * @param[in] other
     *   Other
     *
     * @return *this
     **/
    Packet( Packet &&other) = default;

    //! Default destructor
    virtual ~Packet() noexcept = default;

    /**
     * @brief default copy assignment operator
     *
     * @param[in] other
     *   Other
     *
     * @return *this
     **/
    Packet& operator=( const Packet &other);

    /**
     * @brief default move assignment operator
     *
     * @param[in] other
     *   Other
     *
     * @return *this
     **/
    Packet& operator=( Packet &&other);

    /**
     * @brief Assigns a raw packet to this packet.
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @return *this
     **/
    virtual Packet& operator=( const RawTftpPacketType &rawPacket);

    /**
     * @brief Get the binary representation of the packet.
     *
     * The data is used to transmit the package over the network.
     *
     * @return Binary packet data
     **/
    operator RawTftpPacketType() const;

    /**
     * @brief Returns a string, which describes the packet.
     *
     * This operation is used for debugging and information purposes.
     *
     * @return Packet description.
     **/
    virtual operator string() const;

  protected:
    /**
     * @brief Constructs a TFTP package with the given packet type.
     *
     * @param[in] packetType
     *   The packet type of the packet.
     **/
    Packet( PacketType packetType) noexcept;

    /**
     * @brief Generates a basic TFTP packet from a data buffer
     *
     * Only decodes TFTP header.
     *
     * @param[in] packetType
     *   The packet type of the packet.
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     **/
    Packet(
      PacketType packetType,
      const RawTftpPacketType &rawPacket);

    /**
     * @brief Get the binary representation of the packet.
     *
     * The data is used to transmit the package over the network.
     *
     * @return Binary packet data
     **/
    virtual RawTftpPacketType encode() const = 0;

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
    /**
     * @brief Decodes the TFTP header.
     *
     * Checks the packet size
     * Checks the packet type against the opcode.
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     **/
    void decodeHeader( const RawTftpPacketType &rawPacket);

    //! The TFTP Packet type
    const PacketType packetType;
};

}
}

#endif
