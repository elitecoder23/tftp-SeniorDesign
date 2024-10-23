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
 * @brief Definition of Class Tftp::Packets::AcknowledgementPacket.
 **/

#include "AcknowledgementPacket.hpp"

#include <tftp/packets/PacketException.hpp>

#include <helper/Endianness.hpp>
#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

#include <format>
#include <utility>

namespace Tftp::Packets {

AcknowledgementPacket::AcknowledgementPacket(
  const BlockNumber blockNumber ) noexcept :
  Packet{ PacketType::Acknowledgement },
  blockNumberV{ blockNumber }
{
}

AcknowledgementPacket::AcknowledgementPacket(
  ConstRawTftpPacketSpan rawPacket ) :
  Packet{ PacketType::Acknowledgement, rawPacket }
{
  decodeBody( rawPacket );
}

AcknowledgementPacket& AcknowledgementPacket::operator=(
  ConstRawTftpPacketSpan rawPacket )
{
  decodeHeader( rawPacket );
  decodeBody( rawPacket );
  return *this;
}

BlockNumber AcknowledgementPacket::blockNumber() const
{
  return blockNumberV;
}

void AcknowledgementPacket::blockNumber( const BlockNumber blockNumber )
{
  blockNumberV = blockNumber;
}

AcknowledgementPacket::operator std::string() const
{
  return
    std::format( "ACK: BLOCK NO: {}", static_cast< uint16_t>( blockNumberV ) );
}

RawTftpPacket AcknowledgementPacket::encode() const
{
  RawTftpPacket rawPacket( PacketSize );

  // insert header data
  insertHeader( rawPacket );

  RawTftpPacketSpan rawSpan{ rawPacket.begin() + HeaderSize, rawPacket.end() };

  // block number
  rawSpan = Helper::setInt( rawSpan, static_cast< uint16_t >( blockNumberV ) );
  assert( rawSpan.empty() );

  return rawPacket;
}

void AcknowledgementPacket::decodeBody( ConstRawTftpPacketSpan rawPacket )
{
  // check size
  if ( rawPacket.size() != PacketSize )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo{ "Invalid packet size of ACK packet" } );
  }

  ConstRawTftpPacketSpan rawSpan{
    rawPacket.begin() + HeaderSize,
    rawPacket.end() };

  // decode block number
  std::tie( rawSpan, static_cast< uint16_t & >( blockNumberV ) ) =
    Helper::getInt< uint16_t >( rawSpan );
  assert( rawSpan.empty() );
}

}
