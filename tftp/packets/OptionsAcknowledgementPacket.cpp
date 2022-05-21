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

#include <helper/Endianess.hpp>

namespace Tftp::Packets {

OptionsAcknowledgementPacket::OptionsAcknowledgementPacket(
  const Options &options) noexcept:
  Packet{ PacketType::OptionsAcknowledgement},
  optionsV{ options}
{
}

OptionsAcknowledgementPacket::OptionsAcknowledgementPacket(
  Options &&options) noexcept:
  Packet{ PacketType::OptionsAcknowledgement},
  optionsV{ std::move( options)}
{
}

OptionsAcknowledgementPacket::OptionsAcknowledgementPacket(
  ConstRawTftpPacketSpan rawPacket):
  Packet{ PacketType::OptionsAcknowledgement, rawPacket}
{
  decodeBody( rawPacket);
}

OptionsAcknowledgementPacket& OptionsAcknowledgementPacket::operator=( ConstRawTftpPacketSpan rawPacket)
{
  decodeHeader( rawPacket);
  decodeBody( rawPacket);
  return *this;
}

const Options& OptionsAcknowledgementPacket::options() const
{
  return optionsV;
}

Options& OptionsAcknowledgementPacket::options()
{
  return optionsV;
}

void OptionsAcknowledgementPacket::options( const Options &options)
{
  optionsV = options;
}

void OptionsAcknowledgementPacket::options( Options &&options)
{
  optionsV = std::move( options);
}

OptionsAcknowledgementPacket::operator std::string() const
{
  return (boost::format( "OACK: OPT: \"%s\"" ) %
    TftpOptions_toString( optionsV ) ).str();
}

RawTftpPacket OptionsAcknowledgementPacket::encode() const
{
  auto rawOptions{ TftpOptions_rawOptions( optionsV ) };

  RawTftpPacket rawPacket( HeaderSize + rawOptions.size() );

  insertHeader( rawPacket );

  auto packetIt{ rawPacket.begin() + HeaderSize };

  // options
  std::copy( rawOptions.begin(), rawOptions.end(), packetIt);

  return rawPacket;
}

void OptionsAcknowledgementPacket::decodeBody(
  ConstRawTftpPacketSpan rawPacket )
{
  // check size
  if ( rawPacket.size() <= HeaderSize )
  {
    BOOST_THROW_EXCEPTION( InvalidPacketException()
      << Helper::AdditionalInfo{ "Invalid packet size of OACK packet" } );
  }

  auto packetIt{ rawPacket.begin() + HeaderSize };

  // assign options
  optionsV = TftpOptions_options( RawOptionsSpan{ packetIt, rawPacket.end() } );
}

}
