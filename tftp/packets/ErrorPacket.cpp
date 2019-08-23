/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::ErrorPacket.
 **/

#include "ErrorPacket.hpp"

#include <tftp/packets/PacketException.hpp>
#include <tftp/ErrorCodeDescription.hpp>

#include <helper/Endianess.hpp>

namespace Tftp::Packets {

ErrorPacket::ErrorPacket(
  const ErrorCode errorCode,
  std::string_view errorMessage):
  Packet{ PacketType::Error},
  errorCodeValue{ errorCode},
  errorMessageValue{ errorMessage}
{
}

ErrorPacket::ErrorPacket(
  ErrorCode errorCode,
  std::string &&errorMessage):
  Packet{ PacketType::Error},
  errorCodeValue{ errorCode},
  errorMessageValue{ std::move( errorMessage)}
{
}

ErrorPacket::ErrorPacket( const RawTftpPacket &rawPacket):
  Packet{ PacketType::Error, rawPacket},
  errorCodeValue{ ErrorCode::Invalid}
{
  decodeBody( rawPacket);
}

ErrorPacket& ErrorPacket::operator=(
  const RawTftpPacket &rawPacket)
{
  // call inherited operator
  Packet::operator =( rawPacket);
  decodeBody( rawPacket);
  return *this;
}

ErrorPacket::operator std::string() const
{
  return (boost::format( "ERR: EC: %s (%d) - DESC: \"%s\"") %
    ErrorCodeDescription::instance().name( errorCode()) %
    static_cast< uint16_t>( errorCode()) %
    errorMessage()).str();
}

Tftp::ErrorCode ErrorPacket::errorCode() const noexcept
{
  return errorCodeValue;
}

void ErrorPacket::errorCode( const ErrorCode errorCode) noexcept
{
  errorCodeValue = errorCode;
}

std::string_view ErrorPacket::errorMessage() const
{
  return errorMessageValue;
}

void ErrorPacket::errorMessage( std::string_view errorMessage)
{
  errorMessageValue = errorMessage;
}

void ErrorPacket::errorMessage( std::string &&errorMessage)
{
  errorMessageValue = std::move( errorMessage);
}

Tftp::RawTftpPacket ErrorPacket::encode() const
{
  RawTftpPacket rawPacket( 4U + errorMessageValue.length() + 1U);

  insertHeader( rawPacket);

  auto packetIt{ rawPacket.begin() + HeaderSize};

  // error code
  packetIt = setInt( packetIt, static_cast< uint16_t>( errorCodeValue));

  // error message
  packetIt = std::copy(
    errorMessageValue.begin(),
    errorMessageValue.end(),
    packetIt);
  *packetIt = 0;

  return rawPacket;
}

void ErrorPacket::decodeBody( const RawTftpPacket &rawPacket)
{
  // check size
  if (rawPacket.size() < 5U)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << AdditionalInfo( "Invalid packet size of ERROR packet"));
  }

  auto packetIt{ rawPacket.begin() + HeaderSize};

  // decode error code
  uint16_t errorCodeInt{};
  packetIt = getInt< uint16_t>( packetIt, errorCodeInt);
  errorCodeValue = static_cast< ErrorCode>( errorCodeInt);

  // check terminating 0 character
  if (rawPacket.back()!=0U)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << AdditionalInfo( "error message not 0-terminated"));
  }

  errorMessageValue = std::string{ packetIt, rawPacket.end()-1U};
}

}
