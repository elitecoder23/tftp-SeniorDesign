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
 * @brief Definition of class Tftp::Packets::BaseErrorPacket.
 **/

#include "BaseErrorPacket.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/ErrorCodeDescription.hpp>

#include <helper/Endianess.hpp>
#include <helper/Logger.hpp>

#include <map>

namespace Tftp {
namespace Packets {

BaseErrorPacket::BaseErrorPacket( const ErrorCode errorCode) noexcept:
  Packet( PacketType::Error),
  errorCode( errorCode)
{
}

BaseErrorPacket::BaseErrorPacket(
  const RawTftpPacketType &rawPacket):
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
  getInt< uint16_t>( packetIt, errorCodeInt);
  errorCode = static_cast< ErrorCode>( errorCodeInt);
}


Tftp::ErrorCode BaseErrorPacket::getErrorCode() const
{
  return errorCode;
}

void BaseErrorPacket::setErrorCode( const ErrorCode errorCode)
{
  this->errorCode = errorCode;
}

Tftp::RawTftpPacketType BaseErrorPacket::encode() const
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

BaseErrorPacket::operator string() const
{
  return (boost::format( "ERR: EC: %s (%d) - DESC: \"%s\"") %
    ErrorCodeDescription::getInstance().getDescription( getErrorCode()).name %
    static_cast< uint16_t>( getErrorCode()) %
    getErrorMessage()).str();
}

BaseErrorPacket::string BaseErrorPacket::getErrorMessage(
  const RawTftpPacketType &rawPacket)
{
  // check size
  if (rawPacket.size() < 5)
  {
    //! @throw InvalidPacketException When the packet size is to small.
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "Invalid packet size of ERROR packet"));
  }

  RawTftpPacketType::const_iterator packetIt = rawPacket.begin() + 4;

  // check terminating 0 character
  if (rawPacket.back()!=0)
  {
    //! @throw InvalidPacketException When the error message is not 0-terminated.
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "error message not 0-terminated"));
  }

  return string( packetIt, rawPacket.end()-1);
}

}
}
