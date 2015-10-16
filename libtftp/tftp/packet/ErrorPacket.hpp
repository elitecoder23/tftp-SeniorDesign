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
 * @brief Declaration of class ErrorPacket.
 **/

#ifndef TFTP_PACKET_ERRORPACKET_HPP
#define TFTP_PACKET_ERRORPACKET_HPP

#include <tftp/packet/Packet.hpp>
#include <tftp/packet/BaseErrorPacket.hpp>

#include <string>

namespace Tftp
{
	namespace Packet
	{
		/**
		 * @brief TFTP Error packet (ERR).
		 *
		 * A TFTP error code consists of an error code and a user readable error
		 * message.
		 **/
		class ErrorPacket: public BaseErrorPacket
		{
			public:
				/**
				 * @brief Generates a TFTP error packet with the given error code and
				 *   error message.
				 *
				 * @param[in] errorCode
				 *   Error code, which shall be set.
				 * @param[in] errorMessage
				 *   Error message, which shall be set. By default empty.
				 **/
				ErrorPacket(
					const ErrorCode errorCode,
					const string &errorMessage = string()) noexcept;

				/**
				 * @brief Generates a TFTP error packet from a data buffer
				 *
				 * @param[in] rawPacket
				 *   Packet, which shall be decoded.
				 *
				 * @throw InvalidPacketException
				 *   When rawPacket is not an valid packet.
				 **/
				ErrorPacket( const RawTftpPacketType &rawPacket);

				/**
				 * @brief Returns the error message of this packet.
				 *
				 * @return The error message
				 **/
				virtual string getErrorMessage( void) const override;

				/**
				 * @brief Sets the error message of this packet.
				 *
				 * @param[in] errorMessage
				 *   The error message to set. By default empty.
				 **/
				void setErrorMessage( const string &errorMessage = string());

			private:
				//! The error message.
				string errorMessage;
		};
	}
}

#endif
