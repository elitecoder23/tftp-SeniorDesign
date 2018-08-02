/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class Tftp::Packets::AcknowledgementPacket.
 **/

#include "AcknowledgementPacket.hpp"

#include <tftp/TftpException.hpp>

#include <helper/Endianess.hpp>

namespace Tftp::Packets {

AcknowledgementPacket::AcknowledgementPacket(
  const BlockNumber blockNumber) noexcept :
  Packet( PacketType::Acknowledgement),
  blockNumberValue( blockNumber)
{
}

AcknowledgementPacket::AcknowledgementPacket( const RawTftpPacket &rawPacket) :
  Packet( PacketType::Acknowledgement, rawPacket)
{
  decodeBody( rawPacket);
}

AcknowledgementPacket& AcknowledgementPacket::operator=(
  const RawTftpPacket &rawPacket)
{
  Packet::operator =( rawPacket);
  decodeBody( rawPacket);
  return *this;
}

BlockNumber AcknowledgementPacket::blockNumber() const
{
  return blockNumberValue;
}

void AcknowledgementPacket::blockNumber( const BlockNumber blockBumber)
{
  blockNumberValue = blockBumber;
}

AcknowledgementPacket::operator std::string() const
{
  return (boost::format( "ACK: BLOCKNO: %d") % blockNumber()).str();
}

Tftp::RawTftpPacket AcknowledgementPacket::encode() const
{
  RawTftpPacket rawPacket( 4U);

  // insert header data
  insertHeader( rawPacket);

  auto packetIt( rawPacket.begin() + HeaderSize);

  // Add block number
  setInt( packetIt, static_cast< uint16_t>( blockNumberValue));

  return rawPacket;
}

void AcknowledgementPacket::decodeBody( const RawTftpPacket &rawPacket)
{
  // check size
  if (rawPacket.size() != 4U)
  {
    //! @throw InvalidPacketException When packet size is invalid
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "Invalid packet size of ACK packet"));
  }

  auto packetIt( rawPacket.begin() + HeaderSize);

  // decode block number
  getInt< uint16_t>( packetIt, blockNumberValue);
}

}
