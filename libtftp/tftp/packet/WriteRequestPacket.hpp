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
 * @brief Declaration of class WriteRequestPacket.
 **/

#ifndef TFTP_PACKET_WRITEREQUESTPACKET_HPP
#define TFTP_PACKET_WRITEREQUESTPACKET_HPP

#include <tftp/packet/Packet.hpp>
#include <tftp/packet/ReadWriteRequestPacket.hpp>

namespace Tftp
{
	namespace Packet
	{
		/**
		 * @brief TFTP Write Request packet (WRQ).
		 **/
		class WriteRequestPacket: public ReadWriteRequestPacket
		{
			public:
				/**
				 * @brief Creates a write request packet with the given data.
				 *
				 * @param[in] filename
				 *   The filename, which will be requested
				 * @param[in] mode
				 *   The transfer mode
				 * @param[in] options
				 *   The options, which are set
				 **/
				WriteRequestPacket(
					const string &filename,
					const TransferMode mode,
					const OptionList &options) noexcept;

				/**
				 * @brief Generates a TFTP Write Request packet from a data buffer
				 *
				 * @param[in] rawPacket
				 *   rawPacket, which shall be decoded.
				 *
				 * @throw InvalidPacketException
				 *   When the raw data does not represent a valid write request packet
				 **/
				WriteRequestPacket( const RawTftpPacketType &rawPacket);
		};
	}
}

#endif
