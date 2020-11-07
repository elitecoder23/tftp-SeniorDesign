/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::ReadWriteRequestPacket.
 **/

#include "ReadWriteRequestPacket.hpp"

#include <tftp/packets/PacketException.hpp>

#include <tftp/options/OptionList.hpp>

#include <helper/Endianess.hpp>

#include <algorithm>

namespace Tftp::Packets {

std::string_view ReadWriteRequestPacket::decodeMode(
  const TransferMode mode)
{
  switch (mode)
  {
    case TransferMode::OCTET:
      return "OCTET";

    case TransferMode::NETASCII:
      return "NETASCII";

    case TransferMode::MAIL:
      return "MAIL";

    default:
      return {};
  }
}

TransferMode ReadWriteRequestPacket::decodeMode( std::string_view mode)
{
  //! @todo check implementation of transform
  std::string upperMode{ mode};

  std::transform(
    upperMode.begin(),
    upperMode.end(),
    upperMode.begin(),
    toupper);

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

void ReadWriteRequestPacket::filename( std::string_view filename)
{
  filenameV = filename;
}

void ReadWriteRequestPacket::filename( std::string &&filename)
{
  filenameV = std::move( filename);
}

Tftp::TransferMode ReadWriteRequestPacket::mode() const
{
  return modeV;
}

void ReadWriteRequestPacket::mode( const TransferMode mode)
{
  modeV = mode;
}

const Options::Options& ReadWriteRequestPacket::options() const
{
  return optionsV;
}

Options::Options& ReadWriteRequestPacket::options()
{
  return optionsV;
}

void ReadWriteRequestPacket::options( const Options::Options &options)
{
  optionsV = options;
}

void ReadWriteRequestPacket::options( Options::Options &&options)
{
  optionsV = std::move( options);
}

ReadWriteRequestPacket::operator std::string() const
{
  return (boost::format( "%s: FILE: \"%s\" MODE: \"%s\" OPT: \"%s\"") %
    Packet::operator std::string() %
    filenameV %
    decodeMode( modeV) %
    Options::OptionList::toString( optionsV)).str();
}

ReadWriteRequestPacket::ReadWriteRequestPacket(
  const PacketType packetType,
  std::string_view filename,
  const TransferMode mode,
  const Options::Options &options):
  Packet{ packetType},
  filenameV{ filename},
  modeV{ mode},
  optionsV{ options}
{
  switch ( packetType)
  {
    case PacketType::ReadRequest:
    case PacketType::WriteRequest:
      break;

    default:
      BOOST_THROW_EXCEPTION( InvalidPacketException()
        << Helper::AdditionalInfo( "Wrong packet type supplied only RRQ/WRW allowed"));
      /* no break - because BOOST_THROW_EXCEPTION throws */
  }
}

ReadWriteRequestPacket::ReadWriteRequestPacket(
  const PacketType packetType,
  std::string &&filename,
  const TransferMode mode,
  Options::Options &&options):
  Packet{ packetType},
  filenameV{ std::move( filename)},
  modeV{ mode},
  optionsV{ std::move( options)}
{
  switch ( packetType)
  {
    case PacketType::ReadRequest:
    case PacketType::WriteRequest:
      break;

    default:
      BOOST_THROW_EXCEPTION( InvalidPacketException()
        << Helper::AdditionalInfo( "Wrong packet type supplied only RRQ/WRW allowed"));
      /* no break - because BOOST_THROW_EXCEPTION throws */
  }
}

ReadWriteRequestPacket::ReadWriteRequestPacket(
  const PacketType packetType,
  const RawTftpPacket &rawPacket):
  Packet{ packetType, rawPacket},
  modeV{}
{
  switch (packetType)
  {
    case PacketType::ReadRequest:
    case PacketType::WriteRequest:
      break;

    default:
      BOOST_THROW_EXCEPTION( InvalidPacketException()
        << Helper::AdditionalInfo( "Wrong packet type supplied only RRQ/WRW allowed"));
      /* no break - because BOOST_THROW_EXCEPTION throws */
  }

  decodeBody( rawPacket);
}

void ReadWriteRequestPacket::decodeBody( const RawTftpPacket &rawPacket)
{
  auto packetIt{ rawPacket.begin() + HeaderSize};

  // check size
  if ( rawPacket.size() <= HeaderSize)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo( "Invalid packet size of RRQ/WRQ packet"));
  }

  // check terminating 0 character
  if ( rawPacket.back() != 0 )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo( "RRQ/WRQ message not 0-terminated" ));
  }

  // filename
  auto filenameEnd{ std::find( packetIt, rawPacket.end(), 0 ) };

  if ( filenameEnd == rawPacket.end())
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo( "No 0-termination for filename found"));
  }
  filenameV.assign( packetIt, filenameEnd);
  packetIt = filenameEnd + 1U;

  // transfer mode
  auto modeEnd{ std::find( packetIt, rawPacket.end(), 0)};

  if (modeEnd == rawPacket.end())
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo( "No 0-termination for operation found"));
  }
  modeV = decodeMode( std::string{ packetIt, modeEnd});
  packetIt = modeEnd + 1U;

  // assign options
  optionsV = Options::OptionList::options( packetIt, rawPacket.end());
}

Tftp::RawTftpPacket ReadWriteRequestPacket::encode() const
{
  const auto mode{ decodeMode( modeV)};
  const auto rawOptions{ Options::OptionList::rawOptions( optionsV)};

  RawTftpPacket rawPacket(
    HeaderSize +
    filenameV.size() + 1U +
    mode.size() + 1U +
    rawOptions.size());

  insertHeader( rawPacket);

  auto packetIt{ rawPacket.begin() + HeaderSize};

  // encode filename
  packetIt = std::copy( filenameV.begin(), filenameV.end(), packetIt);
  *packetIt = 0;
  ++packetIt;

  // encode transfer mode
  packetIt = std::copy( mode.begin(), mode.end(), packetIt);
  *packetIt = 0;
  ++packetIt;

  // encode options
  std::copy( rawOptions.begin(), rawOptions.end(), packetIt);

  return rawPacket;
}

}
