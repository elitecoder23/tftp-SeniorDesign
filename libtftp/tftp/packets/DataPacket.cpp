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
 * @brief Definition of class Tftp::Packets::DataPacket.
 **/

#include "DataPacket.hpp"

#include <tftp/TftpException.hpp>

#include <helper/Endianess.hpp>
#include <helper/Logger.hpp>

namespace Tftp {
namespace Packets {

DataPacket::DataPacket(
  BlockNumber blockNumber,
  const std::vector<uint8_t> &data) noexcept:
  Packet( PacketType::Data),
  blockNumber( blockNumber),
  data( data)
{
}

DataPacket::DataPacket(
  const RawTftpPacketType &rawPacket):
  Packet( PacketType::Data, rawPacket)
{
  // check size
  if (rawPacket.size() < 4)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "Invalid packet size of DATA packet"));
  }

  RawTftpPacketType::const_iterator packetIt = rawPacket.begin() + 2;

  // decode block number
  packetIt = getInt< uint16_t>( packetIt, blockNumber);

  // copy data
  data.assign( packetIt, rawPacket.end());
}

BlockNumber DataPacket::getBlockNumber() const
{
  return blockNumber;
}

BlockNumber& DataPacket::getBlockNumber()
{
  return blockNumber;
}

void DataPacket::setBlockNumber( BlockNumber blockBumber)
{
  this->blockNumber = blockBumber;
}

const DataPacket::DataType& DataPacket::getData() const
{
  return data;
}

DataPacket::DataType& DataPacket::getData()
{
  return data;
}

void DataPacket::setData( const DataType &data)
{
  this->data = data;
}

void DataPacket::setData( DataType &&data)
{
  this->data = std::move( data);
}

size_t DataPacket::getDataSize() const
{
  return data.size();
}

Tftp::RawTftpPacketType DataPacket::encode() const
{
  RawTftpPacketType rawPacket( 4 + data.size());

  insertHeader( rawPacket);

  RawTftpPacketType::iterator packetIt = rawPacket.begin() + 2;

  // block number
  packetIt = setInt( packetIt, static_cast< uint16_t>( getBlockNumber()));

  // data
  std::copy( data.begin(), data.end(), packetIt);

  return rawPacket;
}

DataPacket::string DataPacket::toString() const
{
  return (boost::format( "DATA: BLOCKNO: %d DATA: %d bytes") %
    getBlockNumber() %
    getDataSize()).str();
}

}
}
