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

#include <tftp/packets/PacketException.hpp>

#include <tftp/options/OptionList.hpp>

#include <helper/Endianess.hpp>

namespace Tftp::Packets {

OptionsAcknowledgementPacket::OptionsAcknowledgementPacket(
  const Options::OptionList &options) noexcept:
  Packet{ PacketType::OptionsAcknowledgement},
  optionsValue{ options}
{
}

OptionsAcknowledgementPacket::OptionsAcknowledgementPacket(
  Options::OptionList &&options) noexcept:
  Packet{ PacketType::OptionsAcknowledgement},
  optionsValue{ std::move( options)}
{
}

OptionsAcknowledgementPacket::OptionsAcknowledgementPacket(
  const RawTftpPacket &rawPacket):
  Packet{ PacketType::OptionsAcknowledgement, rawPacket}
{
  decodeBody( rawPacket);
}

OptionsAcknowledgementPacket& OptionsAcknowledgementPacket::operator=(
  const RawTftpPacket &rawPacket)
{
  // call inherited operator
  Packet::operator =( rawPacket);
  // decode body
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

void OptionsAcknowledgementPacket::options( Options::OptionList &&options)
{
  optionsValue = std::move( options);
}

std::string OptionsAcknowledgementPacket::option(
  const std::string &name) const
{
  auto option{ optionsValue.get( name)};

  return (option) ? static_cast< std::string>( *option) : std::string();
}

void OptionsAcknowledgementPacket::option(
  const std::string &name,
  const std::string &value)
{
  optionsValue.set( name, value);
}

void OptionsAcknowledgementPacket::option(
  std::string &&name,
  std::string &&value)
{
  optionsValue.set( std::move( name), std::move( value));
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

  auto packetIt{ rawPacket.begin() + HeaderSize};

  // options
  std::copy( rawOptions.begin(), rawOptions.end(), packetIt);

  return rawPacket;
}

void OptionsAcknowledgementPacket::decodeBody( const RawTftpPacket &rawPacket)
{
  // check size
  if (rawPacket.size() <= HeaderSize)
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << AdditionalInfo( "Invalid packet size of OACK packet"));
  }

  auto packetIt = rawPacket.begin() + HeaderSize;

  // assign options
  optionsValue = Options::OptionList( packetIt, rawPacket.end());
}

}
