// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::Packet.
 **/

#include "Packet.hpp"

#include <tftp/packets/PacketException.hpp>

#include <helper/Exception.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/all.hpp>

namespace Tftp::Packets {

PacketType Packet::packetType( Helper::ConstRawDataSpan rawPacket )
{
  // check minimum data size.
  if ( rawPacket.size() < HeaderSize )
  {
    SPDLOG_ERROR( "Packet to small" );
    return PacketType::Invalid;
  }

  // decode opcode value
  auto [ _, opcode ]{ Helper::RawData_getInt< uint16_t >( rawPacket ) };

  // check valid opcodes
  // NOLINTNEXTLINE( clang-analyzer-optin.core.EnumCastOutOfRange ): Validity Check
  switch ( PacketType{ opcode } )
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
      SPDLOG_ERROR( "Invalid opcode 0x{:04X}", opcode );
      return PacketType::Invalid;
  }

  return PacketType{ opcode };
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

Packet::Packet( const PacketType packetType, Helper::ConstRawDataSpan rawPacket ) : packetTypeV{ packetType }
{
  decodeHeader( rawPacket );
}

Packet::operator Helper::RawData() const
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

void Packet::insertHeader( Helper::RawDataSpan rawPacket ) const
{
  // keep assertion --> programming error of subclasses.
  assert( rawPacket.size() >= HeaderSize );

  // encode opcode
  Helper::RawData_setInt( rawPacket, std::to_underlying( packetTypeV ) );
}

void Packet::decodeHeader( Helper::ConstRawDataSpan rawPacket )
{
  // check size
  if ( rawPacket.size() < HeaderSize )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo{ "Invalid packet size (HEADER SIZE)" } );
  }

  // Check Opcode
  auto [ _, opcode ]{ Helper::RawData_getInt< uint16_t >( rawPacket ) };

  if ( opcode != std::to_underlying( packetTypeV ) )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo{ "Invalid opcode" } );
  }
}

}
