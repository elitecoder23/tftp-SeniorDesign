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
 * @brief Declaration of class TftpServerOperationImpl.
 **/

#ifndef TFTP_SERVER_TFTPSERVEROPERATIONIMPL_HPP
#define TFTP_SERVER_TFTPSERVEROPERATIONIMPL_HPP

#include <tftp/server/Server.hpp>
#include <tftp/TftpPacketHandler.hpp>
#include <tftp/packet/Packet.hpp>
#include <tftp/options/OptionList.hpp>

#include <boost/asio.hpp>

#include <string>

namespace Tftp
{
	namespace Server
	{
		using Tftp::Packet::TftpPacket;
		using Tftp::Packet::ReadRequestPacket;
		using Tftp::Packet::WriteRequestPacket;
		using Tftp::Packet::ErrorPacket;
		using Tftp::Packet::OptionsAcknowledgementPacket;

		using Tftp::Options::OptionList;

		class TftpServerInternal;

		/**
		 * @brief Base class for TFTP server operations.
		 *
		 * This class is specialised for the two kinds of TFTP operations
		 * (Read Operation, Write Operation).
		 **/
		class TftpServerOperationImpl: protected TftpPacketHandler
		{
			protected:
				/**
				 * @brief Initialises the TFTP server operation.
				 *
				 * @param[in] tftpServerInternal
				 *   The TFTP internal server.
				 * @param[in] clientAddress
				 *   Address of the remote endpoint (TFTP client).
				 * @param[in] clientOptions
				 *   Received option list from client.
				 * @param[in] serverAddress
				 *   local endpoint, where the server handles the request from.
				 **/
				TftpServerOperationImpl(
					const TftpServerInternal &tftpServerInternal,
					const UdpAddressType &clientAddress,
					const OptionList &clientOptions,
					const UdpAddressType &serverAddress);

				/**
				 * @brief Constructor of operation
				 *
				 * @param[in] tftpServerInternal
				 *   The TFTP internal server.
				 * @param[in] clientAddress
				 *   Address of the remote endpoint (TFTP client).
				 * @param[in] clientOptions
				 *   Received option list from client.
				 **/
				TftpServerOperationImpl(
					const TftpServerInternal &tftpServerInternal,
					const UdpAddressType &clientAddress,
					const OptionList &clientOptions);

				/**
				 * @brief default destructor.
				 **/
				virtual ~TftpServerOperationImpl( void) noexcept;

				virtual void operator ()( void);

				/**
				 * @brief Sets the Finished flag.
				 *
				 * This operation is called, when the last packet has been received or
				 * transmitted to stop the receive loop.
				 **/
				void finished( void) noexcept;

				/**
				 * @brief Sends the given packet to the client.
				 *
				 * @param[in] packet
				 *   The packet, which is sent to the client.
				 *
				 * @throw CommunicationException
				 **/
				void send( const TftpPacket &packet);

				/**
				 * @brief receives a packet and calls the packet handlers
				 **/
				void receive( void);

				/**
				 * @brief Returns the stored TFTP option list.
				 *
				 * @return The stored TFTP option list.
				 **/
				OptionList& getOptions( void);

				/**
				 * @brief Updates the limit of maximum packet size for the receive
				 *   operation.
				 *
				 * This operation must be used to vary the receive buffer, when the
				 * option negotiation results in a modified packet size.
				 *
				 * The set maximum packet size is not forced when a packet is sent to
				 * the client.
				 *
				 * @param[in] maxReceivePacketSize
				 *   The new maximum packet size for receive operation.
				 **/
				void setMaxReceivePacketSize( const uint16_t maxReceivePacketSize);

				/**
				 * @brief Update the receiveTimeout value.
				 *
				 * @param[in] receiveTimeout
				 *   The new receive timeout.
				 **/
				void setReceiveTimeout( const uint8_t receiveTimeout);

				/**
				 * @copydoc TftpPacketHandler::handleReadRequestPacket
				 *
				 * A RRQ packet is not expected - therefore send an error packet an
				 * terminate connection.
				 **/
				virtual void handleReadRequestPacket(
					const UdpAddressType &from,
					const ReadRequestPacket &readRequestPacket) override;

				/**
				 * @copydoc TftpPacketHandler::handleWriteRequestPacket
				 *
				 * A WRQ packet is not expected - therefore send an error packet an
				 * terminate connection.
				 **/
				virtual void handleWriteRequestPacket(
					const UdpAddressType &from,
					const WriteRequestPacket &writeRequestPacket) override;

				/**
				 * @copydoc TftpPacketHandler::handleErrorPacket()
				 *
				 * Terminate connection.
				 **/
				virtual void handleErrorPacket(
					const UdpAddressType &from,
					const ErrorPacket &errorPacket) override;

				/**
				 * @copydoc TftpPacketHandler::handleOptionsAcknowledgementPacket()
				 *
				 * A OACK packet is not expected - therefore send an error packet an
				 * terminate connection.
				 **/
				virtual void handleOptionsAcknowledgementPacket(
					const UdpAddressType &from,
					const OptionsAcknowledgementPacket &optionsAcknowledgementPacket) override;

				/**
				 * @copydoc TftpPacketHandler::handleInvalidPacket()
				 *
				 * Send error packet and terminate connection.
				 **/
				virtual void handleInvalidPacket(
					const UdpAddressType &from,
					const RawTftpPacketType &rawPacket) override;

			private:
				/**
				 * @brief Handler, which is called when new data has been received.
				 *
				 * @param[in] errorCode
				 *   Receive error code
				 * @param bytesTransferred
				 *   Number of transferred bytes.
				 **/
				void receiveHandler(
					const boost::system::error_code& errorCode,
					std::size_t bytesTransferred);

				/**
				 * @brief Handler for receive timeouts
				 *
				 * @param[in] errorCode
				 *   When handler is called with error, errorCode is set
				 **/
				void timeoutHandler(
					const boost::system::error_code& errorCode);

				//! The internal TFTP server
				const TftpServerInternal &tftpServerInternal;

				//! The remote (client) endpoint
				const UdpAddressType clientAddress;
				//! The stored negotiated options
				OptionList options;
				//! The maximum packet size, which can be received
				uint16_t maxReceivePacketSize;
				//! The receive timeout - is initialised to TFTP_DEFAULT_TIMEOUT
				uint8_t receiveTimeout;

				//! The io service, which performs the asynchronous operations.
				boost::asio::io_service ioService;
				//! The TFTP UDP socket
				boost::asio::ip::udp::socket socket;
				//! The timeout timer
				boost::asio::deadline_timer timer;

				//! stores the received packets
				RawTftpPacketType receivePacket;
				//! the transmitted packet is stored for retries
				RawTftpPacketType transmitPacket;
				//! Packet Type of last transmitted packet
				PacketType transmitPacketType;
				//! counter to store how often the same packet has been transmitted (retries)
				unsigned int transmitCounter;
		};
	}
}

#endif
