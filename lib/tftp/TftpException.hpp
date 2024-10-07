// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration/ definition of classes Tftp::TftpException,
 *   Tftp::CommunicationException, Tftp::OptionNegotiationException, and
 *   Tftp::ErrorReceivedException.
 **/

#ifndef TFTP_TFTPEXCEPTION_HPP
#define TFTP_TFTPEXCEPTION_HPP

#include "tftp/Tftp.hpp"

#include "tftp/packets/Packets.hpp"
#include "tftp/packets/ErrorPacket.hpp"

#include <boost/exception/exception.hpp>
#include <boost/exception/error_info.hpp>

#include <exception>

namespace Tftp {

/**
 * @brief TFTP Exception.
 **/
class TFTP_EXPORT TftpException :
  public std::exception,
  public boost::exception
{
  public:
    /**
     * @brief Returns an exception description.
     *
     * @return Exception description.
     **/
    char const * what() const noexcept override
    {
      return "TFTP exception";
    }
};

//! TFTP Communication Exception.
class TFTP_EXPORT CommunicationException : public TftpException
{
  public:
    //! @copydoc TftpException::what() const
    char const * what() const noexcept override
    {
      return "TFTP communication exception";
    }
};

//! TFTP Option Negotiation Exception.
class TFTP_EXPORT OptionNegotiationException: public TftpException
{
  public:
    //! @copydoc TftpException::what() const
    char const * what() const noexcept override
    {
      return "TFTP Option Negotiation exception";
    }
};

//! TFTP Error Packet Received Exception.
class TFTP_EXPORT ErrorReceivedException: public TftpException
{
  public:
    //! @copydoc TftpException::what() const
    char const * what() const noexcept override
    {
      return "TFTP Error received exception";
    }
};

//! TFTP Transfer Phase.
using TransferPhaseInfo =
  boost::error_info< struct TransferPhaseInfoTag, TransferPhase >;

//! TFTP Error Packet Information.
using ErrorPacketInfo =
  boost::error_info< struct ErrorPacketInfoTag, Packets::ErrorPacket >;

}

#endif
