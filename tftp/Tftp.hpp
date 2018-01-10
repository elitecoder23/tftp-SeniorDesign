/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of namespace Tftp.
 **/

#ifndef TFTP_TFTP_HPP
#define TFTP_TFTP_HPP

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/udp.hpp>

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

/**
 * @brief Implementation of the TFTP protocol.
 *
 * This library implementation the TFTP protocol.
 *
 * The library is divided into sections:
 * - Tftp - Contains the basic definitions.
 * - Tftp::Packets - Contains the implementation of all TFTP packet types
 * - Tftp::Options - Contains the implementation of the TFTP Option handling
 * - Tftp::Client - Base implementation of TFTP clients
 * - Tftp::Server - Base implementation of TFTP servers
 * - Tftp::File - Helper classes for file transfers
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

//! The packet type of raw data
using RawTftpPacket = std::vector< uint8_t>;

//! The address type for TFTP operations
//! shortened form of the IP address type (v4 + v6)
using IpAddressType = boost::asio::ip::address;

//! shortened form of the UDP address type (IP + UDP Port)
using UdpAddressType = boost::asio::ip::udp::endpoint;

//!  TFTP version information
enum class Version
{
  //! TFTP Version 2 (RFC1350)
  Version2,
  //! TFTP Version 2 with Options Extension (RFC1350 + RFC2347)
  Version2WithOptionsExtension,

  //! Invalid entry
  Invalid
};

//! TFTP role enumeration
enum class Role
{
  Client, //!< TFTP client role
  Server, //!< TFTP server role

  Invalid //!< invalid value
};

//! TFTP request type
enum class RequestType
{
  Read,
  Write,

  Invalid
};
//! @brief Phases of TFTP transfer
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

//! The TFTP transfer status
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

  //! Invalid value
  Invalid
};

//! Default TFTP port.
constexpr uint16_t DefaultTftpPort = 69;

//! The default TFTP receive timeout in seconds (2 seconds)
constexpr unsigned int DefaultTftpReceiveTimeout = 2;

//! Number of retries performed, when no ACK has been received
constexpr unsigned int DefaultTftpRetries = 1;

/**
 * @brief TFTP Packet types.
 *
 * All packet types, except TFTP_PACKET_OPTIONS_ACKNOWLEDGEMENT (6), are
 * defined within RFC 1350. The packet type
 * TFTP_PACKET_OPTIONS_ACKNOWLEDGEMENT (6) is described within RFC 2347.
 **/
enum class PacketType : uint16_t
{
  ReadRequest            = 1, //!< Read request (RRQ)
  WriteRequest           = 2, //!< Write request (WRQ)
  Data                   = 3, //!< Data (DATA)
  Acknowledgement        = 4, //!< Acknowledgement (ACK)
  Error                  = 5, //!< Error (ERROR)
  OptionsAcknowledgement = 6, //!< Options Acknowledgement (OACK)

  Invalid                     //!< Invalid value
};

//! Maximum size of data field in data package (without blksize option)
constexpr size_t DefaultDataSize = 512U;

//! Size of TFTP header in data package (Opcode + Blocknumber)
constexpr size_t DefaultTftpDataPacketHeaderSize = 4U;

//! Maximum size of TFTP package (without blksize option)
constexpr size_t DefaultMaxPacketSize =
  DefaultDataSize + DefaultTftpDataPacketHeaderSize;

//! TFTP transfer modes.
enum class TransferMode
{
  OCTET,    //!< OCTET transfer mode (binary)
  NETASCII, //!< NETASCII transfer mode.
  MAIL,     //!< MAIL transfer mode (deprecated).

  INVALID   //!< Invalid value
};

/**
 * @brief The TFTP Error codes as defined within the RFCs.
 *
 * The error codes, except the ERROR_CODE_TFTP_OPTION_REFUSED (8) are
 * described within RFC 1350.
 * The error code ERROR_CODE_TFTP_OPTION_REFUSED (8) is described within
 * RFC 2347.
 **/
enum class ErrorCode : uint16_t
{
  //! Not defined, see error message (if any).
  NotDefined                  = 0U,
  //! File not found
  FileNotFound                = 1U,
  //! Access violation.
  AccessViolation             = 2U,
  //! Disk full or allocation exceeded.
  DiskFullOrAllocationExceeds = 3U,
  //! Illegal TFTP operation.
  IllegalTftpOperation        = 4U,
  //! Unknown transfer ID.
  UnknownTransferId           = 5U,
  //! File already exists.
  FileAllreadyExists          = 6U,
  //! No such user.
  NoSuchUser                  = 7U,
  //! TFTP options refused during option negotiation
  TftpOptionRefused           = 8U,

  Invalid                     = 0xFFFFU
};

//! Enumeration of all known TFTP options
enum class KnownOptions
{
  //! Block size option (RFC 2348)
  BlockSize,
  //! Timeout option (RFC 2349)
  Timeout,
  //! Transfer size option (RFC 2349)
  TransferSize
};

//! Minimum TFTP block size option as defined within RFC 2348
constexpr uint16_t BlocksizeOptionMin = 8U;
//! Maximum TFTP block size option as defined within RFC 2348
constexpr uint16_t BlocksizeOptionMax = 65464U;

//! Minimum TFTP timeout option as defined within RFC 2349
constexpr uint8_t TimeoutOptionMin = 1U;
//! maximum TFTP timeout option as defined within RFC 2349
constexpr uint8_t TimeoutOptionMax = 255U;

// Forward declarations
class TftpConfiguration;

class DataHandler;
class ReceiveDataHandler;
class TransmitDataHandler;

//! Receive data handler pointer
using ReceiveDataHandlerPtr = std::shared_ptr< ReceiveDataHandler>;
//! Transmit data handler pointer
using TransmitDataHandlerPtr =  std::shared_ptr< TransmitDataHandler>;

//! Operation Completed handler, which indicates, if the transfer was successful
using OperationCompletedHandler = std::function< void( TransferStatus)>;

class PacketHandler;

}

#endif
