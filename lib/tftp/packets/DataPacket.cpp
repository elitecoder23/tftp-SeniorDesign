// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::DataPacket.
 **/

#include "DataPacket.hpp"

#include <tftp/packets/PacketException.hpp>

#include <helper/Exception.hpp>
#include <helper/RawData.hpp>

#include <boost/exception/all.hpp>

#include <format>
#include <utility>

namespace Tftp::Packets {

DataPacket::DataPacket( BlockNumber blockNumber, Data data ) noexcept :
  Packet{ PacketType::Data },
  blockNumberV{ blockNumber },
  dataV{ std::move( data ) }
{
}

DataPacket::DataPacket( ConstRawDataSpan rawPacket ) :
  Packet{ PacketType::Data, rawPacket }
{
  decodeBody( rawPacket );
}

DataPacket& DataPacket::operator=( ConstRawDataSpan rawPacket )
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
  return std::format( "DATA: Block No: {} DATA: {} bytes", static_cast< uint16_t >( blockNumber() ), dataSize() );
}

RawData DataPacket::encode() const
{
  RawData rawPacket( MinPacketSize + dataV.size() );

  insertHeader( rawPacket );

  auto rawSpan{ RawDataSpan{ rawPacket }.subspan( HeaderSize ) };

  // block number
  rawSpan = Helper::RawData_setInt( rawSpan, static_cast< uint16_t >( blockNumberV ) );
  assert( rawSpan.size() == dataV.size() );

  // data
  std::ranges::copy( dataV, rawSpan.begin() );

  return rawPacket;
}

void DataPacket::decodeBody( ConstRawDataSpan rawPacket )
{
  // check size
  if ( rawPacket.size() < MinPacketSize )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo{ "Invalid packet size of DATA packet" } );
  }

  auto remaining{ ConstRawDataSpan{ rawPacket }.subspan( HeaderSize ) };

  // decode block number
  std::tie( remaining, static_cast< uint16_t & >( blockNumberV ) ) = Helper::RawData_getInt< uint16_t >( remaining );

  // copy data
  dataV.assign( remaining.begin(), remaining.end() );
}

}
