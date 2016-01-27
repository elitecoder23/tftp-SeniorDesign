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
 * @brief Declaration of class Tftp::Client::TftpClientWriteRequestOperationImpl.
 **/

#ifndef TFTP_CLIENT_TFTPCLIENTWRITEREQUESTOPERATIONIMPL_HPP
#define TFTP_CLIENT_TFTPCLIENTWRITEREQUESTOPERATIONIMPL_HPP

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
 * @brief Class which handles a TFTP Write Request on client side.
 *
 * After executed, the class sends the TFTP WRQ packet to the destination
 * and waits for answer. Data is handled by the TftpWriteOperationHandler
 * given at construction time.
 **/
class TftpClientWriteRequestOperationImpl : public TftpClientOperationImpl
{
	public:
		/**
		 * @brief Constructor of TftpClientWriteOperation
		 *
		 * @param[in] handler
		 *   Handler for data.
		 * @param[in] tftpClientInternal
		 *   The TFTP internal client.
		 * @param[in] serverAddress
		 *   Where the connection should be established to.
		 * @param[in] filename
		 *   Which file shall be requested
		 * @param[in] mode
		 *   The transfer mode
		 * @param[in] from
		 *   Optional parameter to define the communication source
		 **/
		TftpClientWriteRequestOperationImpl(
			TftpTransmitDataOperationHandler &handler,
			const TftpClientInternal &tftpClientInternal,
			const UdpAddressType &serverAddress,
			const string &filename,
			const TransferMode mode,
			const UdpAddressType &from);

		/**
		 * @brief Constructor of TftpClientWriteOperation
		 *
		 * @param[in] handler
		 *   Handler for data.
		 * @param[in] tftpClientInternal
		 *   The TFTP internal client.
		 * @param[in] serverAddress
		 *   Where the connection should be established to.
		 * @param[in] filename
		 *   Which file shall be requested
		 * @param[in] mode
		 *   The transfer mode
		 **/
		TftpClientWriteRequestOperationImpl(
			TftpTransmitDataOperationHandler &handler,
			const TftpClientInternal &tftpClientInternal,
			const UdpAddressType &serverAddress,
			const string &filename,
			const TransferMode mode);

		/**
		 * @copybrief TftpClientOperationImpl::operator()
		 *
		 * Assembles and transmit TFTP WRQ packet and start parent receive
		 * loop.
		 **/
		virtual void operator ()( void) override;

	protected:
		/**
		 * @brief Sends the data to the host.
		 *
		 * This operation requests the data from the handler, generates the
		 * TFTP DATA packet and sends them to the host.
		 **/
		void sendData( void);

		/**
		 * @copydoc TftpPacketHandler::handleDataPacket()
		 *
		 * @throw InvalidPacketException
		 *   Always, because an this packet is invalid.
		 **/
		virtual void handleDataPacket(
			const UdpAddressType &from,
			const DataPacket &dataPacket) override;

		/**
		 * @copydoc TftpPacketHandler::handleAcknowledgementPacket()
		 *
		 * @throw InvalidPacketException
		 *   Invalid block number
		 **/
		virtual void handleAcknowledgementPacket(
			const UdpAddressType &from,
			const AcknowledgementPacket &acknowledgementPacket) override;

		/**
		 * @copydoc TftpPacketHandler::handleOptionsAcknowledgementPacket()
		 *
		 * @throw InvalidPacketException
		 *   Empty option list
		 * @throw OptionNegotiationException
		 *   Option negotiation failed
		 **/
		virtual void handleOptionsAcknowledgementPacket(
			const UdpAddressType &from,
			const OptionsAcknowledgementPacket &optionsAcknowledgementPacket) override;

	private:
		//! The handler, which is called
		TftpTransmitDataOperationHandler &handler;
		//! Size of the data-section in the TFTP DATA packet - changed during option negotiation.
		uint16_t transmitDataSize;
		//! Is set, when the last data packet has been transmitted
		bool lastDataPacketTransmitted;
		//! The block number of the last transmitted data packet.
		BlockNumber lastTransmittedBlockNumber;
};

}
}

#endif
