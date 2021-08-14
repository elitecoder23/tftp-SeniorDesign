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

PacketType Packet::packetType( ConstRawTftpPacketSpan rawPacket) noexcept
{
  // check minimum data size.
  if ( rawPacket.size() < HeaderSize )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Packet to small";
    return PacketType::Invalid;
  }

  auto it{ rawPacket.begin() };

  // decode opcode value
  uint16_t opcode{};
  Helper::getInt< uint16_t>( it, opcode );

  // check valid opcodes
  switch ( static_cast< PacketType>( opcode ) )
  {
    case PacketType::ReadRequest:
    case PacketType::WriteRequest:
    case PacketType::Data:
    case PacketType::Acknowledgement:
    case PacketType::Error:
    case PacketType::OptionsAcknowledgement:
      // these are valid opcodes.
      break;

    default:
      // return INVALID for invalid values
      BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
        << "Invalid opcode " << std::hex << opcode;
      return PacketType::Invalid;
  }

  return static_cast< PacketType>( opcode);
}

PacketType Packet::packetType() const noexcept
{
  return packetTypeV;
}

Packet::operator std::string() const
{
  switch ( packetTypeV )
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

Packet::Packet( const PacketType packetType ) noexcept:
  packetTypeV{ packetType }
{
}

Packet::Packet(
  const PacketType packetType,
  ConstRawTftpPacketSpan rawPacket ):
  packetTypeV{ packetType }
{
  decodeHeader( rawPacket );
}

Packet::operator RawTftpPacket() const
{
  return encode();
}

Packet& Packet::operator=( const Packet &other [[maybe_unused]] ) noexcept
{
  // assure same packet type
  assert( packetTypeV == other.packetTypeV );
  return *this;
}

Packet& Packet::operator=( Packet &&other [[maybe_unused]] ) noexcept
{
  // assure same packet type
  assert( packetTypeV == other.packetTypeV );
  return *this;
}

void Packet::insertHeader( RawTftpPacketSpan rawPacket ) const
{
  // keep assertion --> programming error of sub-classes.
  assert( rawPacket.size() >= HeaderSize );

  auto packetIt{ rawPacket.begin() };

  // encode opcode
  Helper::setInt( packetIt, static_cast< uint16_t>( packetTypeV ) );
}

void Packet::decodeHeader( ConstRawTftpPacketSpan rawPacket )
{
  // check size
  if ( rawPacket.size() < HeaderSize )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo{ "Invalid packet size (HEADER SIZE)" } );
  }

  auto packetIt{ rawPacket.begin() };

  // Check Opcode
  uint16_t opcode{};
  Helper::getInt< uint16_t>( packetIt, opcode );

  if ( static_cast< PacketType>( opcode) != packetTypeV )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo{ "Invalid opcode" } );
  }
}

}
