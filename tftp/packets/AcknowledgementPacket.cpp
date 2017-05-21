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
 * @brief Definition of class Tftp::Packets::AcknowledgmentPacket.
 **/

#include "AcknowledgementPacket.hpp"

#include <tftp/TftpException.hpp>

#include <helper/Endianess.hpp>

namespace Tftp {
namespace Packets {

AcknowledgementPacket::AcknowledgementPacket(
  const BlockNumber blockNumber) noexcept:
  Packet( PacketType::Acknowledgement),
  blockNumber( blockNumber)
{
}

AcknowledgementPacket::AcknowledgementPacket(
  const RawTftpPacketType &rawPacket):
  Packet( PacketType::Acknowledgement, rawPacket)
{
  decodeBody( rawPacket);
}

AcknowledgementPacket& AcknowledgementPacket::operator=(
  const RawTftpPacketType &rawPacket)
{
  Packet::operator =( rawPacket);
  decodeBody( rawPacket);
  return *this;
}

BlockNumber AcknowledgementPacket::getBlockNumber() const
{
  return blockNumber;
}

void AcknowledgementPacket::setBlockNumber( const BlockNumber blockBumber)
{
  this->blockNumber = blockBumber;
}

AcknowledgementPacket::operator string() const
{
  return (boost::format( "ACK: BLOCKNO: %d") % getBlockNumber()).str();
}

Tftp::RawTftpPacketType AcknowledgementPacket::encode() const
{
  RawTftpPacketType rawPacket( 4U);

  // insert header data
  insertHeader( rawPacket);

  auto packetIt( rawPacket.begin() + HeaderSize);

  // Add block number
  setInt( packetIt, static_cast< const uint16_t>( blockNumber));

  return rawPacket;
}

void AcknowledgementPacket::decodeBody( const RawTftpPacketType &rawPacket)
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
  getInt< uint16_t>( packetIt, blockNumber);
}

}
}
