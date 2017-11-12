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

ReadWriteRequestPacket::string ReadWriteRequestPacket::getMode(
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
      //! @throw InvalidPacketException if mode cannot be decoded
      BOOST_THROW_EXCEPTION( InvalidPacketException() <<
        AdditionalInfo( "Invalid mode"));

      // never return
      return string();
  }
}

ReadWriteRequestPacket& ReadWriteRequestPacket::operator=(
  const RawTftpPacketType &rawPacket)
{
  Packet::operator =( rawPacket);
  decodeBody( rawPacket);
  return *this;
}

TransferMode ReadWriteRequestPacket::getMode( const string &mode)
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

  //! @throw InvalidPacketException When invalid mode has been set
  BOOST_THROW_EXCEPTION(
    InvalidPacketException() << AdditionalInfo( "Invalid string: " + mode));

  // never return
  return TransferMode::INVALID;
}

const ReadWriteRequestPacket::string& ReadWriteRequestPacket::getFilename() const
{
  return filename;
}

void ReadWriteRequestPacket::setFilename( const string &filename)
{
  this->filename = filename;
}

Tftp::TransferMode ReadWriteRequestPacket::getMode() const
{
  return getMode( mode);
}

void ReadWriteRequestPacket::setMode( const TransferMode mode)
{
  this->mode = getMode( mode);
}

void ReadWriteRequestPacket::setMode( const string &mode)
{
  this->mode = mode;
}

const Options::OptionList& ReadWriteRequestPacket::getOptions() const
{
  return options;
}

Options::OptionList& ReadWriteRequestPacket::getOptions()
{
  return options;
}

void ReadWriteRequestPacket::setOptions( const Options::OptionList &options)
{
  this->options = options;
}

const ReadWriteRequestPacket::string ReadWriteRequestPacket::getOption(
  const string &name) const
{
  Options::OptionPtr option( options.getOption( name));

  return (option) ? static_cast< string>( *option) : string();
}

void ReadWriteRequestPacket::setOption( const string &name, const string &value)
{
  options.setOption( name, value);
}

ReadWriteRequestPacket::operator string() const
{
  return (boost::format( "%s: FILE: \"%s\" MODE: \"%s\" OPT: \"%s\"") %
    Packet::operator string() %
    getFilename() %
    getMode( getMode()) %
    getOptions().toString()).str();
}

ReadWriteRequestPacket::ReadWriteRequestPacket(
  const PacketType packetType,
  const string &filename,
  const TransferMode mode,
  const Options::OptionList &options):
  Packet( packetType),
  filename( filename),
  mode( getMode( mode)),
  options( options)
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
  const RawTftpPacketType &rawPacket):
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

Tftp::RawTftpPacketType ReadWriteRequestPacket::encode() const
{
  Options::OptionList::RawOptionsType rawOptions( options.getRawOptions());

  RawTftpPacketType rawPacket(
    HeaderSize +
    filename.size() + 1 +
    mode.size() + 1 +
    rawOptions.size());

  insertHeader( rawPacket);

  RawTftpPacketType::iterator packetIt( rawPacket.begin() + HeaderSize);

  // decode filename
  packetIt = std::copy( filename.begin(), filename.end(), packetIt);
  *packetIt = 0;
  ++packetIt;

  // decode mode
  packetIt = std::copy( mode.begin(), mode.end(), packetIt);
  *packetIt = 0;
  ++packetIt;

  // decode options
  std::copy( rawOptions.begin(), rawOptions.end(), packetIt);

  return rawPacket;
}

void ReadWriteRequestPacket::decodeBody( const RawTftpPacketType &rawPacket)
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
  RawTftpPacketType::const_iterator filenameEnd =
    std::find( packetIt, rawPacket.end(), 0);

  if (filenameEnd == rawPacket.end())
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "No 0-termination for filename found"));
  }
  filename.assign( packetIt, filenameEnd);
  packetIt = filenameEnd + 1;

  // mode
  RawTftpPacketType::const_iterator modeEnd =
    std::find( packetIt, rawPacket.end(), 0);

  if (modeEnd == rawPacket.end())
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "No 0-termination for operation found"));
  }
  mode.assign( packetIt, modeEnd);
  packetIt = modeEnd + 1;

  // assign options
  options = Options::OptionList( packetIt, rawPacket.end());
}

}
}
