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
 * @brief Definition of class Tftp::Packet::ErrorPacket.
 **/

#include "ErrorPacket.hpp"

namespace Tftp {
namespace Packet {

ErrorPacket::ErrorPacket(
	const ErrorCode errorCode,
	const string &errorMessage) noexcept:
	BaseErrorPacket( errorCode),
	errorMessage( errorMessage)
{
}

ErrorPacket::ErrorPacket( const RawTftpPacketType &rawPacket):
	BaseErrorPacket( rawPacket),
	errorMessage( BaseErrorPacket::getErrorMessage( rawPacket))
{
}

ErrorPacket::string ErrorPacket::getErrorMessage( ) const
{
	return errorMessage;
}

void ErrorPacket::setErrorMessage( const string &errorMessage)
{
	this->errorMessage = errorMessage;
}

}
}
