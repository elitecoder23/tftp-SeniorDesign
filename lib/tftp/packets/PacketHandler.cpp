// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::PacketHandler.
 **/

#include "PacketHandler.hpp"

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/ErrorPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>
#include <tftp/packets/PacketException.hpp>
#include <tftp/packets/PacketStatistic.hpp>
#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>

#include <tftp/Logger.hpp>

namespace Tftp::Packets {

void PacketHandler::packet( const boost::asio::ip::udp::endpoint &remote, Helper::ConstRawDataSpan rawPacket )
{
  BOOST_LOG_FUNCTION()

  switch ( Packets::Packet::packetType( rawPacket ) )
  {
    case PacketType::ReadRequest:
      try
      {
        readRequestPacket( remote, Packets::ReadRequestPacket{ rawPacket } );

        // Update statistic
        PacketStatistic::globalReceive().packet( PacketType::ReadRequest, rawPacket.size() );
      }
      catch ( const Packets::InvalidPacketException &e )
      {
        BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
          << "Error decoding/ handling RRQ packet: " << e.what();

        // Update statistic
        PacketStatistic::globalReceive().packet( PacketType::Invalid, rawPacket.size() );

        invalidPacket( remote, rawPacket );
      }
      break;

    case PacketType::WriteRequest:
      try
      {
        writeRequestPacket( remote, Packets::WriteRequestPacket{ rawPacket } );

        // Update statistic
        PacketStatistic::globalReceive().packet( PacketType::WriteRequest, rawPacket.size() );
      }
      catch ( const Packets::InvalidPacketException &e )
      {
        BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
          << "Error decoding/ handling WRQ packet: " << e.what();

        // Update statistic
        PacketStatistic::globalReceive().packet( PacketType::Invalid, rawPacket.size() );

        invalidPacket( remote, rawPacket );
      }
      break;

    case PacketType::Data:
      try
      {
        dataPacket( remote, Packets::DataPacket{ rawPacket } );

        // Update statistic
        PacketStatistic::globalReceive().packet( PacketType::Data, rawPacket.size() );
      }
      catch ( const Packets::InvalidPacketException &e )
      {
        BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
          << "Error decoding/ handling DATA packet: " << e.what();

        // Update statistic
        PacketStatistic::globalReceive().packet( PacketType::Invalid, rawPacket.size() );

        invalidPacket( remote, rawPacket );
      }
      break;

    case PacketType::Acknowledgement:
      try
      {
        acknowledgementPacket( remote, Packets::AcknowledgementPacket{ rawPacket } );

        // Update statistic
        PacketStatistic::globalReceive().packet( PacketType::Acknowledgement, rawPacket.size() );
      }
      catch ( const Packets::InvalidPacketException &e )
      {
        BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
          << "Error decoding/ handling ACK packet: " << e.what();

        // Update statistic
        PacketStatistic::globalReceive().packet( PacketType::Invalid, rawPacket.size() );

        invalidPacket( remote, rawPacket );
      }
      break;

    case PacketType::Error:
      try
      {
        errorPacket( remote, Packets::ErrorPacket{ rawPacket } );

        // Update statistic
        PacketStatistic::globalReceive().packet( PacketType::Error, rawPacket.size() );
      }
      catch ( const Packets::InvalidPacketException &e )
      {
        BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
          << "Error decoding/ handling ERR packet: " << e.what();

        // Update statistic
        PacketStatistic::globalReceive().packet( PacketType::Invalid, rawPacket.size() );

        invalidPacket( remote, rawPacket );
      }
      break;

    case PacketType::OptionsAcknowledgement:
      try
      {
        optionsAcknowledgementPacket( remote, Packets::OptionsAcknowledgementPacket{ rawPacket } );

        // Update statistic
        PacketStatistic::globalReceive().packet( PacketType::OptionsAcknowledgement, rawPacket.size() );
      }
      catch ( const Packets::InvalidPacketException &e )
      {
        BOOST_LOG_SEV( Logger::get(), Helper::Severity::error )
          << "Error decoding/ handling OACK packet: " << e.what();

        // Update statistic
        PacketStatistic::globalReceive().packet( PacketType::Invalid, rawPacket.size() );

        invalidPacket( remote, rawPacket );
      }
      break;

    default:
      // Update statistic
      PacketStatistic::globalReceive().packet( PacketType::Invalid, rawPacket.size() );

      invalidPacket( remote, rawPacket );
      break;
  }
}

}
