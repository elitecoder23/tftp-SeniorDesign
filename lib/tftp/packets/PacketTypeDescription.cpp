// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::PacketTypeDescription.
 **/

#include "PacketTypeDescription.hpp"

#include <ostream>

namespace Tftp::Packets {

PacketTypeDescription::PacketTypeDescription():
  Description{
    { "ReadRequest",            PacketType::ReadRequest },
    { "WriteRequest",           PacketType::WriteRequest },
    { "Data",                   PacketType::Data },
    { "Acknowledgement",        PacketType::Acknowledgement },
    { "Error",                  PacketType::Error },
    { "OptionsAcknowledgement", PacketType::OptionsAcknowledgement }
  }
{
}

std::ostream& operator<<( std::ostream &stream, const PacketType packetType )
{
  return stream << PacketTypeDescription::instance().name( packetType );
}

}
