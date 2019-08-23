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

  if (upperMode=="OCTET")
  {
    return TransferMode::OCTET;
  }

  if (upperMode=="NETASCII")
  {
    return TransferMode::NETASCII;
  }

  if (upperMode=="MAIL")
  {
    return TransferMode::MAIL;
  }

  return TransferMode::Invalid;
}

ReadWriteRequestPacket& ReadWriteRequestPacket::operator=(
  const RawTftpPacket &rawPacket)
{
  // call inherited operator
  Packet::operator =( rawPacket);
  // decode body
  decodeBody( rawPacket);
  return *this;
}

std::string_view ReadWriteRequestPacket::filename() const
{
  return filenameValue;
}

void ReadWriteRequestPacket::filename( std::string_view filename)
{
  filenameValue = filename;
}

void ReadWriteRequestPacket::filename( std::string &&filename)
{
  filenameValue = std::move( filename);
}

Tftp::TransferMode ReadWriteRequestPacket::mode() const
{
  return modeValue;
}

void ReadWriteRequestPacket::mode( const TransferMode mode)
{
  modeValue = mode;
}

const Options::OptionList& ReadWriteRequestPacket::options() const
{
  return optionsValue;
}

Options::OptionList& ReadWriteRequestPacket::options()
{
  return optionsValue;
}

void ReadWriteRequestPacket::options( const Options::OptionList &options)
{
  optionsValue = options;
}

void ReadWriteRequestPacket::options( Options::OptionList &&options)
{
  optionsValue = std::move( options);
}

std::string ReadWriteRequestPacket::option(
  std::string_view name) const
{
  auto  option{ optionsValue.get( name)};

  return (option) ? static_cast< std::string>( *option) : std::string();
}

void ReadWriteRequestPacket::option(
  std::string_view name,
  std::string_view value)
{
  optionsValue.set( name, value);
}

void ReadWriteRequestPacket::option(
  std::string &&name,
  std::string &&value)
{
  optionsValue.set( std::move( name), std::move( value));
}

ReadWriteRequestPacket::operator std::string() const
{
  return (boost::format( "%s: FILE: \"%s\" MODE: \"%s\" OPT: \"%s\"") %
    Packet::operator std::string() %
    filenameValue %
    decodeMode( modeValue) %
    optionsValue.toString()).str();
}

ReadWriteRequestPacket::ReadWriteRequestPacket(
  const PacketType packetType,
  std::string_view filename,
  const TransferMode mode,
  const Options::OptionList &options):
  Packet{ packetType},
  filenameValue{ filename},
  modeValue{ mode},
  optionsValue{ options}
{
  switch (packetType)
  {
    case PacketType::ReadRequest:
    case PacketType::WriteRequest:
      break;

    default:
      BOOST_THROW_EXCEPTION( InvalidPacketException()
        << AdditionalInfo( "Wrong packet type supplied only RRQ/WRW allowed"));
      /* no break - because BOOST_THROW_EXCEPTION throws */
  }
}

ReadWriteRequestPacket::ReadWriteRequestPacket(
  const PacketType packetType,
  std::string &&filename,
  const TransferMode mode,
  Options::OptionList &&options):
  Packet{ packetType},
  filenameValue{ std::move( filename)},
  modeValue{ mode},
  optionsValue{ std::move( options)}
{
  switch (packetType)
  {
    case PacketType::ReadRequest:
    case PacketType::WriteRequest:
      break;

    default:
      BOOST_THROW_EXCEPTION( InvalidPacketException()
        << AdditionalInfo( "Wrong packet type supplied only RRQ/WRW allowed"));
      /* no break - because BOOST_THROW_EXCEPTION throws */
  }
}

ReadWriteRequestPacket::ReadWriteRequestPacket(
  const PacketType packetType,
  const RawTftpPacket &rawPacket):
  Packet{ packetType, rawPacket},
  modeValue{}
{
  switch (packetType)
  {
    case PacketType::ReadRequest:
    case PacketType::WriteRequest:
      break;

    default:
      BOOST_THROW_EXCEPTION( InvalidPacketException()
        << AdditionalInfo( "Wrong packet type supplied only RRQ/WRW allowed"));
      /* no break - because BOOST_THROW_EXCEPTION throws */
  }

  decodeBody( rawPacket);
}

Tftp::RawTftpPacket ReadWriteRequestPacket::encode() const
{
  const auto mode{ decodeMode( modeValue)};
  const auto rawOptions{ optionsValue.rawOptions()};

  RawTftpPacket rawPacket(
    HeaderSize +
    filenameValue.size() + 1 +
    mode.size() + 1 +
    rawOptions.size());

  insertHeader( rawPacket);

  auto packetIt{ rawPacket.begin() + HeaderSize};

  // encode filename
  packetIt = std::copy( filenameValue.begin(), filenameValue.end(), packetIt);
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

void ReadWriteRequestPacket::decodeBody( const RawTftpPacket &rawPacket)
{
  auto packetIt( rawPacket.begin() + HeaderSize);

  // check size
  if (rawPacket.size() <= HeaderSize)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << AdditionalInfo( "Invalid packet size of RRQ/WRQ packet"));
  }

  // check terminating 0 character
  if (rawPacket.back()!=0)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << AdditionalInfo( "RRQ/WRQ message not 0-terminated"));
  }

  // filename
  auto filenameEnd{ std::find( packetIt, rawPacket.end(), 0)};

  if (filenameEnd == rawPacket.end())
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << AdditionalInfo( "No 0-termination for filename found"));
  }
  filenameValue.assign( packetIt, filenameEnd);
  packetIt = filenameEnd + 1;

  // transfer mode
  auto modeEnd{ std::find( packetIt, rawPacket.end(), 0)};

  if (modeEnd == rawPacket.end())
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << AdditionalInfo( "No 0-termination for operation found"));
  }
  modeValue = decodeMode( std::string{ packetIt, modeEnd});
  packetIt = modeEnd + 1;

  // assign options
  optionsValue = Options::OptionList( packetIt, rawPacket.end());
}

}
