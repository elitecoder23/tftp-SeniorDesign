/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Packets::PacketTypeDescription.
 **/

#ifndef TFTP_PACKETS_PACKETTYPEDESCRIPTION_HPP
#define TFTP_PACKETS_PACKETTYPEDESCRIPTION_HPP

#include <tftp/packets/Packets.hpp>

#include <helper/Description.hpp>

#include <iosfwd>

namespace Tftp::Packets {

//! Description of TFTP Error Codes (ErrorCode)
class TFTP_EXPORT PacketTypeDescription :
  public Helper::Description< PacketTypeDescription, PacketType >
{
  public:
    //! Initialises the Instance
    PacketTypeDescription();
};

/**
 * @brief Outputs the error code to the stream.
 *
 * @param[in] stream
 *   Output stream.
 * @param[out] packetType
 *   Packet Type.
 *
 * @return @p stream for chaining.
 **/
TFTP_EXPORT std::ostream&
operator<<( std::ostream& stream, PacketType packetType );

}

#endif
