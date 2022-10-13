/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration/ Definition of Class
 *   Tftp::Packets::InvalidPacketException.
 **/

#ifndef TFTP_PACKETS_PACKETEXCEPTION_HPP
#define TFTP_PACKETS_PACKETEXCEPTION_HPP

#include <tftp/Tftp.hpp>
#include <tftp/TftpException.hpp>

namespace Tftp::Packets {

//! TFTP %Packet Processing Exception.
class InvalidPacketException: public virtual TftpException
{
  public:
    //! @copydoc TftpException::what() const
    char const * what() const noexcept override
    {
      return "TFTP Invalid Packet exception";
    }
};

//! TFTP Packet Type Information.
using PacketTypeInfo =
  boost::error_info< struct PacketTypeInfoTag, PacketType >;

}

#endif
