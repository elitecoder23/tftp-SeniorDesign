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

#include <helper/Endianess.hpp>

namespace Tftp::Packets {

AcknowledgementPacket::AcknowledgementPacket(
  const BlockNumber blockNumber) noexcept :
  Packet{ PacketType::Acknowledgement},
  blockNumberV{ blockNumber}
{
}

AcknowledgementPacket::AcknowledgementPacket(
  ConstRawTftpPacketSpan rawPacket ) :
  Packet{ PacketType::Acknowledgement, rawPacket}
{
  decodeBody( rawPacket);
}

AcknowledgementPacket& AcknowledgementPacket::operator=( ConstRawTftpPacketSpan rawPacket)
{
  decodeHeader( rawPacket);
  decodeBody( rawPacket);
  return *this;
}

BlockNumber AcknowledgementPacket::blockNumber() const
{
  return blockNumberV;
}

void AcknowledgementPacket::blockNumber( const BlockNumber blockNumber)
{
  blockNumberV = blockNumber;
}

AcknowledgementPacket::operator std::string() const
{
  return ( boost::format( "ACK: BLOCKNO: %d") %
    static_cast< uint16_t>( blockNumber())).str();
}

RawTftpPacket AcknowledgementPacket::encode() const
{
  RawTftpPacket rawPacket( 4U);

  // insert header data
  insertHeader( rawPacket);

  auto packetIt( rawPacket.begin() + HeaderSize);

  // Add block number
  Helper::setInt( packetIt, static_cast< uint16_t>( blockNumberV ) );

  return rawPacket;
}

void AcknowledgementPacket::decodeBody( ConstRawTftpPacketSpan rawPacket)
{
  // check size
  if ( rawPacket.size() != PacketSize)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo( "Invalid packet size of ACK packet"));
  }

  auto packetIt{ rawPacket.begin() + HeaderSize};

  // decode block number
  Helper::getInt< uint16_t>( packetIt, static_cast< uint16_t&>( blockNumberV ));
}

}
