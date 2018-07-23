/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::Packets::PacketFactory.
 **/

#ifndef TFTP_PACKETS_PACKETFACTORY_HPP
#define TFTP_PACKETS_PACKETFACTORY_HPP

#include <tftp/packets/Packets.hpp>

#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/ErrorPacket.hpp>
#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>

namespace Tftp::Packets {

/**
 * @brief Helper Class to get the type of a received TFTP packet and get the
 *   approbate instance.
 *
 * This class is used internally to decode the TFTP packet type and
 * generate the approbate class.
 **/
class PacketFactory
{
  public:
    /**
     * @brief Determines the packet type for the given raw data packet.
     *
     * @param[in] rawPacket
     *   The raw data packet.
     *
     * @return The TFTP packet type of the data packet or INVALID.
     * @retval INVALID
     *   If the packet cannot be decoded.
     **/
    static PacketType packetType( const RawTftpPacket &rawPacket) noexcept;

    /**
     * @brief Decodes the given raw data as TFTP Read Request Packet.
     *
     * @param[in] rawPacket
     *   The raw packet data.
     *
     * @return The corresponding packet class.
     *
     * @throw InvalidPacketException
     *   When packet cannot be decoded as Read Request Packet (RRQ).
     **/
    static ReadRequestPacket readRequestPacket(
      const RawTftpPacket &rawPacket);

    /**
     * @brief Decodes the given raw data as TFTP Write Request Packet.
     *
     * @param[in] rawPacket
     *   The raw packet data
     *
     * @return The corresponding packet class.
     *
     * @throw InvalidPacketException
     *    When packet cannot be decoded as Write Request Packet.
     **/
    static WriteRequestPacket writeRequestPacket(
      const RawTftpPacket &rawPacket);

    /**
     * @brief Decodes the given raw data as TFTP Data Packet.
     *
     * @param[in] rawPacket
     *   The raw packet data
     *
     * @return The corresponding packet class.
     *
     * @throw InvalidPacketException
     *  When packet cannot be decoded as Data Packet.
     **/
    static DataPacket dataPacket( const RawTftpPacket &rawPacket);

    /**
     * @brief Decodes the given raw data as TFTP Error Packet.
     *
     * @param[in] rawPacket
     *   The raw packet data.
     *
     * @return The corresponding packet class.
     *
     * @throw InvalidPacketException
     *  When packet cannot be decoded as Error Packet.
     **/
    static ErrorPacket errorPacket( const RawTftpPacket &rawPacket);

    /**
     * @brief Decodes the given raw data as TFTP Acknowledgement Packet.
     *
     * @param[in] rawPacket
     *   The raw packet data.
     *
     * @return The corresponding packet class.
     *
     * @throw InvalidPacketException
     *    When packet cannot be decoded as Acknowledgement Packet.
     **/
    static AcknowledgementPacket acknowledgementPacket(
      const RawTftpPacket &rawPacket);

    /**
     * @brief Decodes the given raw data as TFTP Options Acknowledgement
     *   Packet.
     *
     * @param[in] rawPacket
     *   The raw packet data.
     *
     * @return The corresponding packet class.
     *
     * @throw InvalidPacketException
     *   When packet cannot be decoded as Options Acknowledgement Packet.
     **/
    static OptionsAcknowledgementPacket optionsAcknowledgementPacket(
      const RawTftpPacket &rawPacket);

  private:
    //! Private Constructor, which is not implemented.
    PacketFactory() = delete;
};

}

#endif
