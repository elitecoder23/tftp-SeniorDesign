// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Packets::PacketHandler.
 **/

#ifndef TFTP_PACKETS_PACKETHANDLER_HPP
#define TFTP_PACKETS_PACKETHANDLER_HPP

#include <tftp/packets/Packets.hpp>

#include <boost/asio/ip/udp.hpp>

namespace Tftp::Packets {

/**
 * @brief TFTP Packet Handler
 *
 * Interface, which must be implemented by an TFTP packet Handler.
 *
 * The packet handlers are used internally.
 * A user of the TFTP library doesn't need to derive from this class.
 **/
class TFTP_EXPORT PacketHandler
{
  public:
    //! Destructor
    virtual ~PacketHandler() = default;

    /**
     * @brief The handler of all received packets.
     *
     * This handler tries to decode the received packet as TFTP packet and calls
     * the suitable handler method.
     *
     * If the packet cannot be decoded handleInvalidPacket() is called.
     *
     * If during handling (including packet conversion) a InvalidPacketException
     * exception is thrown, handleInvalidPacket is called automatically.
     * This exception is not re-thrown.
     *
     * @param[in] remote
     *   Source of the TFTP Packet.
     * @param[in] rawPacket
     *   Received TFTP Packet.
     **/
    void packet(
      const boost::asio::ip::udp::endpoint &remote,
      ConstRawDataSpan rawPacket );

  protected:
    /**
     * @brief Handler for TFTP Read Request Packets (RRQ).
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] readRequestPacket
     *   Read request packet.
     **/
    virtual void readRequestPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::ReadRequestPacket &readRequestPacket ) = 0;

    /**
     * @brief Handler for TFTP Write Request Packets (WRQ).
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] writeRequestPacket
     *   Write request packet.
     **/
    virtual void writeRequestPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::WriteRequestPacket &writeRequestPacket ) = 0;

    /**
     * @brief Handler for TFTP Data Packets (DATA).
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] dataPacket
     *   Data packet.
     **/
    virtual void dataPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::DataPacket &dataPacket ) = 0;

    /**
     * @brief Handler for TFTP Acknowledgement Packets (ACK).
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] acknowledgementPacket
     *   Acknowledgement packet.
     **/
    virtual void acknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::AcknowledgementPacket &acknowledgementPacket ) = 0;

    /**
     * @brief Handler for TFTP Error Packets (ERR).
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] errorPacket
     *   Error packet.
     **/
    virtual void errorPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::ErrorPacket &errorPacket ) = 0;

    /**
     * @brief Handler for TFTP Option Acknowledgement Packets (RRQ).
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] optionsAcknowledgementPacket
     *   Option acknowledgement packet.
     **/
    virtual void optionsAcknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket ) = 0;

    /**
     * @brief Handler for Invalid TFTP Packets.
     *
     * @param[in] remote
     *   Source of the packet.
     * @param[in] rawPacket
     *   Invalid packet data.
     **/
    virtual void invalidPacket(
      const boost::asio::ip::udp::endpoint &remote,
      ConstRawDataSpan rawPacket) = 0;
};

}

#endif
