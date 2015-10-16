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
 * @brief Definition of class TftpServerBaseErrorOperation.
 **/

#include "TftpServerBaseErrorOperation.hpp"
#include <tftp/packet/BaseErrorPacket.hpp>
#include <tftp/TftpException.hpp>

#include <helper/Logger.hpp>

using namespace Tftp::Server;

TftpServerBaseErrorOperation::TftpServerBaseErrorOperation(
	const AddressType &clientAddress)
try:
clientAddress( clientAddress),
	socket( ioService)
{
	try
	{
		socket.open( clientAddress.protocol());

		socket.connect( clientAddress);
	}
	catch (boost::system::system_error &err)
	{
		if (socket.is_open())
		{
			socket.close();
		}

		BOOST_THROW_EXCEPTION( CommunicationException() <<
			AdditionalInfo( err.what()));
	}
}
catch (boost::system::system_error &err)
{
	BOOST_THROW_EXCEPTION( CommunicationException() <<
		AdditionalInfo( err.what()));
}

TftpServerBaseErrorOperation::TftpServerBaseErrorOperation(
	const AddressType &clientAddress,
	const AddressType &from)
try:
clientAddress( clientAddress),
	socket( ioService)
{
	try
	{
		socket.open( clientAddress.protocol());

		socket.bind( from);

		socket.connect( clientAddress);
	}
	catch (boost::system::system_error &err)
	{
		if (socket.is_open())
		{
			socket.close();
		}

		BOOST_THROW_EXCEPTION( TftpException() <<
			AdditionalInfo( err.what()));
	}
}
catch (boost::system::system_error &err)
{
	BOOST_THROW_EXCEPTION( CommunicationException() <<
		AdditionalInfo( err.what()));
}

TftpServerBaseErrorOperation::~TftpServerBaseErrorOperation( void) noexcept
{
	try
	{
		socket.close();
	}
	catch (boost::system::system_error &err)
	{
		BOOST_LOG_TRIVIAL( error) << err.what();
	}
}

void TftpServerBaseErrorOperation::sendError( const BaseErrorPacket &error)
{
	try
	{
		socket.send( boost::asio::buffer( error.encode()));
	}
	catch (boost::system::system_error &err)
	{
		BOOST_THROW_EXCEPTION( CommunicationException() <<
			AdditionalInfo( err.what()));
	}
}
