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
 * @brief TFTP %Packet.
 **/
class Packet
{
  public:
    //! Minimum Size is the Opcode Field.
    static constexpr std::size_t HeaderSize{ sizeof( uint16_t )};

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
    static PacketType packetType( const RawTftpPacket &rawPacket ) noexcept;

    /**
     * @brief Return the packet type of the TFTP Packet.
     *
     * @return Packet type.
     **/
    [[nodiscard]] PacketType packetType() const noexcept;

    //! Destructor
    virtual ~Packet() noexcept = default;

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
    explicit Packet( PacketType packetType ) noexcept;

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
    Packet( PacketType packetType, const RawTftpPacket &rawPacket );

    // Defaulted Copy Constructor
    Packet( const Packet &other ) = default;

    // Defaulted Move Constructor
    Packet( Packet &&other ) = default;

    /**
     * @brief Copy Assignment Operator
     *
     * @param[in] other
     *   Other Packet
     *
     * @return *this
     **/
    Packet& operator=( const Packet &other ) noexcept;

    /**
     * @brief Move Assignment Operator
     *
     * @param[in] other
     *   Other Packet
     *
     * @return *this
     **/
    Packet& operator=( Packet &&other ) noexcept;

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
     * The size of @p rawPacket must be at least @p HeaderSize.
     *
     * @param[in,out] rawPacket
     *   The raw packet, which will be filled with the appropriate data.
     **/
    void insertHeader( RawTftpPacket &rawPacket ) const;

    /**
     * @brief Decodes the TFTP Header.
     *
     * Checks the packet size
     * Checks the packet type against the opcode.
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     **/
    void decodeHeader( const RawTftpPacket &rawPacket);

  private:
    //! TFTP Packet Type
    const PacketType packetTypeValue;
};

}

#endif
