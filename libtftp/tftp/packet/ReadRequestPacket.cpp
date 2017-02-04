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
 * @brief Definition of class Tftp::Packet::ReadRequestPacket.
 **/

#include "ReadRequestPacket.hpp"

namespace Tftp {
namespace Packet {

ReadRequestPacket::ReadRequestPacket(
  const string &filename,
  const TransferMode mode,
  const Options::OptionList &options) noexcept:
  ReadWriteRequestPacket(
    PacketType::ReadRequest,
    filename,
    mode,
    options)
{
}

ReadRequestPacket::ReadRequestPacket( const RawTftpPacketType &rawPacket):
  ReadWriteRequestPacket( PacketType::ReadRequest, rawPacket)
{
}

}
}
