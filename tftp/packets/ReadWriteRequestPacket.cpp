/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class Tftp::Packets::ReadWriteRequestPacket.
 **/

#include "ReadWriteRequestPacket.hpp"

#include <tftp/TftpException.hpp>

#include <helper/Endianess.hpp>

#include <algorithm>

namespace Tftp {
namespace Packets {

ReadWriteRequestPacket::string ReadWriteRequestPacket::decodeMode(
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
      return string();
  }
}

TransferMode ReadWriteRequestPacket::decodeMode( const std::string &mode)
{
  //! @todo check implementation of transform
  string upperMode = mode;

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
  Packet::operator =( rawPacket);
  decodeBody( rawPacket);
  return *this;
}

const std::string& ReadWriteRequestPacket::filename() const
{
  return filenameValue;
}

void ReadWriteRequestPacket::filename( const std::string &filename)
{
  filenameValue = filename;
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

const ReadWriteRequestPacket::string ReadWriteRequestPacket::option(
  const std::string &name) const
{
  Options::OptionPtr option( optionsValue.get( name));

  return (option) ? static_cast< string>( *option) : string();
}

void ReadWriteRequestPacket::option(
  const std::string &name,
  const std::string &value)
{
  optionsValue.set( name, value);
}

ReadWriteRequestPacket::operator std::string() const
{
  return (boost::format( "%s: FILE: \"%s\" MODE: \"%s\" OPT: \"%s\"") %
    Packet::operator string() %
    filenameValue %
    decodeMode( modeValue) %
    optionsValue.toString()).str();
}

ReadWriteRequestPacket::ReadWriteRequestPacket(
  const PacketType packetType,
  const string &filename,
  const TransferMode mode,
  const Options::OptionList &options):
  Packet( packetType),
  filenameValue( filename),
  modeValue( mode),
  optionsValue( options)
{
  switch (packetType)
  {
    case PacketType::ReadRequest:
    case PacketType::WriteRequest:
      break;

    default:
      BOOST_THROW_EXCEPTION( InvalidPacketException() <<
        AdditionalInfo( "Wrong packet type supplied only RRQ/WRW allowed"));
      /* no break - because BOOST_THROW_EXCEPTION throws */
  }
}

ReadWriteRequestPacket::ReadWriteRequestPacket(
  const PacketType packetType,
  const RawTftpPacket &rawPacket):
  Packet( packetType, rawPacket)
{
  switch (packetType)
  {
    case PacketType::ReadRequest:
    case PacketType::WriteRequest:
      break;

    default:
      BOOST_THROW_EXCEPTION( InvalidPacketException() <<
        AdditionalInfo( "Wrong packet type supplied only RRQ/WRW allowed"));
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

  RawTftpPacket::iterator packetIt( rawPacket.begin() + HeaderSize);

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
  if (rawPacket.size() <= 2)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "Invalid packet size of RRQ/WRQ packet"));
  }

  // check terminating 0 character
  if (rawPacket.back()!=0)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "RRQ/WRQ message not 0-terminated"));
  }

  // filename
  RawTftpPacket::const_iterator filenameEnd =
    std::find( packetIt, rawPacket.end(), 0);

  if (filenameEnd == rawPacket.end())
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "No 0-termination for filename found"));
  }
  filenameValue.assign( packetIt, filenameEnd);
  packetIt = filenameEnd + 1;

  // transfer mode
  RawTftpPacket::const_iterator modeEnd =
    std::find( packetIt, rawPacket.end(), 0);

  if (modeEnd == rawPacket.end())
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "No 0-termination for operation found"));
  }
  modeValue = decodeMode( string{ packetIt, modeEnd});
  packetIt = modeEnd + 1;

  // assign options
  optionsValue = Options::OptionList( packetIt, rawPacket.end());
}

}
}
