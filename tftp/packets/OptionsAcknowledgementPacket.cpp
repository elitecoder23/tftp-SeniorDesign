/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::OptionsAcknowledgementPacket.
 **/

#include "OptionsAcknowledgementPacket.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/options/OptionList.hpp>

#include <helper/Endianess.hpp>

namespace Tftp::Packets {

OptionsAcknowledgementPacket::OptionsAcknowledgementPacket(
  const Options::OptionList &options) noexcept:
  Packet( PacketType::OptionsAcknowledgement),
  optionsValue( options)
{
}

OptionsAcknowledgementPacket::OptionsAcknowledgementPacket(
  const RawTftpPacket &rawPacket):
  Packet( PacketType::OptionsAcknowledgement, rawPacket)
{
  decodeBody( rawPacket);
}

OptionsAcknowledgementPacket& OptionsAcknowledgementPacket::operator=(
  const RawTftpPacket &rawPacket)
{
  Packet::operator =( rawPacket);
  decodeBody( rawPacket);
  return *this;
}

const Options::OptionList& OptionsAcknowledgementPacket::options() const
{
  return optionsValue;
}

Options::OptionList& OptionsAcknowledgementPacket::options()
{
  return optionsValue;
}

void OptionsAcknowledgementPacket::options(
  const Options::OptionList &options)
{
  optionsValue = options;
}

const std::string OptionsAcknowledgementPacket::option(
  const std::string &name) const
{
  Options::OptionPtr option( optionsValue.get( name));

  return (option) ? static_cast< std::string>( *option) : std::string();
}

void OptionsAcknowledgementPacket::option(
  const std::string &name,
  const std::string &value)
{
  optionsValue.set( name, value);
}

OptionsAcknowledgementPacket::operator std::string() const
{
  return (boost::format( "OACK: OPT: \"%s\"") % optionsValue.toString()).str();
}

Tftp::RawTftpPacket OptionsAcknowledgementPacket::encode() const
{
  auto rawOptions{ optionsValue.rawOptions()};

  RawTftpPacket rawPacket( 2 + rawOptions.size());

  insertHeader( rawPacket);

  RawTftpPacket::iterator packetIt = rawPacket.begin() + 2;

  // options
  std::copy( rawOptions.begin(), rawOptions.end(), packetIt);

  return rawPacket;
}

void OptionsAcknowledgementPacket::decodeBody( const RawTftpPacket &rawPacket)
{
  // check size
  if (rawPacket.size() <= 2)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException() <<
      AdditionalInfo( "Invalid packet size of OACK packet"));
  }

  RawTftpPacket::const_iterator packetIt = rawPacket.begin() + 2;

  // assign options
  optionsValue = Options::OptionList( packetIt, rawPacket.end());
}

}
