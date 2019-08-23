/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Packets::Packet.
 **/

#ifndef TFTP_PACKETS_PACKET_HPP
#define TFTP_PACKETS_PACKET_HPP

#include <tftp/packets/Packets.hpp>

#include <string>

namespace Tftp::Packets {

/**
 * @brief Base-class of all TFTP packets.
 **/
class Packet
{
  public:
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
    static PacketType packetType( const RawTftpPacket &rawPacket) noexcept;

    /**
     * @brief Return the packet type of the TFTP Packet.
     *
     * @return Packet type.
     **/
    [[nodiscard]] PacketType packetType() const noexcept;

    /**
     * @brief Copy Constructor
     *
     * @param[in] other
     *   Other
     *
     * @return *this
     **/
    Packet( const Packet &other) = default;

    /**
     * @brief Move Constructor
     *
     * @param[in] other
     *   Other
     *
     * @return *this
     **/
    Packet( Packet &&other) = default;

    //! Destructor
    virtual ~Packet() noexcept = default;

    /**
     * @brief Copy assignment operator
     *
     * @param[in] other
     *   Other
     *
     * @return *this
     **/
    Packet& operator=( const Packet &other);

    /**
     * @brief Move assignment operator
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
    virtual Packet& operator=( const RawTftpPacket &rawPacket);

    /**
     * @brief Get the binary representation of the packet.
     *
     * The data is used to transmit the package over the network.
     *
     * @return Binary packet data
     **/
    explicit operator RawTftpPacket() const;

    /**
     * @brief Returns a string, which describes the packet.
     *
     * This operation is used for debugging and information purposes.
     *
     * @return Packet description.
     **/
    explicit virtual operator std::string() const;

  protected:
    /**
     * @brief Constructs a TFTP package with the given packet type.
     *
     * @param[in] packetType
     *   The packet type of the packet.
     **/
    explicit Packet( PacketType packetType) noexcept;

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
    Packet( PacketType packetType, const RawTftpPacket &rawPacket);

    /**
     * @brief Get the binary representation of the packet.
     *
     * The data is used to transmit the package over the network.
     *
     * @return Binary packet data
     **/
    [[nodiscard]] virtual RawTftpPacket encode() const = 0;

    /**
     * @brief Insert the header data to the raw packet.
     *
     * Can be used by child classes to assemble a raw packet.
     *
     * @param[in,out] rawPacket
     *   The raw packet, which will be filled with the approbate data.
     **/
    void insertHeader( RawTftpPacket &rawPacket) const;

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
    void decodeHeader( const RawTftpPacket &rawPacket);

    //! TFTP Packet Type
    const PacketType packetTypeValue;
};

}

#endif
