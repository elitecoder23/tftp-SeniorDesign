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
 * @brief Declaration of class Tftp::Packets::ErrorPacket.
 **/

#ifndef TFTP_PACKETS_ERRORPACKET_HPP
#define TFTP_PACKETS_ERRORPACKET_HPP

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/Packet.hpp>

#include <string>

namespace Tftp {
namespace Packets {

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
      ErrorCode errorCode = ErrorCode::Invalid,
      const string &errorMessage = string()) noexcept;

    /**
     * @brief Generates a TFTP error packet from a data buffer
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     *
     * @throw InvalidPacketException
     *   When rawPacket is not an valid packet.
     **/
    ErrorPacket( const RawTftpPacketType &rawPacket);

    //! @copydoc Packet::operator=(const RawTftpPacketType&)
    virtual ErrorPacket& operator=(
      const RawTftpPacketType &rawPacket) override;

    //! @copydoc Packet::operator string() const
    virtual operator string() const override;

    /**
     * @brief Returns the error code.
     *
     * @return The error code.
     **/
    ErrorCode getErrorCode() const;

    /**
     * @brief Sets the error code.
     *
     * @param[in] errorCode
     *   The error code to set
     **/
    void setErrorCode( ErrorCode errorCode);

    /**
     * @brief Returns the error message of this packet.
     *
     * @return The error message
     **/
    string getErrorMessage() const;

    /**
     * @brief Sets the error message of this packet.
     *
     * @param[in] errorMessage
     *   The error message to set. By default empty.
     **/
    void setErrorMessage( const string &errorMessage = {});

  private:
    //! @copydoc Packet::encode()
    virtual RawTftpPacketType encode() const override;

    void decodeBody( const RawTftpPacketType &rawPacket);

    //! The error code
    ErrorCode errorCode;
    //! The error message.
    string errorMessage;
};

}
}

#endif
