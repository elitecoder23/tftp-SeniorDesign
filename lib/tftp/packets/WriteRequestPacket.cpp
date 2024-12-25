// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::WriteRequestPacket.
 **/

#include "WriteRequestPacket.hpp"

namespace Tftp::Packets {

WriteRequestPacket::WriteRequestPacket( std::string filename, TransferMode mode, Options options ) noexcept :
  ReadWriteRequestPacket{ PacketType::WriteRequest, std::move( filename ), mode, std::move( options ) }
{
}

WriteRequestPacket::WriteRequestPacket( ConstRawDataSpan rawPacket ) :
  ReadWriteRequestPacket{ PacketType::WriteRequest, rawPacket }
{
}

WriteRequestPacket& WriteRequestPacket::operator=( ConstRawDataSpan rawPacket )
{
  decodeHeader( rawPacket);
  decodeBody( rawPacket);
  return *this;
}

}
