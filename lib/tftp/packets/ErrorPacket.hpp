/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Packets::ErrorPacket.
 **/

#ifndef TFTP_PACKETS_ERRORPACKET_HPP
#define TFTP_PACKETS_ERRORPACKET_HPP

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/Packet.hpp>

#include <string>
#include <string_view>

namespace Tftp::Packets {

/**
 * @brief TFTP Error %Packet (ERR).
 *
 * A TFTP error code consists of an error code and a user readable error
 * message.
 *
 * | ERR | ErrorCode | ErrMsg |  0  |
 * |:---:|:---------:|:------:|:---:|
 * | 2 B |    2 B    |  str   | 1 B |
 *
 **/
class TFTP_EXPORT ErrorPacket : public Packet
{
  public:
    //! Minimum Header Size ( Opcode, Error Code, Empty String)
    static constexpr size_t MinPacketSize{ HeaderSize + 2U + 1U };

    /**
     * @brief Generates a TFTP error packet with the given error code and
     *   error message.
     *
     * @param[in] errorCode
     *   Error code, which shall be set.
     * @param[in] errorMessage
     *   Error message, which shall be set.
     *   By default, empty.
     **/
    explicit ErrorPacket(
      ErrorCode errorCode,
      std::string errorMessage = {} );

    /**
     * @brief Generates a TFTP error packet from a data buffer
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When rawPacket is not an valid packet.
     **/
    explicit ErrorPacket( ConstRawTftpPacketSpan rawPacket );

    /**
     * @brief Assigns a Raw Packet to this Packet.
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @return *this
     **/
    ErrorPacket& operator=( ConstRawTftpPacketSpan rawPacket );

    // @copydoc Packet::operator std::string() const
    explicit operator std::string() const final;

    /**
     * @name Error Code
     * @{
     **/

    /**
     * @brief Returns the error code.
     *
     * @return TFTP error code.
     **/
    [[nodiscard]] ErrorCode errorCode() const noexcept;

    /**
     * @brief Sets the error code.
     *
     * @param[in] errorCode
     *   TFTP error code to set
     **/
    void errorCode( ErrorCode errorCode ) noexcept;

    //! @}

    /**
     * @name Error Message
     * @{
     **/

    /**
     * @brief Returns the error message of this packet.
     *
     * @return TFTP error message
     **/
    [[nodiscard]] std::string_view errorMessage() const;

    /**
     * @brief Sets the error message of this packet.
     *
     * @param[in] errorMessage
     *   TFTP Error message to set.
     **/
    void errorMessage( std::string errorMessage );

    //! @}

  private:
    //! @copydoc Packet::encode()
    [[nodiscard]] RawTftpPacket encode() const final;

    /**
     * @brief Decodes the TFTP body.
     *
     * @param[in] rawPacket
     *   Raw TFP packet
     *
     * @throw InvalidPacketException
     *   If data or packet is invalid.
     * @throw InvalidPacketException
     *   When rawPacket is not an valid packet.
     * @throw InvalidPacketException
     *   When the error message is not 0-terminated.
     **/
    void decodeBody( ConstRawTftpPacketSpan rawPacket );

    //! Error Code
    ErrorCode errorCodeV;
    //! Error Message.
    std::string errorMessageV;
};

}

#endif
