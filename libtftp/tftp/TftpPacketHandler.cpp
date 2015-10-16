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
 * @brief Definition of class TftpPacketHandler.
 **/

#include "TftpPacketHandler.hpp"

#include <tftp/packet/PacketFactory.hpp>
#include <tftp/TftpException.hpp>

#include <helper/Logger.hpp>

using namespace Tftp;
using Tftp::Packet::PacketFactory;

void TftpPacketHandler::handlePacket(
	const UdpAddressType &from,
	const RawTftpPacketType &rawPacket)
{
	BOOST_LOG_FUNCTION();

	switch (PacketFactory::getPacketType( rawPacket))
	{
		case PacketType::READ_REQUEST:
			try
			{
				handleReadRequestPacket(
					from,
					PacketFactory::getReadRequestPacket( rawPacket));
			}
			catch ( InvalidPacketException &e)
			{
				BOOST_LOG_TRIVIAL( error) << "Error decoding/ handling RRQ packet: " << e.what();
				handleInvalidPacket( from, rawPacket);
			}
			break;

		case PacketType::WRITE_REQUEST:
			try
			{
				handleWriteRequestPacket(
					from,
					PacketFactory::getWriteRequestPacket(rawPacket));
			}
			catch ( InvalidPacketException &e)
			{
				BOOST_LOG_TRIVIAL( error) << "Error decoding/ handling WRQ packet: " << e.what();
				handleInvalidPacket( from, rawPacket);
			}
			break;

		case PacketType::DATA:
			try
			{
				handleDataPacket(
					from,
					PacketFactory::getDataPacket( rawPacket));
			}
			catch ( InvalidPacketException &e)
			{
				BOOST_LOG_TRIVIAL( error) <<  "Error decoding/ handling DATA packet: " << e.what();
				handleInvalidPacket( from, rawPacket);
			}
			break;

		case PacketType::ACKNOWLEDGEMENT:
			try
			{
				handleAcknowledgementPacket(
					from,
					PacketFactory::getAcknowledgementPacket(rawPacket));
			}
			catch ( InvalidPacketException &e)
			{
				BOOST_LOG_TRIVIAL( error) << "Error decoding/ handling ACK packet: " << e.what();
				handleInvalidPacket( from, rawPacket);
			}
			break;

		case PacketType::ERROR:
			try
			{
				handleErrorPacket(
					from,
					PacketFactory::getErrorPacket( rawPacket));
			}
			catch ( InvalidPacketException &e)
			{
				BOOST_LOG_TRIVIAL( error) << "Error decoding/ handling ERR packet: " << e.what();
				handleInvalidPacket( from, rawPacket);
			}
			break;

		case PacketType::OPTIONS_ACKNOWLEDGEMENT:
			try
			{
				handleOptionsAcknowledgementPacket(
					from,
					PacketFactory::getOptionsAcknowledgementPacket( rawPacket));
			}
			catch ( InvalidPacketException &e)
			{
				BOOST_LOG_TRIVIAL( error) << "Error decoding/ handling OACK packet: " << e.what();
				handleInvalidPacket( from, rawPacket);
			}
			break;

		default:
			handleInvalidPacket( from, rawPacket);
			break;
	}
}
