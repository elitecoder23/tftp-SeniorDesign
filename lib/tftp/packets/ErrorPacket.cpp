// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::ErrorPacket.
 **/

#include "ErrorPacket.hpp"

#include <tftp/packets/ErrorCodeDescription.hpp>
#include <tftp/packets/PacketException.hpp>

#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

#include <format>
#include <utility>

namespace Tftp::Packets {

ErrorPacket::ErrorPacket() :
  Packet{ PacketType::Error }
{
}

ErrorPacket::ErrorPacket( const ErrorCode errorCode, std::string errorMessage ) :
  Packet{ PacketType::Error },
  errorCodeV{ errorCode },
  errorMessageV{ std::move( errorMessage ) }
{
}

ErrorPacket::ErrorPacket( const Helper::ConstRawDataSpan rawPacket ) :
  Packet{ PacketType::Error, rawPacket }
{
  decodeBody( rawPacket );
}

ErrorPacket& ErrorPacket::operator=( const Helper::ConstRawDataSpan rawPacket )
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
    static_cast< uint16_t >( errorCode() ),
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

ErrorInformation ErrorPacket::errorInformation() const
{
  return ErrorInformation{ std::make_tuple( errorCodeV, errorMessageV ) };
}

Helper::RawData ErrorPacket::encode() const
{
  Helper::RawData rawPacket( MinPacketSize + errorMessageV.length() );

  insertHeader( rawPacket );

  auto rawSpan{ Helper::RawDataSpan{ rawPacket }.subspan( HeaderSize ) };

  // error code
  rawSpan = Helper::RawData_setInt( rawSpan, static_cast< uint16_t >( errorCodeV ) );

  // error message
  rawSpan = Helper::RawData_setString( rawSpan, errorMessageV );
  rawSpan.back() = std::byte{ 0 };

  return rawPacket;
}

void ErrorPacket::decodeBody( Helper::ConstRawDataSpan rawPacket )
{
  // check size
  if ( rawPacket.size() < MinPacketSize )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException{}
      << Helper::AdditionalInfo{ "Invalid packet size of ERROR packet" } );
  }

  auto rawSpan{ Helper::ConstRawDataSpan{ rawPacket }.subspan( HeaderSize ) };

  // decode error code
  uint16_t errorCodeInt{};
  std::tie( rawSpan, errorCodeInt ) = Helper::RawData_getInt< uint16_t >( rawSpan );
  errorCodeV = static_cast< ErrorCode >( errorCodeInt );

  // check terminating 0 character
  if ( rawSpan.back() != std::byte{ 0 } )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException{}
      << Helper::AdditionalInfo{ "error message not 0-terminated" } );
  }

  std::tie( rawSpan, errorMessageV ) = Helper::RawData_getString( rawSpan, rawSpan.size() - 1U );
}

}
