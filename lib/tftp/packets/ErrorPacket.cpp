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
 * @brief Definition of Class Tftp::Packets::ErrorPacket.
 **/

#include "ErrorPacket.hpp"

#include <tftp/packets/PacketException.hpp>
#include <tftp/packets/ErrorCodeDescription.hpp>

#include <helper/Endianness.hpp>
#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

#include <format>

namespace Tftp::Packets {

ErrorPacket::ErrorPacket(
  const ErrorCode errorCode,
  std::string errorMessage ):
  Packet{ PacketType::Error },
  errorCodeV{ errorCode },
  errorMessageV{ std::move( errorMessage ) }
{
}

ErrorPacket::ErrorPacket( ConstRawTftpPacketSpan rawPacket ):
  Packet{ PacketType::Error, rawPacket },
  errorCodeV{ ErrorCode::Invalid }
{
  decodeBody( rawPacket );
}

ErrorPacket& ErrorPacket::operator=( ConstRawTftpPacketSpan rawPacket )
{
  decodeHeader( rawPacket );
  decodeBody( rawPacket );
  return *this;
}

ErrorPacket::operator std::string() const
{
  return std::format(
    "ERR: EC: {} ({}) - DESC: \"{}\"",
    ErrorCodeDescription::instance().name( errorCode() ),
    static_cast< uint16_t>( errorCode() ),
    errorMessage() );
}

ErrorCode ErrorPacket::errorCode() const noexcept
{
  return errorCodeV;
}

void ErrorPacket::errorCode( const ErrorCode errorCode ) noexcept
{
  errorCodeV = errorCode;
}

std::string_view ErrorPacket::errorMessage() const
{
  return errorMessageV;
}

void ErrorPacket::errorMessage( std::string errorMessage )
{
  errorMessageV = std::move( errorMessage );
}

RawTftpPacket ErrorPacket::encode() const
{
  RawTftpPacket rawPacket( 4U + errorMessageV.length() + 1U );

  insertHeader( rawPacket);

  auto packetIt{ rawPacket.begin() + HeaderSize };

  // error code
  packetIt = Helper::setInt( packetIt, static_cast< uint16_t>( errorCodeV ) );

  // error message
  packetIt = std::copy(
    errorMessageV.begin(),
    errorMessageV.end(),
    packetIt );
  *packetIt = 0;

  return rawPacket;
}

void ErrorPacket::decodeBody( ConstRawTftpPacketSpan rawPacket )
{
  // check size
  if ( rawPacket.size() < MinPacketSize )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo{ "Invalid packet size of ERROR packet" } );
  }

  auto packetIt{ rawPacket.begin() + HeaderSize};

  // decode error code
  uint16_t errorCodeInt{};
  packetIt = Helper::getInt< uint16_t >( packetIt, errorCodeInt );
  errorCodeV = static_cast< ErrorCode>( errorCodeInt );

  // check terminating 0 character
  if ( rawPacket.back() != 0U)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo{ "error message not 0-terminated" } );
  }

  errorMessageV = std::string{ packetIt, rawPacket.end()-1U };
}

}
