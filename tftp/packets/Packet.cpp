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
#include <tftp/TftpLogger.hpp>

#include <helper/Endianess.hpp>

namespace Tftp {
namespace Packets {

PacketType Packet::getPacketType( const RawTftpPacketType &rawPacket) noexcept
{
  // check minimum data size.
  if (rawPacket.size() < HeaderSize)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Packet to small";
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
      BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
        "Invalid opcode " << std::hex << opcode;
      return PacketType::Invalid;
  }

  return static_cast< PacketType>( opcode);
}

PacketType Packet::getPacketType() const
{
  return packetType;
}

Packet::operator string() const
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
  const PacketType packetType,
  const RawTftpPacketType &rawPacket):
  packetType( packetType)
{
  decodeHeader( rawPacket);
}

Packet& Packet::operator=( const Packet &other)
{
  if (packetType != other.packetType)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "Packet types are not same"));
  }

  return *this;
}

Packet& Packet::operator=( Packet &&other)
{
  if (packetType != other.packetType)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "Packet types are not same"));
  }

  return *this;
}

Packet& Packet::operator=( const RawTftpPacketType &rawPacket)
{
  decodeHeader( rawPacket);
  return *this;
}

Packet::operator RawTftpPacketType() const
{
  return encode();
}

void Packet::insertHeader( RawTftpPacketType &rawPacket) const
{
  assert( rawPacket.size() >= HeaderSize);

  auto packetIt( rawPacket.begin());

  // encode opcode
  setInt( packetIt, static_cast< uint16_t>( packetType));
}

void Packet::decodeHeader( const RawTftpPacketType &rawPacket)
{
  // check size
  if (rawPacket.size() < HeaderSize)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "Invalid packet size (<TFTP_PACKETS_HEADER_SIZE)"));
  }

  auto packetIt( rawPacket.begin());

  // Check Opcode
  uint16_t opcode;
  getInt< uint16_t>( packetIt, opcode);

  if ( static_cast< PacketType>( opcode) != packetType)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "Invalid opcode"));
  }
}

}
}
