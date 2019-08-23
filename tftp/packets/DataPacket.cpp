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
#include <utility>

namespace Tftp::Packets {

DataPacket::DataPacket(
  BlockNumber blockNumber,
  const Data &data) noexcept:
  Packet{ PacketType::Data},
  blockNumberValue{ blockNumber},
  dataValue{ data}
{
}

DataPacket::DataPacket(
  BlockNumber blockNumber,
  Data &&data) noexcept:
  Packet{ PacketType::Data},
  blockNumberValue{ blockNumber},
  dataValue{ std::move( data)}
{
}

DataPacket::DataPacket( const RawTftpPacket &rawPacket) :
  Packet{ PacketType::Data, rawPacket}
{
  decodeBody( rawPacket);
}

DataPacket& DataPacket::operator=( const RawTftpPacket &rawPacket)
{
  // inherited operator
  Packet::operator =( rawPacket);
  // decode body
  decodeBody( rawPacket);
  return *this;
}

BlockNumber DataPacket::blockNumber() const
{
  return blockNumberValue;
}

BlockNumber& DataPacket::blockNumber()
{
  return blockNumberValue;
}

void DataPacket::blockNumber( const BlockNumber blockNumber)
{
  blockNumberValue = blockNumber;
}

const DataPacket::Data& DataPacket::data() const
{
  return dataValue;
}

DataPacket::Data& DataPacket::data()
{
  return dataValue;
}

void DataPacket::data( const Data &data)
{
  dataValue = data;
}

void DataPacket::data( Data &&data)
{
  dataValue = std::move( data);
}

size_t DataPacket::dataSize() const
{
  return dataValue.size();
}

DataPacket::operator std::string() const
{
  return (boost::format( "DATA: BLOCKNO: %d DATA: %d bytes") %
    blockNumber() %
    dataSize()).str();
}

Tftp::RawTftpPacket DataPacket::encode() const
{
  RawTftpPacket rawPacket( 4 + dataValue.size());

  insertHeader( rawPacket);

  auto packetIt{ rawPacket.begin() + HeaderSize};

  // block number
  packetIt = setInt( packetIt, static_cast< uint16_t>( blockNumberValue));

  // data
  std::copy( dataValue.begin(), dataValue.end(), packetIt);

  return rawPacket;
}

void DataPacket::decodeBody( const RawTftpPacket &rawPacket)
{
  // check size
  if (rawPacket.size() < 4U)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << AdditionalInfo( "Invalid packet size of DATA packet"));
  }

  auto packetIt{ rawPacket.begin() + HeaderSize};

  // decode block number
  packetIt = getInt< uint16_t>( packetIt, blockNumberValue);

  // copy data
  dataValue.assign( packetIt, rawPacket.end());
}

}
