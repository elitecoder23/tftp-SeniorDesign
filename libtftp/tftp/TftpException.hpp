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
 * @brief Declaration/ definition of classes Tftp::TftpException,
 *   Tftp::InvalidPacketException, Tftp::CommunicationException,
 *   Tftp::OptionNegotiationException, and Tftp::ErrorReceivedException.
 **/

#ifndef TFTP_TFTPEXCEPTION_HPP
#define TFTP_TFTPEXCEPTION_HPP

#include <tftp/Tftp.hpp>
#include <tftp/packet/Packet.hpp>
#include <tftp/packet/ErrorPacket.hpp>

#include <helper/Exception.hpp>

#include <string>

namespace Tftp {

/**
 * @brief Base class for TFTP exceptions
 **/
class TftpException: public virtual Exception
{
  public:
    virtual char const * what() const noexcept override
    {
      return "TFTP exception";
    }
};

//! Exception occurred during TFTP packet processing.
class InvalidPacketException: public virtual TftpException
{
  public:
    virtual char const * what() const noexcept override
    {
      return "TFTP Invalid Packet exception";
    }
};

//! Exception occurred during TFTP communication.
class CommunicationException: public virtual TftpException
{
  public:
    virtual char const * what() const noexcept override
    {
      return "TFTP communication exception";
    }
};

//! Exception occurred during TFTP option negotiation.
class OptionNegotiationException: public virtual TftpException
{
  public:
    virtual char const * what() const noexcept override
    {
      return "TFTP Option Negotiation exception";
    }
};

//! Exception thrown, when a TFTP ERR packet has been received
class ErrorReceivedException: public virtual TftpException
{
  public:
    virtual char const * what() const noexcept override
    {
      return "TFTP Error received exception";
    }
};

//! Definition of the TFTP transfer phase.
using TftpTransferPhaseInfo =
  boost::error_info< struct TftpTransferPhaseInfoTag, TftpTransferPhase>;

//! TFTP packet type information
using TftpPacketTypeInfo =
  boost::error_info< struct TftpPacketTypeInfoTag, PacketType>;

//! TFTP error packet information
using TftpErrorPacketInfo =
  boost::error_info< struct TftpErrorPacketInfoTag, Packet::ErrorPacket>;

}

#endif
