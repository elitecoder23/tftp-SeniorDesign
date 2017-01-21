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
 * @brief Definition of class Tftp::Packet::BaseErrorPacket.
 **/

#include "BaseErrorPacket.hpp"

#include <tftp/TftpException.hpp>

#include <helper/Endianess.hpp>
#include <helper/Logger.hpp>

#include <map>

namespace Tftp {
namespace Packet {

BaseErrorPacket::string BaseErrorPacket::getErrorCodeString( ErrorCode errorCode) noexcept
{
  typedef std::map< ErrorCode, std::string> ErrorCodeMap;
  static const ErrorCodeMap errorCodes =
  {
    std::make_pair( ErrorCode::NOT_DEFINED,                     "NOT_DEFINED"),
    std::make_pair( ErrorCode::FILE_NOT_FOUND,                  "FILE_NOT_FOUND"),
    std::make_pair( ErrorCode::ACCESS_VIOLATION,                "ACCESS_VIOLATION"),
    std::make_pair( ErrorCode::DISK_FULL_OR_ALLOCATION_EXCEEDS, "DISK_FULL_OR_ALLOCATION_EXCEEDS"),
    std::make_pair( ErrorCode::ILLEGAL_TFTP_OPERATION,          "ILLEGAL_TFTP_OPERATION"),
    std::make_pair( ErrorCode::UNKNOWN_TRANSFER_ID,             "UNKNOWN_TRANSFER_ID"),
    std::make_pair( ErrorCode::FILE_ALLREADY_EXISTS,            "FILE_ALLREADY_EXISTS"),
    std::make_pair( ErrorCode::NO_SUCH_USER,                    "NO_SUCH_USER"),
    std::make_pair( ErrorCode::TFTP_OPTION_REFUSED,             "TFTP_OPTION_REFUSED"),
  };

  ErrorCodeMap::const_iterator it = errorCodes.find( errorCode);

  return (errorCodes.end() == it) ? "***UNKNOWN***" : it->second;
}

BaseErrorPacket::BaseErrorPacket( const ErrorCode errorCode) noexcept:
  TftpPacket( PacketType::Error),
  errorCode( errorCode)
{
}

BaseErrorPacket::BaseErrorPacket(
  const RawTftpPacketType &rawPacket):
  TftpPacket( PacketType::Error, rawPacket)
{
  // check size
  if (rawPacket.size() < 5)
  {
    //! @throw InvalidPacketException When rawPacket is not an valid packet.
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "Invalid packet size of ERROR packet"));
  }

  RawTftpPacketType::const_iterator packetIt = rawPacket.begin() + TFTP_PACKET_HEADER_SIZE;

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

  RawTftpPacketType::iterator packetIt = rawPacket.begin() + TFTP_PACKET_HEADER_SIZE;

  // error code
  packetIt = setInt( packetIt, static_cast< const uint16_t>( getErrorCode()));

  // error message
  packetIt = std::copy( errorMessage.begin(), errorMessage.end(), packetIt);
  *packetIt = 0;

  return rawPacket;
}

BaseErrorPacket::string BaseErrorPacket::toString() const
{
  return (boost::format( "ERR: EC: %s (%d) - DESC: \"%s\"") %
    getErrorCodeString( getErrorCode()) %
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
