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
  const Options::Options &options) noexcept:
  Packet{ PacketType::OptionsAcknowledgement},
  optionsValue{ options}
{
}

OptionsAcknowledgementPacket::OptionsAcknowledgementPacket(
  Options::Options &&options) noexcept:
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

const Options::Options& OptionsAcknowledgementPacket::options() const
{
  return optionsValue;
}

Options::Options& OptionsAcknowledgementPacket::options()
{
  return optionsValue;
}

void OptionsAcknowledgementPacket::options( const Options::Options &options)
{
  optionsValue = options;
}

void OptionsAcknowledgementPacket::options( Options::Options &&options)
{
  optionsValue = std::move( options);
}

OptionsAcknowledgementPacket::operator std::string() const
{
  return (boost::format( "OACK: OPT: \"%s\"") %
    Options::OptionList::toString( optionsValue)).str();
}

Tftp::RawTftpPacket OptionsAcknowledgementPacket::encode() const
{
  auto rawOptions{ Options::OptionList::rawOptions( optionsValue)};

  RawTftpPacket rawPacket( 2U + rawOptions.size());

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

  auto packetIt{ rawPacket.begin() + HeaderSize};

  // assign options
  optionsValue = Options::OptionList::options( packetIt, rawPacket.end());
}

}
