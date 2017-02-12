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
 * @brief Declaration of class Tftp::Packets::BaseErrorPacket.
 **/

#ifndef TFTP_PACKETS_BASEERRORPACKET_HPP
#define TFTP_PACKETS_BASEERRORPACKET_HPP

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/TftpPacket.hpp>

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
class BaseErrorPacket: public TftpPacket
{
  public:
    /**
     * @brief Returns a string, which describes the error code.
     *
     * This operation is used for debugging and information purposes.
     *
     * @param[in] errorCode
     *   The error code.
     *
     * @return String describing the error code.
     **/
    static string getErrorCodeString( ErrorCode errorCode) noexcept;

    /**
     * @brief Generates a TFTP error packet with the given error code and error
     *   message.
     *
     * @param[in] errorCode
     *   Error code, which shall be set.
     **/
    BaseErrorPacket( ErrorCode errorCode) noexcept;

    /**
     * @brief Generates a TFTP error packet from a data buffer.
     *
     * @param[in] rawPacket
     *   Packet, which shall be decoded.
     **/
    BaseErrorPacket( const RawTftpPacketType &rawPacket);

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
     * @return The error message.
     **/
    virtual string getErrorMessage() const = 0;

    //! @copydoc TftpPacket::encode()
    virtual RawTftpPacketType encode() const override;

    //! @copydoc TftpPacket::toString()
    virtual string toString() const override;

  protected:
    /**
     * @brief Extract the error message from a raw packet.
     *
     * This method is used to extract the error message string from the raw data
     * packet as received from somewhere.
     *
     * This method does not perform the tests, which are executed during
     * constructing the error packet by calling the BaseErrorPacket()
     * constructor.
     *
     * @param[in] rawPacket
     *   The raw error packet
     *
     * @return The decoded error message.
     **/
    static string getErrorMessage( const RawTftpPacketType &rawPacket);

  private:
    ErrorCode errorCode; //!< The error code
};

}
}

#endif
