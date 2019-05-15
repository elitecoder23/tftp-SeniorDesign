/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::PacketHandler.
 **/

#ifndef TFTP_PACKETHANDLER_HPP
#define TFTP_PACKETHANDLER_HPP

#include <tftp/Tftp.hpp>
#include <tftp/packets/Packets.hpp>

#include <boost/asio/ip/udp.hpp>

namespace Tftp {

/**
 * @brief TFTP Packet Handler
 *
 * Interface, which must be implemented by an TFTP packet Handler.
 *
 * The packet handlers are used internally.
 * A user of the TFTP library doesn't need to derive from this class.
 **/
class PacketHandler
{
  public:
    //! Destructur
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
     * @param[in] remote
     *   The source of the packet.
     * @param[in] rawPacket
     *   The received packet.
     **/
    void packet(
      const boost::asio::ip::udp::endpoint &remote,
      const RawTftpPacket &rawPacket);

  protected:
    /**
     * @brief Handler for TFTP Read Request Packets (RRQ).
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] readRequestPacket
     *   The read request packet.
     **/
    virtual void readRequestPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::ReadRequestPacket &readRequestPacket) = 0;

    /**
     * @brief Handler for TFTP Write Request Packets (WRQ).
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] writeRequestPacket
     *   The write request packet.
     **/
    virtual void writeRequestPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::WriteRequestPacket &writeRequestPacket) = 0;

    /**
     * @brief Handler for TFTP Data Packets (DATA).
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] dataPacket
     *   The data packet.
     **/
    virtual void dataPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::DataPacket &dataPacket) = 0;

    /**
     * @brief Handler for TFTP Acknowledgement Packets (ACK).
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] acknowledgementPacket
     *   The acknowledgement packet.
     **/
    virtual void acknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::AcknowledgementPacket &acknowledgementPacket) = 0;

    /**
     * @brief Handler for TFTP Error Packets (ERR).
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] errorPacket
     *   The error packet.
     **/
    virtual void errorPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::ErrorPacket &errorPacket) = 0;

    /**
     * @brief Handler for TFTP Option Acknowledgement Packets (RRQ).
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] optionsAcknowledgementPacket
     *   The option acknowledgement packet.
     **/
    virtual void optionsAcknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket) = 0;

    /**
     * @brief Handler for Invalid TFTP Packets.
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] rawPacket
     *   The invalid packet data.
     **/
    virtual void invalidPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const RawTftpPacket &rawPacket) = 0;
};

}

#endif
