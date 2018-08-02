/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class Tftp::PacketHandler.
 **/

#include "PacketHandler.hpp"

#include <tftp/packets/PacketFactory.hpp>

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>

namespace Tftp {

void PacketHandler::handlePacket(
  const boost::asio::ip::udp::endpoint &from,
  const RawTftpPacket &rawPacket)
{
  BOOST_LOG_FUNCTION();

  using Packets::PacketFactory;

  switch ( PacketFactory::packetType( rawPacket))
  {
    case PacketType::ReadRequest:
      try
      {
        handleReadRequestPacket(
          from,
          PacketFactory::readRequestPacket( rawPacket));
      }
      catch ( InvalidPacketException &e)
      {
        BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
          "Error decoding/ handling RRQ packet: " <<
          e.what();
        handleInvalidPacket( from, rawPacket);
      }
      break;

    case PacketType::WriteRequest:
      try
      {
        handleWriteRequestPacket(
          from,
          PacketFactory::writeRequestPacket( rawPacket));
      }
      catch ( InvalidPacketException &e)
      {
        BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
          "Error decoding/ handling WRQ packet: " <<
          e.what();
        handleInvalidPacket( from, rawPacket);
      }
      break;

    case PacketType::Data:
      try
      {
        handleDataPacket( from, PacketFactory::dataPacket( rawPacket));
      }
      catch ( InvalidPacketException &e)
      {
        BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
          "Error decoding/ handling DATA packet: "<<
          e.what();
        handleInvalidPacket( from, rawPacket);
      }
      break;

    case PacketType::Acknowledgement:
      try
      {
        handleAcknowledgementPacket(
          from,
          PacketFactory::acknowledgementPacket( rawPacket));
      }
      catch ( InvalidPacketException &e)
      {
        BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
          "Error decoding/ handling ACK packet: " <<
          e.what();
        handleInvalidPacket( from, rawPacket);
      }
      break;

    case PacketType::Error:
      try
      {
        handleErrorPacket( from, PacketFactory::errorPacket( rawPacket));
      }
      catch ( InvalidPacketException &e)
      {
        BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
          "Error decoding/ handling ERR packet: " <<
          e.what();
        handleInvalidPacket( from, rawPacket);
      }
      break;

    case PacketType::OptionsAcknowledgement:
      try
      {
        handleOptionsAcknowledgementPacket(
          from,
          PacketFactory::optionsAcknowledgementPacket( rawPacket));
      }
      catch ( InvalidPacketException &e)
      {
        BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
          "Error decoding/ handling OACK packet: " <<
          e.what();
        handleInvalidPacket( from, rawPacket);
      }
      break;

    default:
      handleInvalidPacket( from, rawPacket);
      break;
  }
}

}
