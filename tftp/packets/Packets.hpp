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
 * @brief Declaration of the Tftp::Packets namespace.
 **/

#ifndef TFTP_PACKETS_PACKETS_HPP
#define TFTP_PACKETS_PACKETS_HPP

#include <tftp/Tftp.hpp>

namespace Tftp {

/**
 * @brief Contains the implementation of all TFTP packets.
 *
 * The base for the packets is the class TftpPacket.
 * For raw-data to TFTP packet handling, the class PacketFactory with its
 * static methods can be used.
 **/
namespace Packets {

class Packet;
class ReadRequestPacket;
class WriteRequestPacket;
class DataPacket;
class AcknowledgementPacket;
class ErrorPacket;
class OptionsAcknowledgementPacket;

class BlockNumber;

class PacketFactory;
}
}

#endif
