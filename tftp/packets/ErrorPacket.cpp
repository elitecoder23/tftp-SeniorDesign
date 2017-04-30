/*
 * $Date$
 * $Revision$
 */
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class Tftp::Packets::ErrorPacket.
 **/

#include "ErrorPacket.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/ErrorCodeDescription.hpp>

#include <helper/Endianess.hpp>

namespace Tftp {
namespace Packets {

ErrorPacket::ErrorPacket(
  const ErrorCode errorCode,
  const string &errorMessage) noexcept:
  Packet( PacketType::Error),
  errorCode( errorCode),
  errorMessage( errorMessage)
{
}

ErrorPacket::ErrorPacket( const RawTftpPacketType &rawPacket):
  Packet( PacketType::Error, rawPacket)
{
  // check size
  if (rawPacket.size() < 5)
  {
    //! @throw InvalidPacketException When rawPacket is not an valid packet.
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "Invalid packet size of ERROR packet"));
  }

  RawTftpPacketType::const_iterator packetIt( rawPacket.begin() + HeaderSize);

  // decode error code
  uint16_t errorCodeInt;
  packetIt = getInt< uint16_t>( packetIt, errorCodeInt);
  errorCode = static_cast< ErrorCode>( errorCodeInt);

  // check terminating 0 character
  if (rawPacket.back()!=0)
  {
    //! @throw InvalidPacketException When the error message is not 0-terminated.
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "error message not 0-terminated"));
  }

  errorMessage = string( packetIt, rawPacket.end()-1);
}

Tftp::RawTftpPacketType ErrorPacket::encode() const
{
  const string errorMessage = getErrorMessage();

  RawTftpPacketType rawPacket( 4 + errorMessage.length() + 1);

  insertHeader( rawPacket);

  RawTftpPacketType::iterator packetIt( rawPacket.begin() + HeaderSize);

  // error code
  packetIt = setInt( packetIt, static_cast< const uint16_t>( getErrorCode()));

  // error message
  packetIt = std::copy( errorMessage.begin(), errorMessage.end(), packetIt);
  *packetIt = 0;

  return rawPacket;
}

ErrorPacket::operator string() const
{
  return (boost::format( "ERR: EC: %s (%d) - DESC: \"%s\"") %
    ErrorCodeDescription::getInstance().getDescription( getErrorCode()).name %
    static_cast< uint16_t>( getErrorCode()) %
    getErrorMessage()).str();
}

Tftp::ErrorCode ErrorPacket::getErrorCode() const
{
  return errorCode;
}

void ErrorPacket::setErrorCode( const ErrorCode errorCode)
{
  this->errorCode = errorCode;
}

ErrorPacket::string ErrorPacket::getErrorMessage() const
{
  return errorMessage;
}

void ErrorPacket::setErrorMessage( const string &errorMessage)
{
  this->errorMessage = errorMessage;
}

}
}
