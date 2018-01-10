/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class interface Tftp::PacketHandler.
 **/

#ifndef TFTP_PACKETHANDLER_HPP
#define TFTP_PACKETHANDLER_HPP

#include <tftp/Tftp.hpp>
#include <tftp/packets/Packets.hpp>

namespace Tftp {

/**
 * @brief Interface, which must be implemented by an TFTP packet Handler.
 *
 * The packet handlers are used internally.
 * A user of the TFTP library doesn't need to derive from this class.
 **/
class PacketHandler
{
  public:
    //! Default virtual destructur
    virtual ~PacketHandler() noexcept = default;

    /**
     * @brief The handler of all received packets.
     *
     * This handler tries to decode the received packet as TFTP packet and calls
     * the approbate handler method.
     *
     * If the packet cannot be decoded handleInvalidPacket() is called.
     *
     * If during handling (including packet conversion) a InvalidPacketException
     * exception is thrown, handleInvalidPacket is called automatically.
     * This exception is not re-thrown.
     *
     * @param[in] from
     *   The source of the packet.
     * @param[in] rawPacket
     *   The received packet.
     **/
    void handlePacket(
      const UdpAddressType &from,
      const RawTftpPacket &rawPacket);

  protected:
    /**
     * @brief Handler for TFTP read request packets (RRQ).
     *
     * @param[in] from
     *   Source of the packet.
     * @param[in] readRequestPacket
     *   The read request packet.
     **/
    virtual void handleReadRequestPacket(
      const UdpAddressType &from,
      const Packets::ReadRequestPacket &readRequestPacket) = 0;

    /**
     * @brief Handler for TFTP write request packets (WRQ).
     *
     * @param[in] from
     *   Source of the packet.
     * @param[in] writeRequestPacket
     *   The write request packet.
     **/
    virtual void handleWriteRequestPacket(
      const UdpAddressType &from,
      const Packets::WriteRequestPacket &writeRequestPacket) = 0;

    /**
     * @brief Handler for TFTP data packets (DATA).
     *
     * @param[in] from
     *   Source of the packet.
     * @param[in] dataPacket
     *   The data packet.
     **/
    virtual void handleDataPacket(
      const UdpAddressType &from,
      const Packets::DataPacket &dataPacket) = 0;

    /**
     * @brief Handler for TFTP acknowledgement packets (ACK).
     *
     * @param[in] from
     *   Source of the packet.
     * @param[in] acknowledgementPacket
     *   The acknowledgement packet.
     **/
    virtual void handleAcknowledgementPacket(
      const UdpAddressType &from,
      const Packets::AcknowledgementPacket &acknowledgementPacket) = 0;

    /**
     * @brief Handler for TFTP error packets (ERR).
     *
     * @param[in] from
     *   Source of the packet.
     * @param[in] errorPacket
     *   The error packet.
     **/
    virtual void handleErrorPacket(
      const UdpAddressType &from,
      const Packets::ErrorPacket &errorPacket) = 0;

    /**
     * @brief Handler for TFTP option acknowledgement packets (RRQ).
     *
     * @param[in] from
     *   Source of the packet.
     * @param[in] optionsAcknowledgementPacket
     *   The option acknowledgement packet.
     **/
    virtual void handleOptionsAcknowledgementPacket(
      const UdpAddressType &from,
      const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket) = 0;

    /**
     * @brief Handler for invalid TFTP packets.
     *
     * @param[in] from
     *   Source of the packet.
     * @param[in] rawPacket
     *   The invalid packet data.
     **/
    virtual void handleInvalidPacket(
      const UdpAddressType &from,
      const RawTftpPacket &rawPacket) = 0;
};

}

#endif
