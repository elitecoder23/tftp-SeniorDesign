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
 * @brief Definition of class PacketFactory.
 **/

#include "PacketFactory.hpp"

#include <helper/Logger.hpp>

namespace Tftp {
namespace Packets {

Tftp::PacketType PacketFactory::getPacketType(
  const RawTftpPacketType &rawPacket) noexcept
{
  return Packet::getPacketType( rawPacket);
}

ReadRequestPacket PacketFactory::getReadRequestPacket(
  const RawTftpPacketType &rawPacket)
{
  return ReadRequestPacket( rawPacket);
}

WriteRequestPacket PacketFactory::getWriteRequestPacket(
  const RawTftpPacketType &rawPacket)
{
  return WriteRequestPacket( rawPacket);
}

DataPacket PacketFactory::getDataPacket(
  const RawTftpPacketType &rawPacket)
{
  return DataPacket( rawPacket);
}

ErrorPacket PacketFactory::getErrorPacket(
  const RawTftpPacketType &rawPacket)
{
  return ErrorPacket( rawPacket);
}

AcknowledgementPacket PacketFactory::getAcknowledgementPacket(
  const RawTftpPacketType &rawPacket)
{
  return AcknowledgementPacket( rawPacket);
}

OptionsAcknowledgementPacket PacketFactory::getOptionsAcknowledgementPacket(
  const RawTftpPacketType &rawPacket)
{
  return OptionsAcknowledgementPacket( rawPacket);
}

}
}
