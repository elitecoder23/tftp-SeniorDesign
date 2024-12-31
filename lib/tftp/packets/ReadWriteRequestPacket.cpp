// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::ReadWriteRequestPacket.
 **/

#include "ReadWriteRequestPacket.hpp"

#include <tftp/packets/PacketException.hpp>
#include <tftp/packets/Options.hpp>

#include <helper/Exception.hpp>
#include <helper/RawData.hpp>

#include <boost/exception/all.hpp>

#include <algorithm>
#include <format>
#include <utility>

namespace Tftp::Packets {

std::string_view ReadWriteRequestPacket::decodeMode( const TransferMode mode )
{
  switch ( mode )
  {
    using enum TransferMode;

    case OCTET:
      return "OCTET";

    case NETASCII:
      return "NETASCII";

    case MAIL:
      return "MAIL";

    default:
      return {};
  }
}

TransferMode ReadWriteRequestPacket::decodeMode( std::string_view mode )
{
  //! @todo check implementation of transform
  std::string upperMode{ mode };

  std::transform( upperMode.begin(), upperMode.end(), upperMode.begin(), toupper );

  if ( upperMode == "OCTET" )
  {
    return TransferMode::OCTET;
  }

  if ( upperMode == "NETASCII" )
  {
    return TransferMode::NETASCII;
  }

  if ( upperMode == "MAIL" )
  {
    return TransferMode::MAIL;
  }

  return TransferMode::Invalid;
}

std::string_view ReadWriteRequestPacket::filename() const
{
  return filenameV;
}

void ReadWriteRequestPacket::filename( std::string filename )
{
  filenameV = std::move( filename );
}

TransferMode ReadWriteRequestPacket::mode() const
{
  return modeV;
}

void ReadWriteRequestPacket::mode( const TransferMode mode )
{
  modeV = mode;
}

const Options& ReadWriteRequestPacket::options() const
{
  return optionsV;
}

Options& ReadWriteRequestPacket::options()
{
  return optionsV;
}

void ReadWriteRequestPacket::options( Options options )
{
  optionsV = std::move( options);
}

ReadWriteRequestPacket::operator std::string() const
{
  return std::format(
    "{}: FILE: \"{}\" MODE: \"{}\" OPT: \"{}\"",
    Packet::operator std::string(),
    filenameV,
    decodeMode( modeV ),
    Options_toString( optionsV ) );
}

ReadWriteRequestPacket::ReadWriteRequestPacket(
  const PacketType packetType,
  std::string filename,
  const TransferMode mode,
  Options options ) :
  Packet{ packetType },
  filenameV{ std::move( filename ) },
  modeV{ mode },
  optionsV{ std::move( options ) }
{
  switch ( packetType )
  {
    case PacketType::ReadRequest:
    case PacketType::WriteRequest:
      break;

    default:
      BOOST_THROW_EXCEPTION( InvalidPacketException()
        << Helper::AdditionalInfo{
          "Wrong packet type supplied only RRQ/WRW allowed" } );
      /* no break - because BOOST_THROW_EXCEPTION throws */
  }
}

ReadWriteRequestPacket::ReadWriteRequestPacket( const PacketType packetType, ConstRawDataSpan rawPacket ) :
  Packet{ packetType, rawPacket },
  modeV{}
{
  switch ( packetType )
  {
    case PacketType::ReadRequest:
    case PacketType::WriteRequest:
      break;

    default:
      BOOST_THROW_EXCEPTION( InvalidPacketException()
        << Helper::AdditionalInfo{
          "Wrong packet type supplied only RRQ/WRW allowed" } );
      /* no break - because BOOST_THROW_EXCEPTION throws */
  }

  decodeBody( rawPacket );
}

void ReadWriteRequestPacket::decodeBody( ConstRawDataSpan rawPacket )
{
  // check size
  if ( rawPacket.size() <= HeaderSize )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo{ "Invalid packet size of RRQ/WRQ packet" } );
  }

  auto rawSpan{ ConstRawDataSpan{ rawPacket }.subspan( HeaderSize ) };

  // check terminating 0 character
  if ( rawSpan.back() != std::byte{ 0 } )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo{ "RRQ/WRQ message not 0-terminated" } );
  }

  auto [ _, rawRequestString ]{ Helper::RawData_getString( rawSpan, rawSpan.size() ) };

  // filename
  const auto filenameEnd{ rawRequestString.find( '\0' ) };

  if ( filenameEnd == std::string_view::npos )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo{ "No 0-termination for filename found" } );
  }

  filenameV = rawRequestString.substr( 0, filenameEnd );
  rawRequestString = rawRequestString.substr( filenameEnd + 1U );

  // transfer mode
  const auto modeEnd{ rawRequestString.find( '\0' ) };

  if ( modeEnd == std::string_view::npos )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo{ "No 0-termination for operation found" } );
  }

  modeV = decodeMode( rawRequestString.substr( 0, modeEnd ) );
  rawRequestString = rawRequestString.substr( modeEnd + 1U );

  // assign options
  optionsV = Options_options( rawRequestString );
}

RawData ReadWriteRequestPacket::encode() const
{
  const auto mode{ decodeMode( modeV ) };
  const auto rawOptions{ Options_rawOptions( optionsV ) };

  RawData rawPacket( HeaderSize + filenameV.size() + 1U + mode.size() + 1U + rawOptions.size() );

  insertHeader( rawPacket );

  auto rawSpan{ RawDataSpan{ rawPacket }.subspan( HeaderSize ) };

  // encode filename
  auto rawFilename{ Helper::RawData_toRawData( filenameV  ) };
  auto filenameEnd{ std::copy( rawFilename.begin(), rawFilename.end(), rawSpan.begin() ) };
  *filenameEnd = std::byte{ 0 };
  ++filenameEnd;

  // encode transfer mode
  auto rawMode{ Helper::RawData_toRawData( mode ) };
  auto modeEnd{ std::copy( rawMode.begin(), rawMode.end(), filenameEnd ) };
  *modeEnd = std::byte{ 0 };
  ++modeEnd;

  // encode options
  std::copy( rawOptions.begin(), rawOptions.end(), modeEnd );

  return rawPacket;
}

}
