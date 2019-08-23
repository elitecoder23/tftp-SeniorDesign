/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::WriteRequestPacket.
 **/

#include "WriteRequestPacket.hpp"

namespace Tftp::Packets {

WriteRequestPacket::WriteRequestPacket(
  std::string_view filename,
  const TransferMode mode,
  const Options::OptionList &options) noexcept:
  ReadWriteRequestPacket{
    PacketType::WriteRequest,
    filename,
    mode,
    options}
{
}

WriteRequestPacket::WriteRequestPacket(
  std::string &&filename,
  TransferMode mode,
  Options::OptionList &&options) noexcept:
  ReadWriteRequestPacket{
    PacketType::WriteRequest,
    std::move( filename),
    mode,
    std::move( options)}
{
}

WriteRequestPacket::WriteRequestPacket( const RawTftpPacket &rawPacket):
  ReadWriteRequestPacket{ PacketType::WriteRequest, rawPacket}
{
}

}
