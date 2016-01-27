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
 * @brief Declaration of class Tftp::Client::TftpClientReadRequestOperationImpl.
 **/

#ifndef TFTP_CLIENT_TFTPCLIENTREADREQUESTOPERATIONIMPL_HPP
#define TFTP_CLIENT_TFTPCLIENTREADREQUESTOPERATIONIMPL_HPP

#include <tftp/client/Client.hpp>
#include <tftp/client/implementation/TftpClientOperationImpl.hpp>
#include <tftp/packet/BlockNumber.hpp>

namespace Tftp {
namespace Client {

using Tftp::Packet::DataPacket;
using Tftp::Packet::AcknowledgementPacket;
using Tftp::Packet::OptionsAcknowledgementPacket;
using Tftp::Packet::BlockNumber;

/**
 * @brief Class which handles a TFTP Read Request on client side.
 *
 * After executed, the class sends the TFTP RRQ packet to the destination
 * and waits for answer. Received data is handled by the
 * TftpReadOperationHandler given at construction time.
 **/
class TftpClientReadRequestOperationImpl : public TftpClientOperationImpl
{
	public:
		/**
		 * @brief Constructor of TftpClientReadOperation
		 *
		 * @param[in] handler
		 *   Handler for received data.
		 * @param[in] tftpClientInternal
		 *   The TFTP internal client.
		 * @param[in] serverAddress
		 *   Where the connection should be established to.
		 * @param[in] filename
		 *   Which file shall be requested
		 * @param[in] mode
		 *   The transfer mode
		 * @param[in] from
		 *   communication source
		 **/
		TftpClientReadRequestOperationImpl(
			TftpReceiveDataOperationHandler &handler,
			const TftpClientInternal &tftpClientInternal,
			const UdpAddressType &serverAddress,
			const string &filename,
			const TransferMode mode,
			const UdpAddressType &from);

		/**
		 * @brief Constructor of TftpClientReadOperation
		 *
		 * @param[in] handler
		 *   Handler for received data.
		 * @param[in] tftpClientInternal
		 *   The TFTP internal client.
		 * @param[in] serverAddress
		 *   Where the connection should be established to.
		 * @param[in] filename
		 *   Which file shall be requested
		 * @param[in] mode
		 *   The transfer mode
		 **/
		TftpClientReadRequestOperationImpl(
			TftpReceiveDataOperationHandler &handler,
			const TftpClientInternal &tftpClientInternal,
			const UdpAddressType &serverAddress,
			const string &filename,
			const TransferMode mode);

		/**
		 * @copybrief TftpClientOperationImpl::operator()
		 *
		 * Assembles and transmit TFTP RRQ packet and start parent receive
		 * loop.
		 **/
		virtual void operator ()( void) override;

	protected:
		/**
		 * @copydoc TftpPacketHandler::handleDataPacket()
		 *
		 * The TFTP DATA packet is decoded and checked.
		 * If everything is fine, handler is called with extracted data and
		 * the receive operation is continued.
		 **/
		virtual void handleDataPacket(
			const UdpAddressType &from,
			const DataPacket &dataPacket) override;

		/**
		 * @copydoc TftpPacketHandler::handleAcknowledgementPacket()
		 *
		 * ACK packets are not expected for this operation.
		 * They are rejected by error transmission
		 **/
		virtual void handleAcknowledgementPacket(
			const UdpAddressType &from,
			const AcknowledgementPacket &acknowledgementPacket) override;

		/**
		 * @copydoc TftpPacketHandler::handleOptionsAcknowledgementPacket()
		 *
		 **/
		virtual void handleOptionsAcknowledgementPacket(
			const UdpAddressType &from,
			const OptionsAcknowledgementPacket &optionsAcknowledgementPacket) override;

	private:
		//! Registered handler.
		TftpReceiveDataOperationHandler &handler;
		//! Size of the data-section in the TFTP DATA packet - changed during option negotiation.
		uint16_t receiveDataSize;
		//! last received block number.
		BlockNumber lastReceivedBlockNumber;
};

}
}

#endif
