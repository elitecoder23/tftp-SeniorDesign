/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class Tftp::Packets::DataPacket.
 **/

#include "DataPacket.hpp"

#include <tftp/TftpException.hpp>

#include <helper/Endianess.hpp>

namespace Tftp {
namespace Packets {

DataPacket::DataPacket(
  BlockNumber blockNumber,
  const DataType &data) noexcept:
  Packet( PacketType::Data),
  blockNumberValue( blockNumber),
  dataValue( data)
{
}

DataPacket::DataPacket( const RawTftpPacket &rawPacket) :
  Packet( PacketType::Data, rawPacket)
{
  decodeBody( rawPacket);
}

DataPacket& DataPacket::operator=( const RawTftpPacket &rawPacket)
{
  Packet::operator =( rawPacket);
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

void DataPacket::blockNumber( BlockNumber blockBumber)
{
  blockNumberValue = blockBumber;
}

const DataPacket::DataType& DataPacket::data() const
{
  return dataValue;
}

DataPacket::DataType& DataPacket::data()
{
  return dataValue;
}

void DataPacket::data( const DataType &data)
{
  dataValue = data;
}

void DataPacket::data( DataType &&data)
{
  dataValue = std::move( data);
}

size_t DataPacket::dataSize() const
{
  return dataValue.size();
}

DataPacket::operator string() const
{
  return (boost::format( "DATA: BLOCKNO: %d DATA: %d bytes") %
    blockNumber() %
    dataSize()).str();
}

Tftp::RawTftpPacket DataPacket::encode() const
{
  RawTftpPacket rawPacket( 4 + dataValue.size());

  insertHeader( rawPacket);

  RawTftpPacket::iterator packetIt = rawPacket.begin() + 2;

  // block number
  packetIt = setInt( packetIt, static_cast< uint16_t>( blockNumberValue));

  // data
  std::copy( dataValue.begin(), dataValue.end(), packetIt);

  return rawPacket;
}

void DataPacket::decodeBody( const RawTftpPacket &rawPacket)
{
  // check size
  if (rawPacket.size() < 4)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "Invalid packet size of DATA packet"));
  }

  auto packetIt{ rawPacket.begin() + 2};

  // decode block number
  packetIt = getInt< uint16_t>( packetIt, blockNumberValue);

  // copy data
  dataValue.assign( packetIt, rawPacket.end());
}

}
}
