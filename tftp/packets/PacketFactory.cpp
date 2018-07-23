/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class PacketFactory.
 **/

#include "PacketFactory.hpp"

namespace Tftp::Packets {

Tftp::PacketType PacketFactory::packetType(
  const RawTftpPacket &rawPacket) noexcept
{
  return Packet::packetType( rawPacket);
}

ReadRequestPacket PacketFactory::readRequestPacket(
  const RawTftpPacket &rawPacket)
{
  return ReadRequestPacket( rawPacket);
}

WriteRequestPacket PacketFactory::writeRequestPacket(
  const RawTftpPacket &rawPacket)
{
  return WriteRequestPacket( rawPacket);
}

DataPacket PacketFactory::dataPacket( const RawTftpPacket &rawPacket)
{
  return DataPacket( rawPacket);
}

ErrorPacket PacketFactory::errorPacket( const RawTftpPacket &rawPacket)
{
  return ErrorPacket( rawPacket);
}

AcknowledgementPacket PacketFactory::acknowledgementPacket(
  const RawTftpPacket &rawPacket)
{
  return AcknowledgementPacket( rawPacket);
}

OptionsAcknowledgementPacket PacketFactory::optionsAcknowledgementPacket(
  const RawTftpPacket &rawPacket)
{
  return OptionsAcknowledgementPacket( rawPacket);
}

}
