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
 * @brief Definition of class TftpServerReadRequestOperationImpl.
 **/

#include "TftpServerReadRequestOperationImpl.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/TftpTransmitDataOperationHandler.hpp>
#include <tftp/packet/AcknowledgementPacket.hpp>
#include <tftp/packet/OptionsAcknowledgementPacket.hpp>
#include <tftp/packet/DataPacket.hpp>
#include <tftp/packet/ErrorPacket.hpp>

#include <helper/Logger.hpp>

using namespace Tftp::Server;

using Tftp::Packet::AcknowledgementPacket;
using Tftp::Packet::DataPacket;
using Tftp::Packet::ErrorPacket;
using Tftp::Packet::OptionsAcknowledgementPacket;

TftpServerReadRequestOperationImpl::TftpServerReadRequestOperationImpl(
	TftpTransmitDataOperationHandler &handler,
	const TftpServerInternal &tftpServerInternal,
	const UdpAddressType &clientAddress,
	const OptionList &clientOptions,
	const UdpAddressType &serverAddress):
	TftpServerOperationImpl( tftpServerInternal, clientAddress, clientOptions, serverAddress),
	handler( handler),
	transmitDataSize( DEFAULT_DATA_SIZE),
	lastDataPacketTransmitted( false),
	lastTransmittedBlockNumber( 0)
{
}

TftpServerReadRequestOperationImpl::TftpServerReadRequestOperationImpl(
	TftpTransmitDataOperationHandler &handler,
	const TftpServerInternal &tftpServerInternal,
	const UdpAddressType &clientAddress,
	const OptionList &clientOptions):
	TftpServerOperationImpl( tftpServerInternal, clientAddress, clientOptions),
	handler( handler),
	transmitDataSize( DEFAULT_DATA_SIZE),
	lastDataPacketTransmitted( false),
	lastTransmittedBlockNumber( 0)
{
}

TftpServerReadRequestOperationImpl::~TftpServerReadRequestOperationImpl( void) noexcept
{
}

void TftpServerReadRequestOperationImpl::operator ()( void)
{
	try
	{
		//! option negotiation leads to empty option list
		if (getOptions().getOptions().empty())
		{
			sendData();
		}
		else
		{
			//! check blocksize option
			if (0 != getOptions().getBlocksizeOption())
			{
				transmitDataSize = getOptions().getBlocksizeOption();
			}

			//! check timeout option
			if (0 != getOptions().getTimeoutOption())
			{
				setReceiveTimeout( getOptions().getTimeoutOption());
			}

			//! check transfer size option
			if (getOptions().hasTransferSizeOption())
			{
				uint64_t transferSize;

				if (handler.requestedTransferSize( transferSize))
				{
					getOptions().addTransferSizeOption( transferSize);
				}
				else
				{
					getOptions().removeTransferSizeOption();
				}
			}

			//! Send OACK
			send( OptionsAcknowledgementPacket( getOptions()));
		}

		//! start receive loop
		TftpServerOperationImpl::operator ()();
	}
	catch (...)
	{
		handler.finishedOperation();

		throw;
	}

	handler.finishedOperation();
}

void TftpServerReadRequestOperationImpl::sendData( void)
{
	lastTransmittedBlockNumber++;

	DataPacket data(
		lastTransmittedBlockNumber,
		handler.sendData( transmitDataSize));

	if (data.getDataSize() < transmitDataSize)
	{
		lastDataPacketTransmitted = true;
	}

	//! send data
	send( data);
}

void TftpServerReadRequestOperationImpl::handleDataPacket(
	const UdpAddressType &,
	const DataPacket &dataPacket)
{
	BOOST_LOG_TRIVIAL( error) << "RX ERROR: " << dataPacket.toString();

	send( ErrorPacket(
		ErrorCode::ILLEGAL_TFTP_OPERATION,
		"DATA not expected"));

	//! Operation completed
	finished();

	//! @throw CommunicationException Always, because this packet is invalid.
	BOOST_THROW_EXCEPTION( CommunicationException() <<
		AdditionalInfo( "DATA not expected"));
}

void TftpServerReadRequestOperationImpl::handleAcknowledgementPacket(
	const UdpAddressType &,
	const AcknowledgementPacket &acknowledgementPacket)
{
	BOOST_LOG_TRIVIAL( info) << "RX: " << acknowledgementPacket.toString();

	// check retransmission
	if (acknowledgementPacket.getBlockNumber() == lastTransmittedBlockNumber.previous())
	{
		BOOST_LOG_TRIVIAL( info) <<
			"Received previous ACK packet: retry of last data package - "
			"IGNORE it due to Sorcerer's Apprentice Syndrome";

		return;
	}

	//! check invalid block number
	if (acknowledgementPacket.getBlockNumber() != lastTransmittedBlockNumber)
	{
		BOOST_LOG_TRIVIAL( error) << "Invalid block number received";

		send( ErrorPacket(
			ErrorCode::ILLEGAL_TFTP_OPERATION,
			"Block number not expected"));

		//! Operation completed
		finished();

		//! @throw CommunicationException On invalid block number
		BOOST_THROW_EXCEPTION( CommunicationException() <<
			AdditionalInfo( "Block number not expected"));

	}

	//! if it was the last ACK of the last data packet - we are finished.
	if (lastDataPacketTransmitted)
	{
		finished();

		return;
	}

	//! send data
	sendData();

	//! receive next packet
	receive();
}
