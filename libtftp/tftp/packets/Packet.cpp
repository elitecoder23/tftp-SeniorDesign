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
 * @brief Definition of class Tftp::Packets::Packet.
 **/

#include "Packet.hpp"

#include <tftp/TftpException.hpp>

#include <helper/Endianess.hpp>
#include <helper/Logger.hpp>

namespace Tftp {
namespace Packets {

PacketType Packet::getPacketType(
	const RawTftpPacketType &rawPacket) noexcept
{
  // check minimum data size.
  if (rawPacket.size() < TFTP_PACKET_HEADER_SIZE)
  {
    BOOST_LOG_TRIVIAL( error) << "Packet to small";
    return PacketType::Invalid;
  }

  RawTftpPacketType::const_iterator it = rawPacket.begin();

  // decode opcode value
  uint16_t opcode;
  getInt< uint16_t>( it, opcode);

  // check valid opcodes
  switch ( static_cast< PacketType>( opcode))
  {
    case PacketType::ReadRequest:
    case PacketType::WriteRequest:
    case PacketType::Data:
    case PacketType::Acknowledgement:
    case PacketType::Error:
    case PacketType::OptionsAcknowledgement:
      // this are valid opcodes.
      break;

    default:
      // return INVALID for invalid values
      BOOST_LOG_TRIVIAL( error) << "Invalid opcode " << std::hex << opcode;
      return PacketType::Invalid;
  }

  return static_cast< PacketType>( opcode);
}

PacketType Packet::getPacketType() const
{
  return packetType;
}

Packet::string Packet::toString() const
{
  switch (packetType)
  {
    case PacketType::ReadRequest:
      return "RRQ";

    case PacketType::WriteRequest:
      return "WRQ";

    case PacketType::Data:
      return "DATA";

    case PacketType::Acknowledgement:
      return "ACK";

    case PacketType::Error:
      return "ERR";

    case PacketType::OptionsAcknowledgement:
      return "OACK";

    default:
      return "INV";
  }
}

Packet::Packet( const PacketType packetType) noexcept:
  packetType( packetType)
{
}

Packet::Packet(
  const PacketType expectedPacketType,
  const RawTftpPacketType &rawPacket):
  packetType( expectedPacketType)
{
  // check size
  if (rawPacket.size() < TFTP_PACKET_HEADER_SIZE)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "Invalid packet size (<TFTP_PACKETS_HEADER_SIZE)"));
  }

  RawTftpPacketType::const_iterator packetIt = rawPacket.begin();

  // Check Opcode
  uint16_t opcode;
  getInt< uint16_t>( packetIt, opcode);

  if ( static_cast< PacketType>( opcode) != expectedPacketType)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "Invalid opcode"));
  }
}

void Packet::setPacketType( const PacketType packetType)
{
  this->packetType = packetType;
}

void Packet::insertHeader( RawTftpPacketType &rawPacket) const
{
  assert( rawPacket.size() >= TFTP_PACKET_HEADER_SIZE);

  RawTftpPacketType::iterator packetIt = rawPacket.begin();

  // encode opcode
  setInt( packetIt, static_cast< uint16_t>( packetType));
}

}
}
