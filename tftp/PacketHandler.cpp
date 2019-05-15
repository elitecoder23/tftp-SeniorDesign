/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::PacketHandler.
 **/

#include "PacketHandler.hpp"

#include <tftp/packets/Packet.hpp>
#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>
#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/ErrorPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>

namespace Tftp {

void PacketHandler::packet(
  const boost::asio::ip::udp::endpoint &remote,
  const RawTftpPacket &rawPacket)
{
  BOOST_LOG_FUNCTION()

  switch ( Packets::Packet::packetType( rawPacket))
  {
    case PacketType::ReadRequest:
      try
      {
        readRequestPacket(
          remote,
          Packets::ReadRequestPacket{ rawPacket});
      }
      catch ( InvalidPacketException &e)
      {
        BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
          << "Error decoding/ handling RRQ packet: " << e.what();
        invalidPacket( remote, rawPacket);
      }
      break;

    case PacketType::WriteRequest:
      try
      {
        writeRequestPacket(
          remote,
          Packets::WriteRequestPacket{ rawPacket});
      }
      catch ( InvalidPacketException &e)
      {
        BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
          << "Error decoding/ handling WRQ packet: " << e.what();
        invalidPacket( remote, rawPacket);
      }
      break;

    case PacketType::Data:
      try
      {
        dataPacket( remote, Packets::DataPacket{ rawPacket});
      }
      catch ( InvalidPacketException &e)
      {
        BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
          << "Error decoding/ handling DATA packet: "<< e.what();
        invalidPacket( remote, rawPacket);
      }
      break;

    case PacketType::Acknowledgement:
      try
      {
        acknowledgementPacket(
          remote,
          Packets::AcknowledgementPacket{ rawPacket});
      }
      catch ( InvalidPacketException &e)
      {
        BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
          << "Error decoding/ handling ACK packet: " << e.what();
        invalidPacket( remote, rawPacket);
      }
      break;

    case PacketType::Error:
      try
      {
        errorPacket( remote, Packets::ErrorPacket{ rawPacket});
      }
      catch ( InvalidPacketException &e)
      {
        BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
          << "Error decoding/ handling ERR packet: " << e.what();
        invalidPacket( remote, rawPacket);
      }
      break;

    case PacketType::OptionsAcknowledgement:
      try
      {
        optionsAcknowledgementPacket(
          remote,
          Packets::OptionsAcknowledgementPacket{ rawPacket});
      }
      catch ( InvalidPacketException &e)
      {
        BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
          << "Error decoding/ handling OACK packet: " << e.what();
        invalidPacket( remote, rawPacket);
      }
      break;

    default:
      invalidPacket( remote, rawPacket);
      break;
  }
}

}
