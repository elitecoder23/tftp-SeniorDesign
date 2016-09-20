/*
 * $Date$
 * $Revision$
 */
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::Packet::AcknowledgementPacket.
 **/

#ifndef TFTP_PACKET_ACKNOWLEDGEMENTPACKET_HPP
#define TFTP_PACKET_ACKNOWLEDGEMENTPACKET_HPP

#include <tftp/packet/Packet.hpp>
#include <tftp/packet/TftpPacket.hpp>
#include <tftp/packet/BlockNumber.hpp>

namespace Tftp {
namespace Packet {

/**
 * @brief TFTP Acknowledgement package (ACK).
 *
 * The TFTP Acknowledgement packet acknowledges a received WRQ packet without
 * options negotiation or an data packet.
 *
 * | ACK | Block # |
 * |:---:|:-------:|
 * | 2 B |   2 B   |
 **/
class AcknowledgementPacket: public TftpPacket
{
	public:
		/**
		 * @brief Creates the packet with the given block number.
		 *
		 * If the blockNumber parameter is suppressed, a default block number is
		 * chosen.
		 *
		 * @param[in] blockNumber
		 *   Block number of packet. Defaults to the BlockNumber default
		 *   constructor.
		 **/
		AcknowledgementPacket(
		  const BlockNumber blockNumber = BlockNumber()) noexcept;

		/**
		 * @brief Generates a TFTP acknowledgement packet from a data buffer.
		 *
		 * @param[in] rawPacket
		 *   Packet, which shall be decoded.
		 *
		 * @throw InvalidPacketException
		 *   When rawPacket is not an valid packet.
		 **/
		AcknowledgementPacket( const RawTftpPacketType &rawPacket);

		/**
		 * @brief Returns the block number.
		 *
		 * @return The block number.
		 **/
		BlockNumber getBlockNumber( void) const;

		/**
		 * @brief Sets the block number of the packet.
		 *
		 * @param[in] blockBumber
		 *   Block number of packet.
		 **/
		void setBlockNumber( const BlockNumber blockBumber);

		//! @copydoc TftpPacket::encode()
		virtual RawTftpPacketType encode( void) const override;

		//! @copydoc TftpPacket::toString()
		virtual string toString( void) const override;

	private:
		//! Block number of the packet
		BlockNumber blockNumber;
};

}
}

#endif
