/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::DataPacket.
 **/

#include "DataPacket.hpp"

#include <tftp/packets/PacketException.hpp>

#include <helper/Endianess.hpp>

#include <fmt/format.h>

#include <utility>

namespace Tftp::Packets {

DataPacket::DataPacket(
  BlockNumber blockNumber,
  const Data &data ) noexcept:
  Packet{ PacketType::Data }, blockNumberV{ blockNumber }, dataV{ data }
{
}

DataPacket::DataPacket(
  BlockNumber blockNumber,
  Data &&data ) noexcept:
  Packet{ PacketType::Data }, blockNumberV{ blockNumber }, dataV{ std::move( data ) }
{
}

DataPacket::DataPacket( ConstRawTftpPacketSpan rawPacket ) :
  Packet{ PacketType::Data, rawPacket }
{
  decodeBody( rawPacket );
}

DataPacket& DataPacket::operator=( ConstRawTftpPacketSpan rawPacket )
{
  decodeHeader( rawPacket );
  decodeBody( rawPacket );
  return *this;
}

BlockNumber DataPacket::blockNumber() const
{
  return blockNumberV;
}

BlockNumber& DataPacket::blockNumber()
{
  return blockNumberV;
}

void DataPacket::blockNumber( const BlockNumber blockNumber )
{
  blockNumberV = blockNumber;
}

const DataPacket::Data& DataPacket::data() const
{
  return dataV;
}

DataPacket::Data& DataPacket::data()
{
  return dataV;
}

void DataPacket::data( const Data &data )
{
  dataV = data;
}

void DataPacket::data( Data &&data )
{
  dataV = std::move( data );
}

size_t DataPacket::dataSize() const
{
  return dataV.size();
}

DataPacket::operator std::string() const
{
  return fmt::format(
    "DATA: BLOCK NO: {} DATA: {} bytes",
    static_cast< uint16_t>( blockNumber() ),
    dataSize() );
}

RawTftpPacket DataPacket::encode() const
{
  RawTftpPacket rawPacket( 4U + dataV.size() );

  insertHeader( rawPacket );

  auto packetIt{ rawPacket.begin() + HeaderSize };

  // block number
  packetIt = Helper::setInt( packetIt, static_cast< uint16_t>( blockNumberV ) );

  // data
  std::copy( dataV.begin(), dataV.end(), packetIt );

  return rawPacket;
}

void DataPacket::decodeBody( ConstRawTftpPacketSpan rawPacket )
{
  // check size
  if (rawPacket.size() < MinPacketSize)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo{ "Invalid packet size of DATA packet" } );
  }

  auto packetIt{ rawPacket.begin() + HeaderSize};

  // decode block number
  packetIt = Helper::getInt< uint16_t>(
    packetIt,
    static_cast< uint16_t&>( blockNumberV ));

  // copy data
  dataV.assign( packetIt, rawPacket.end());
}

}
