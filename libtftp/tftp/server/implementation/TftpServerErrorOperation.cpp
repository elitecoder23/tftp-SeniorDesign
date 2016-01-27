/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * $Date$
 * $Revision$
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class Tftp::Server::TftpServerErrorOperation.
 **/

#include "TftpServerErrorOperation.hpp"

#include <tftp/packet/ErrorPacket.hpp>

#include <helper/Logger.hpp>

namespace Tftp {
namespace Server {

using Tftp::Packet::ErrorPacket;

TftpServerErrorOperation::TftpServerErrorOperation(
	const AddressType &clientAddress,
	const AddressType &from,
	const ErrorCode errorCode,
	const string &errorMessage):
	TftpServerBaseErrorOperation( clientAddress, from),
	errorCode( errorCode),
	errorMessage( errorMessage)
{
}

TftpServerErrorOperation::TftpServerErrorOperation(
	const AddressType &clientAddress,
	const ErrorCode errorCode,
	const string &errorMessage):
	TftpServerBaseErrorOperation( clientAddress),
	errorCode( errorCode),
	errorMessage( errorMessage)
{
}

void TftpServerErrorOperation::operator ()( void)
{
	sendError( ErrorPacket( errorCode, errorMessage));
}

}
}
