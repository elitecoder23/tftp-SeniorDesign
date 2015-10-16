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
 * @brief Definition of class TftpClientWriteRequestOperationImpl.
 **/

#include "TftpClientWriteRequestOperationImpl.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/TftpTransmitDataOperationHandler.hpp>
#include <tftp/packet/PacketFactory.hpp>

#include <helper/Dump.hpp>
#include <helper/Logger.hpp>

using namespace Tftp::Client;
using Tftp::Options::OptionList;

TftpClientWriteRequestOperationImpl::TftpClientWriteRequestOperationImpl(
	TftpTransmitDataOperationHandler &handler,
	const TftpClientInternal &tftpClientInternal,
	const UdpAddressType &serverAddress,
	const string &filename,
	const TransferMode mode,
	const UdpAddressType &from):
	TftpClientOperationImpl( tftpClientInternal, serverAddress, filename, mode, from),
	handler( handler),
	transmitDataSize( DEFAULT_DATA_SIZE),
	lastDataPacketTransmitted( false),
	lastTransmittedBlockNumber( 0)
{
}

TftpClientWriteRequestOperationImpl::TftpClientWriteRequestOperationImpl(
	TftpTransmitDataOperationHandler &handler,
	const TftpClientInternal &tftpClientInternal,
	const UdpAddressType &serverAddress,
	const string &filename,
	const TransferMode mode):
	TftpClientOperationImpl( tftpClientInternal, serverAddress, filename, mode),
	handler( handler),
	transmitDataSize( DEFAULT_DATA_SIZE),
	lastDataPacketTransmitted( false),
	lastTransmittedBlockNumber( 0)
{
}

void TftpClientWriteRequestOperationImpl::operator ()( void)
{
	try
	{
		transmitDataSize = DEFAULT_DATA_SIZE;
		lastDataPacketTransmitted = false;
		lastTransmittedBlockNumber = 0;

		//! if Transfer Size option is set, query them from the handler
		if (getOptions().hasTransferSizeOption())
		{
			uint64_t transferSize;

			//! If the handler supplies a transfer size
			if (handler.requestedTransferSize( transferSize))
			{
				//! set transfer size TFTP option
				getOptions().addTransferSizeOption( transferSize);
			}
			else
			{
				//! otherwise remove this option
				getOptions().removeTransferSizeOption();
			}
		}

		//! send write request packet
		sendFirst( WriteRequestPacket(
			getFilename(),
			getMode(),
			getOptions()));

		//! wait for answers
		TftpClientOperationImpl::operator ()();
	}
	catch (...)
	{
		handler.finishedOperation();

		throw;
	}

	handler.finishedOperation();
}

void TftpClientWriteRequestOperationImpl::sendData( void)
{
	lastTransmittedBlockNumber++;

	DataPacket data(
		lastTransmittedBlockNumber,
		handler.sendData( transmitDataSize));

	if (data.getDataSize() < transmitDataSize)
	{
		lastDataPacketTransmitted = true;
	}

	//! send packet
	send( data);
}

void TftpClientWriteRequestOperationImpl::handleDataPacket(
	const UdpAddressType &,
	const DataPacket &dataPacket)
{
	BOOST_LOG_TRIVIAL( info) <<
		"RX ERROR: " << dataPacket.toString();

	send( ErrorPacket(
		ErrorCode::ILLEGAL_TFTP_OPERATION,
		"DATA not expected"));

	//! Operation completed
	finished();

	//! @throw CommunicationException Always, because this packet is invalid.
	BOOST_THROW_EXCEPTION( CommunicationException() <<
		AdditionalInfo( "DATA not expected"));
}

void TftpClientWriteRequestOperationImpl::handleAcknowledgementPacket(
	const UdpAddressType &,
	const AcknowledgementPacket &acknowledgementPacket)
{
	BOOST_LOG_TRIVIAL( info) << "RX: " << acknowledgementPacket.toString();

	//! check retransmission
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
			"Wrong block number"));

		finished();

		//! @throw CommunicationException When invalid block number has been received.
		BOOST_THROW_EXCEPTION( CommunicationException() <<
			AdditionalInfo( "Invalid block number received"));
	}

	//! if ACK for last data packet - QUIT
	if (lastDataPacketTransmitted)
	{
		finished();

		return;
	}

	//! send data
	sendData();

	//! wait for next packet
	receive();
}

void TftpClientWriteRequestOperationImpl::handleOptionsAcknowledgementPacket(
	const UdpAddressType &,
	const OptionsAcknowledgementPacket &optionsAcknowledgementPacket)
{
	BOOST_LOG_TRIVIAL( info) << "RX: " << optionsAcknowledgementPacket.toString();

	OptionList options = optionsAcknowledgementPacket.getOptions();

	//! check empty options
	if (options.getOptions().empty())
	{
		BOOST_LOG_TRIVIAL( error) << "Received option list is empty";

		send( ErrorPacket(
			ErrorCode::ILLEGAL_TFTP_OPERATION,
			"Empty OACK not allowed"));

		//! @throw CommunicationException When Option list is empty.
		BOOST_THROW_EXCEPTION( CommunicationException() <<
			AdditionalInfo( "Received option list is empty"));
	}

	//! perform option negotiation
	OptionList negotiatedOptions = getOptions().negotiateClient( options);
	if (negotiatedOptions.getOptions().empty())
	{
		BOOST_LOG_TRIVIAL( error) << "Option negotiation failed";

		send( ErrorPacket(
			ErrorCode::TFTP_OPTION_REFUSED,
			"Option negotiation failed"));

		//! @throw OptionNegotiationException When option negotiation failed.
		BOOST_THROW_EXCEPTION( OptionNegotiationException() <<
			AdditionalInfo( "Option negotiation failed"));
	}

	//! check blocksize option
	if (0 != negotiatedOptions.getBlocksizeOption())
	{
		transmitDataSize = negotiatedOptions.getBlocksizeOption();
	}

	//! check timeout option
	if (0 != negotiatedOptions.getTimeoutOption())
	{
		setReceiveTimeout( negotiatedOptions.getTimeoutOption());
	}

	//! Transfer size option is not checked here (already performed during option negotiation)

	//! send data
	sendData();
	
	//! wait for next packet
	receive();

}
