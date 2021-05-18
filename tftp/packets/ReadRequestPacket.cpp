/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::ReadRequestPacket.
 **/

#include "ReadRequestPacket.hpp"

namespace Tftp::Packets {

ReadRequestPacket::ReadRequestPacket(
  std::string_view filename,
  const TransferMode mode,
  const Options &options) noexcept:
  ReadWriteRequestPacket{
    PacketType::ReadRequest,
    filename,
    mode,
    options}
{
}

ReadRequestPacket::ReadRequestPacket(
  std::string &&filename,
  TransferMode mode,
  Options &&options) noexcept:
  ReadWriteRequestPacket{
    PacketType::ReadRequest,
    std::move( filename),
    mode,
    std::move( options)}
{
}

ReadRequestPacket::ReadRequestPacket( const RawTftpPacket &rawPacket):
  ReadWriteRequestPacket( PacketType::ReadRequest, rawPacket)
{
}

ReadRequestPacket& ReadRequestPacket::operator=(
  const RawTftpPacket &rawPacket)
{
  decodeHeader( rawPacket);
  decodeBody( rawPacket);
  return *this;
}

}
