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
 * @brief Declaration/ definition of class TftpException.
 **/

#ifndef TFTP_TFTPEXCEPTION_HPP
#define TFTP_TFTPEXCEPTION_HPP

#include <tftp/Tftp.hpp>
#include <tftp/packet/Packet.hpp>
#include <tftp/packet/ErrorPacket.hpp>

#include <helper/Exception.hpp>

#include <string>

namespace Tftp
{
	using std::string;

	/**
	 * @brief Base class for TFTP exceptions
	 **/
	class TftpException : public virtual Exception
	{
		public:
			virtual char const * what( void) const noexcept override
			{
				return "TFTP exception";
			}
	};

	//! Exception occurred during TFTP packet processing.
	class InvalidPacketException : public virtual TftpException
	{
		public:
			virtual char const * what( void) const noexcept override
			{
				return "TFTP Invalid Packet exception";
			}
	};

	//! Exception occurred during TFTP communication.
	class CommunicationException : public virtual TftpException
	{
		public:
			virtual char const * what( void) const noexcept override
			{
				return "TFTP communication exception";
			}
	};

	//! Exception occurred during TFTP option negotiation.
	class OptionNegotiationException : public virtual TftpException
	{
		public:
			virtual char const * what( void) const noexcept override
			{
				return "TFTP Option Negotiation exception";
			}
	};

	//! Exception thrown, when a TFTP ERR packet has been received
	class ErrorReceivedException : public virtual TftpException
	{
		public:
			/**
			 * @brief Creates the Error Received Exception with the necessary
			 *   information.
			 *
			 * @param[in] basePacketType
			 *   The TFTP packet sent, previous to the reception of the error.
			 * @param[in] errorPacket
			 *   The received TFTP Error packet
			 **/
			ErrorReceivedException(
				const PacketType basePacketType,
				const Packet::ErrorPacket &errorPacket):
					basePacketType( basePacketType),
					errorPacket( errorPacket)
			{
			}

			/**
			 * @brief Returns the TFTP packet type of the sent packet.
			 *
			 * @return  The TFTP packet sent, previous to the reception of the error.
			 **/
			PacketType getBasePacketType( void) const noexcept
			{
				return basePacketType;
			}

			/**
			 * @brief Returns the received TFTP Error packet.
			 *
			 * @return The received TFTP Error packet.
			 **/
			const Packet::ErrorPacket& getErrorPacket( void) const noexcept
			{
				return errorPacket;
			}

			virtual char const * what( void) const noexcept override
			{
				return "TFTP Error received exception";
			}

		private:
			//! Stored base packet type.
			const PacketType basePacketType;
			//! Stored error packet.
			const Packet::ErrorPacket errorPacket;
	};

	//! Definition of the TFTP transfer phase.
	typedef boost::error_info< struct TftpTransferPhaseInfoTag, TftpTransferPhase> TftpTransferPhaseInfo;
}

#endif
