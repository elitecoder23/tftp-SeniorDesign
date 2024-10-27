// SPDX-License-Identifier: MPL-2.0
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
#include <tftp/packets/Options.hpp>

#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

#include <format>
#include <utility>

namespace Tftp::Packets {

OptionsAcknowledgementPacket::OptionsAcknowledgementPacket(
  Options options ) :
  Packet{ PacketType::OptionsAcknowledgement },
  optionsV{ std::move( options ) }
{
}

OptionsAcknowledgementPacket::OptionsAcknowledgementPacket(
  ConstRawTftpPacketSpan rawPacket ):
  Packet{ PacketType::OptionsAcknowledgement, rawPacket }
{
  decodeBody( rawPacket );
}

OptionsAcknowledgementPacket& OptionsAcknowledgementPacket::operator=(
  ConstRawTftpPacketSpan rawPacket )
{
  decodeHeader( rawPacket );
  decodeBody( rawPacket );
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

void OptionsAcknowledgementPacket::options( Options options )
{
  optionsV = std::move( options );
}

OptionsAcknowledgementPacket::operator std::string() const
{
  return std::format( "OACK: OPT: \"{}\"", Options_toString( optionsV ) );
}

RawTftpPacket OptionsAcknowledgementPacket::encode() const
{
  RawTftpPacket rawPacket( HeaderSize );

  insertHeader( rawPacket );

  // append options
  // TODO change to rawPacket.append_range when GCC supports this
  auto rawOptions{ Options_rawOptions( optionsV ) };
  rawPacket.insert( rawPacket.end(), rawOptions.begin(), rawOptions.end() );

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

  ConstRawTftpPacketSpan rawSpan{
    rawPacket.begin() + HeaderSize,
    rawPacket.end() };

  // assign options
  optionsV =
    Options_options( RawOptionsSpan{ rawSpan.begin(), rawSpan.end() } );
}

}
