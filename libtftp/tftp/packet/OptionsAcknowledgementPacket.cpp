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
 * @brief Definition of class OptionsAcknowledgementPacket.
 **/

#include "OptionsAcknowledgementPacket.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/options/OptionList.hpp>

#include <helper/Endianess.hpp>
#include <helper/Logger.hpp>

using namespace Tftp::Packet;

OptionsAcknowledgementPacket::OptionsAcknowledgementPacket(
	const OptionList &options) noexcept:
	TftpPacket( PacketType::OPTIONS_ACKNOWLEDGEMENT),
	options( options)
{
}

OptionsAcknowledgementPacket::OptionsAcknowledgementPacket(
	const RawTftpPacketType &rawPacket):
	TftpPacket( PacketType::OPTIONS_ACKNOWLEDGEMENT, rawPacket)
{
	//! check size
	if (rawPacket.size() <= 2)
	{
		BOOST_THROW_EXCEPTION( InvalidPacketException() <<
			AdditionalInfo( "Invalid packet size of OACK packet"));
	}

	RawTftpPacketType::const_iterator packetIt = rawPacket.begin() + 2;

	//! assign options
	options = OptionList( packetIt, rawPacket.end());
}

const OptionList& OptionsAcknowledgementPacket::getOptions( void) const
{
	return options;
}

OptionList& OptionsAcknowledgementPacket::getOptions( void)
{
	return options;
}

void OptionsAcknowledgementPacket::setOptions( const OptionList &options)
{
	this->options = options;
}

const std::string OptionsAcknowledgementPacket::getOption(
	const std::string &name) const
{
	OptionList::OptionPointer option = options.getOption( name);
	return (option) ? option->getValueString() : std::string();
}

void OptionsAcknowledgementPacket::setOption(
	const std::string &name,
	const std::string &value)
{
	options.setOption( name, value);
}

Tftp::RawTftpPacketType OptionsAcknowledgementPacket::encode( void) const
{
	OptionList::RawOptionsType rawOptions = options.getRawOptions();

	RawTftpPacketType rawPacket( 2 + rawOptions.size());

	insertHeader( rawPacket);

	RawTftpPacketType::iterator packetIt = rawPacket.begin() + 2;

	//! options
	std::copy( rawOptions.begin(), rawOptions.end(), packetIt);

	return rawPacket;
}

string OptionsAcknowledgementPacket::toString( void) const
{
	return (boost::format( "OACK: OPT: \"%s\"") % options.toString()).str();
}
