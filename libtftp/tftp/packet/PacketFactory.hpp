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
 * @brief Declaration of class Tftp::Packet::PacketFactory.
 **/

#ifndef TFTP_PACKET_PACKETFACTORY_HPP
#define TFTP_PACKET_PACKETFACTORY_HPP

#include <tftp/packet/Packet.hpp>

#include <tftp/packet/ReadRequestPacket.hpp>
#include <tftp/packet/WriteRequestPacket.hpp>
#include <tftp/packet/DataPacket.hpp>
#include <tftp/packet/ErrorPacket.hpp>
#include <tftp/packet/AcknowledgementPacket.hpp>
#include <tftp/packet/OptionsAcknowledgementPacket.hpp>

namespace Tftp {
namespace Packet {

/**
 * @brief Helper Class to get the type of a received TFTP packet and get the
 *   approbate instance.
 *
 * This class is used internally to decode the TFTP packet type and
 * generate the approbate class.
 **/
class PacketFactory
{
	public:
		/**
		 * @brief Determines the packet type for the given raw data packet.
		 *
		 * @param[in] rawPacket
		 *   The raw data packet.
		 *
		 * @return The TFTP packet type of the data packet or INVALID.
		 * @retval INVALID
		 *   If the packet cannot be decoded.
		 **/
		static PacketType getPacketType( const RawTftpPacketType &rawPacket)
		  noexcept;

		/**
		 * @brief Decodes the given raw data as TFTP Read Request Packet.
		 *
		 * @param[in] rawPacket
		 *   The raw packet data.
		 *
		 * @return The corresponding packet class.
		 *
		 * @throw InvalidPacketException
		 *   When packet cannot be decoded as Read Request Packet (RRQ).
		 **/
		static ReadRequestPacket getReadRequestPacket(
		  const RawTftpPacketType &rawPacket);

		/**
		 * @brief Decodes the given raw data as TFTP Write Request Packet.
		 *
		 * @param[in] rawPacket
		 *   The raw packet data
		 *
		 * @return The corresponding packet class.
		 *
		 * @throw InvalidPacketException
		 *    When packet cannot be decoded as Write Request Packet.
		 **/
		static WriteRequestPacket getWriteRequestPacket(
		  const RawTftpPacketType &rawPacket);

		/**
		 * @brief Decodes the given raw data as TFTP Data Packet.
		 *
		 * @param[in] rawPacket
		 *   The raw packet data
		 *
		 * @return The corresponding packet class.
		 *
		 * @throw InvalidPacketException
		 *  When packet cannot be decoded as Data Packet.
		 **/
		static DataPacket getDataPacket( const RawTftpPacketType &rawPacket);

		/**
		 * @brief Decodes the given raw data as TFTP Error Packet.
		 *
		 * @param[in] rawPacket
		 *   The raw packet data.
		 *
		 * @return The corresponding packet class.
		 *
		 * @throw InvalidPacketException
		 *  When packet cannot be decoded as Error Packet.
		 **/
		static ErrorPacket getErrorPacket( const RawTftpPacketType &rawPacket);

		/**
		 * @brief Decodes the given raw data as TFTP Acknowledgement Packet.
		 *
		 * @param[in] rawPacket
		 *   The raw packet data.
		 *
		 * @return The corresponding packet class.
		 *
		 * @throw InvalidPacketException
		 *    When packet cannot be decoded as Acknowledgement Packet.
		 **/
		static AcknowledgementPacket getAcknowledgementPacket(
		  const RawTftpPacketType &rawPacket);

		/**
		 * @brief Decodes the given raw data as TFTP Options Acknowledgement
		 *   Packet.
		 *
		 * @param[in] rawPacket
		 *   The raw packet data.
		 *
		 * @return The corresponding packet class.
		 *
		 * @throw InvalidPacketException
		 *   When packet cannot be decoded as Options Acknowledgement Packet.
		 **/
		static OptionsAcknowledgementPacket getOptionsAcknowledgementPacket(
		  const RawTftpPacketType &rawPacket);

	private:
		//! Private Constructor, which is not implemented.
		PacketFactory( void) = delete;
};

}
}

#endif
