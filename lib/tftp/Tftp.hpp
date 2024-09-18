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
 * @brief Declaration of Namespace Tftp.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Tftp.
 **/

#ifndef TFTP_TFTP_HPP
#define TFTP_TFTP_HPP

#include "tftp/tftp_export.h"

#include "helper/Helper.hpp"

#include <chrono>
#include <cstdint>
#include <memory>

/**
 * @brief TFTP Protocol.
 *
 * This library implements the TFTP protocol.
 *
 * The library is divided into sections:
 * - Tftp.
 * - Tftp::Packets - TFTP Packets
 * - Tftp::Options - TFTP Option Handling
 * - Tftp::Client - TFTP Clients
 * - Tftp::Server - TFTP Servers
 * - Tftp::File - Helper Classes for file transfers
 *
 * @par Referenced Documents
 * - RFC 1350 The TFTP Protocol (Revision 2)<br>
 *   http://tools.ietf.org/html/rfc1350
 * - RFC 2347 TFTP Option Extension<br>
 *   http://tools.ietf.org/html/rfc2347
 * - RFC 2348 TFTP Blocksize Option<br>
 *   http://tools.ietf.org/html/rfc2348
 * - RFC 2349 TFTP Timeout Interval and Transfer Size Options<br>
 *   http://tools.ietf.org/html/rfc2349
 **/
namespace Tftp {

//!  TFTP Version Information
enum class TftpVersion
{
  //! TFTP Version 2 (RFC1350)
  Version2,
  //! TFTP Version 2 with Options Extension (RFC1350 + RFC2347)
  Version2WithOptionsExtension
};

//! TFTP Role Enumeration
enum class Role
{
  Client, //!< TFTP Client Role
  Server  //!< TFTP Server Role
};

//! TFTP Request Type
enum class RequestType
{
  Read,
  Write
};

//! @brief Phases of TFTP Transfer
enum class TransferPhase
{
  //! Initialisation phase before any request has been sent/ received.
  Initialisation,
  //! TFTP request phase RRQ/WRQ and wait for ACK.
  Request,
  //! TFTP option negotiation phase wait for OACK.
  OptionNegotiation,
  //! TFTP data transfer phase.
  DataTransfer,

  //! TFTP transfer phase unknown.
  Unknown
};

//! TFTP Transfer Status
enum class TransferStatus
{
  //! Transfer completed successfully
  Successful,
  //! Communication error (i.e. timeout, read error) occurred
  CommunicationError,
  //! Error received/ transmitted - RRQ/ WRQ rejected
  RequestError,
  //! Error received/ transmitted - Option Negotiation Failed
  OptionNegotiationError,
  //! Error received/ transmitted - Invalid data or packets
  TransferError,
  //! User (own side) requested abort (gracefully or not gracefully)
  Aborted,
};

//! Default TFTP Port.
constexpr uint16_t DefaultTftpPort{ 69U };

//! Default TFTP Receive Timeout in Seconds (2 seconds)
constexpr std::chrono::seconds DefaultTftpReceiveTimeout{ 2U };

//! Number of retries performed, when no ACK has been received
constexpr uint16_t DefaultTftpRetries{ 1U };

// Forward declarations
class TftpConfiguration;
class TftpOptionsConfiguration;

class DataHandler;
class ReceiveDataHandler;
class TransmitDataHandler;

//! Receive Data Handler Pointer
using ReceiveDataHandlerPtr = std::shared_ptr< ReceiveDataHandler >;
//! Transmit Data Handler Pointer
using TransmitDataHandlerPtr =  std::shared_ptr< TransmitDataHandler >;

class Version;

/**
 * @brief Provide TFTP Library Version Information.
 *
 * @return Version Information.
 **/
[[nodiscard]] TFTP_EXPORT Helper::VersionsInformation::value_type tftpVersion();

}

#endif
