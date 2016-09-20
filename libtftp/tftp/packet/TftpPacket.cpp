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
 * @brief Definition of class TftpPacket.
 **/

#include "TftpPacket.hpp"

#include <tftp/TftpException.hpp>

#include <helper/Endianess.hpp>
#include <helper/Logger.hpp>

namespace Tftp {
namespace Packet {

PacketType TftpPacket::getPacketType(
	const RawTftpPacketType &rawPacket) noexcept
{
	// check minimum data size.
	if (rawPacket.size() < TFTP_PACKET_HEADER_SIZE)
	{
		BOOST_LOG_TRIVIAL( error) << "Packet to small";
		return PacketType::INVALID;
	}

	RawTftpPacketType::const_iterator it = rawPacket.begin();

	// decode opcode value
	uint16_t opcode;
	getInt< uint16_t>( it, opcode);

	// check valid opcodes
	switch ( static_cast< PacketType>( opcode))
	{
		case PacketType::READ_REQUEST:
		case PacketType::WRITE_REQUEST:
		case PacketType::DATA:
		case PacketType::ACKNOWLEDGEMENT:
		case PacketType::ERROR:
		case PacketType::OPTIONS_ACKNOWLEDGEMENT:
			// this are valid opcodes.
			break;

		default:
			// return INVALID for invalid values
			BOOST_LOG_TRIVIAL( error) << "Invalid opcode " << std::hex << opcode;
			return PacketType::INVALID;
	}

	return static_cast< PacketType>( opcode);
}

PacketType TftpPacket::getPacketType( ) const
{
	return packetType;
}

TftpPacket::string TftpPacket::toString( ) const
{
	switch (packetType)
	{
		case PacketType::READ_REQUEST:
			return "RRQ";

		case PacketType::WRITE_REQUEST:
			return "WRQ";

		case PacketType::DATA:
			return "DATA";

		case PacketType::ACKNOWLEDGEMENT:
			return "ACK";

		case PacketType::ERROR:
			return "ERR";

		case PacketType::OPTIONS_ACKNOWLEDGEMENT:
			return "OACK";

		default:
			return "INV";
	}
}

TftpPacket::TftpPacket( PacketType packetType) noexcept:
	packetType( packetType)
{
}

TftpPacket::TftpPacket(
	PacketType expectedPacketType,
	const RawTftpPacketType &rawPacket):
	packetType( expectedPacketType)
{
	// check size
	if (rawPacket.size() < TFTP_PACKET_HEADER_SIZE)
	{
		BOOST_THROW_EXCEPTION( InvalidPacketException() <<
			AdditionalInfo( "Invalid packet size (<TFTP_PACKET_HEADER_SIZE)"));
	}

	RawTftpPacketType::const_iterator packetIt = rawPacket.begin();

	// Check Opcode
	uint16_t opcode;
	getInt< uint16_t>( packetIt, opcode);

	if ( static_cast< PacketType>( opcode) != expectedPacketType)
	{
		BOOST_THROW_EXCEPTION( InvalidPacketException() <<
			AdditionalInfo( "Invalid opcode"));
	}
}

void TftpPacket::setPacketType( PacketType packetType)
{
	this->packetType = packetType;
}

void TftpPacket::insertHeader( RawTftpPacketType &rawPacket) const
{
	assert( rawPacket.size() >= TFTP_PACKET_HEADER_SIZE);

	RawTftpPacketType::iterator packetIt = rawPacket.begin();

	// encode opcode
	setInt( packetIt, static_cast< uint16_t>( packetType));
}

}
}
