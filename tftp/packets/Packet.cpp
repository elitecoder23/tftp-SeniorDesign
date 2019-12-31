/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::Packet.
 **/

#include "Packet.hpp"

#include <tftp/packets/PacketException.hpp>

#include <tftp/TftpLogger.hpp>

#include <helper/Endianess.hpp>

namespace Tftp::Packets {

PacketType Packet::packetType( const RawTftpPacket &rawPacket) noexcept
{
  // check minimum data size.
  if (rawPacket.size() < HeaderSize)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
      << "Packet to small";
    return PacketType::Invalid;
  }

  RawTftpPacket::const_iterator it = rawPacket.begin();

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
      BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
        << "Invalid opcode " << std::hex << opcode;
      return PacketType::Invalid;
  }

  return static_cast< PacketType>( opcode);
}

PacketType Packet::packetType() const noexcept
{
  return packetTypeValue;
}

Packet::operator std::string() const
{
  switch (packetTypeValue)
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
  packetTypeValue( packetType)
{
}

Packet::Packet(
  const PacketType packetType,
  const RawTftpPacket &rawPacket):
  packetTypeValue( packetType)
{
  decodeHeader( rawPacket);
}

Packet& Packet::operator=( const Packet &other)
{
  if (packetTypeValue != other.packetTypeValue)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << AdditionalInfo( "Packet types are not same"));
  }

  return *this;
}

Packet& Packet::operator=( Packet &&other)
{
  if (packetTypeValue != other.packetTypeValue)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << AdditionalInfo( "Packet types are not same"));
  }

  return *this;
}

Packet& Packet::operator=( const RawTftpPacket &rawPacket)
{
  decodeHeader( rawPacket);
  return *this;
}

Packet::operator RawTftpPacket() const
{
  return encode();
}

void Packet::insertHeader( RawTftpPacket &rawPacket) const
{
  // keep assertion --> programming error of sub-classes.
  assert( rawPacket.size() >= HeaderSize);

  auto packetIt{ rawPacket.begin()};

  // encode opcode
  setInt( packetIt, static_cast< uint16_t>( packetTypeValue));
}

void Packet::decodeHeader( const RawTftpPacket &rawPacket)
{
  // check size
  if (rawPacket.size() < HeaderSize)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << AdditionalInfo( "Invalid packet size (<HEADER SIZE)"));
  }

  auto packetIt{ rawPacket.begin()};

  // Check Opcode
  uint16_t opcode{};
  getInt< uint16_t>( packetIt, opcode);

  if ( static_cast< PacketType>( opcode) != packetTypeValue)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << AdditionalInfo( "Invalid opcode"));
  }
}

}
