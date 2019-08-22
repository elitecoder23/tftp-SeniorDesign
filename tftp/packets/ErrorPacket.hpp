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

namespace Tftp::Packets {

/**
 * @brief TFTP Error packet (ERR).
 *
 * A TFTP error code consists of an error code and a user readable error
 * message.
 *
 * | ERR | ErrorCode | ErrMsg |  0  |
 * |:---:|:---------:|:------:|:---:|
 * | 2 B |    2 B    |  str   | 1 B |
 *
 **/
class ErrorPacket: public Packet
{
  public:
    /**
     * @brief Generates a TFTP error packet with the given error code and
     *   error message.
     *
     * @param[in] errorCode
     *   Error code, which shall be set.
     * @param[in] errorMessage
     *   Error message, which shall be set. By default empty.
     **/
    ErrorPacket(
      ErrorCode errorCode,
      const std::string &errorMessage);

    //! @copydoc ErrorPacket(ErrorCode,const std::string&)
    explicit ErrorPacket(
      ErrorCode errorCode,
      std::string &&errorMessage = {});

    /**
     * @brief Generates a TFTP error packet from a data buffer
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When rawPacket is not an valid packet.
     **/
    explicit ErrorPacket( const RawTftpPacket &rawPacket);

    //! @copydoc Packet::operator=(const RawTftpPacket&)
    ErrorPacket& operator=( const RawTftpPacket &rawPacket) final;

    // @copydoc Packet::operator std::string() const
    operator std::string() const final;

    /**
     * @brief Returns the error code.
     *
     * @return The error code.
     **/
    ErrorCode errorCode() const noexcept;

    /**
     * @brief Sets the error code.
     *
     * @param[in] errorCode
     *   The error code to set
     **/
    void errorCode( ErrorCode errorCode) noexcept;

    /**
     * @brief Returns the error message of this packet.
     *
     * @return The error message
     **/
    const std::string& errorMessage() const;

    /**
     * @brief Sets the error message of this packet.
     *
     * @param[in] errorMessage
     *   The error message to set.
     **/
    void errorMessage( const std::string &errorMessage);

    //! @copydoc errorMessage(const std::string&)
    void errorMessage( std::string &&errorMessage);

  private:
    //! @copydoc Packet::encode()
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

    //! The error code
    ErrorCode errorCodeValue;
    //! The error message.
    std::string errorMessageValue;
};

}

#endif
