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
  std::string filename,
  TransferMode mode,
  Options options ) noexcept:
  ReadWriteRequestPacket{
    PacketType::ReadRequest,
    std::move( filename ),
    mode,
    std::move( options ) }
{
}

ReadRequestPacket::ReadRequestPacket( ConstRawTftpPacketSpan rawPacket ) :
  ReadWriteRequestPacket( PacketType::ReadRequest, rawPacket )
{
}

ReadRequestPacket& ReadRequestPacket::operator=(
  ConstRawTftpPacketSpan rawPacket )
{
  decodeHeader( rawPacket );
  decodeBody( rawPacket );
  return *this;
}

}
