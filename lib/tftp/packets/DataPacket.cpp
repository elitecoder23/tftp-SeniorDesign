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
 * @brief Definition of Class Tftp::Packets::DataPacket.
 **/

#include "DataPacket.hpp"

#include <tftp/packets/PacketException.hpp>

#include <helper/Endianness.hpp>
#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

#include <format>
#include <utility>

namespace Tftp::Packets {

DataPacket::DataPacket(
  BlockNumber blockNumber,
  Data data ) noexcept :
  Packet{ PacketType::Data },
  blockNumberV{ blockNumber },
  dataV{ std::move( data ) }
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

void DataPacket::data( Data data )
{
  dataV = std::move( data );
}

size_t DataPacket::dataSize() const
{
  return dataV.size();
}

DataPacket::operator std::string() const
{
  return std::format(
    "DATA: BLOCK NO: {} DATA: {} bytes",
    static_cast< uint16_t>( blockNumber() ),
    dataSize() );
}

RawTftpPacket DataPacket::encode() const
{
  RawTftpPacket rawPacket( MinPacketSize + dataV.size() );

  insertHeader( rawPacket );

  RawTftpPacketSpan rawSpan{ rawPacket.begin() + HeaderSize, rawPacket.end() };

  // block number
  rawSpan = Helper::setInt( rawSpan, static_cast< uint16_t >( blockNumberV ) );
  assert( rawSpan.size() == dataV.size() );

  // data
  std::ranges::copy( dataV, rawSpan.begin() );

  return rawPacket;
}

void DataPacket::decodeBody( ConstRawTftpPacketSpan rawPacket )
{
  // check size
  if ( rawPacket.size() < MinPacketSize )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo{ "Invalid packet size of DATA packet" } );
  }

  ConstRawTftpPacketSpan rawSpan{
    rawPacket.begin() + HeaderSize,
    rawPacket.end() };

  // decode block number
  std::tie( rawSpan, static_cast< uint16_t & >( blockNumberV ) ) =
    Helper::getInt< uint16_t >( rawSpan );

  // copy data
  dataV.assign( rawSpan.begin(), rawPacket.end() );
}

}
