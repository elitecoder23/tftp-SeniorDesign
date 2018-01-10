/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class OptionsAcknowledgementPacket.
 **/

#include "OptionsAcknowledgementPacket.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/options/OptionList.hpp>

#include <helper/Endianess.hpp>

namespace Tftp {
namespace Packets {

OptionsAcknowledgementPacket::OptionsAcknowledgementPacket(
  const Options::OptionList &options) noexcept:
  Packet( PacketType::OptionsAcknowledgement),
  options( options)
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

const Options::OptionList& OptionsAcknowledgementPacket::getOptions() const
{
  return options;
}

Options::OptionList& OptionsAcknowledgementPacket::getOptions()
{
  return options;
}

void OptionsAcknowledgementPacket::setOptions(
  const Options::OptionList &options)
{
  this->options = options;
}

const OptionsAcknowledgementPacket::string
OptionsAcknowledgementPacket::getOption(
  const string &name) const
{
  Options::OptionPtr option( options.getOption( name));

  return (option) ? static_cast< string>( *option) : string();
}

void OptionsAcknowledgementPacket::setOption(
  const std::string &name,
  const std::string &value)
{
  options.setOption( name, value);
}

OptionsAcknowledgementPacket::operator string() const
{
  return (boost::format( "OACK: OPT: \"%s\"") % options.toString()).str();
}

Tftp::RawTftpPacket OptionsAcknowledgementPacket::encode() const
{
  auto rawOptions{ options.getRawOptions()};

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
  options = Options::OptionList( packetIt, rawPacket.end());
}

}
}
