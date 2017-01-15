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
 * @brief Basis definition for TFTP implementation.
 **/

#ifndef TFTP_TFTP_HPP
#define TFTP_TFTP_HPP

#include <boost/asio.hpp>

#include <string>
#include <vector>
#include <cstdint>

/**
 * @mainpage
 * @brief Implementation of the TFTP protocol.
 *
 * This library implementation the TFTP protocol.
 *
 * The library is divided into sections:
 * - Tftp - Contains the basic definitions.
 * - Tftp::Packet - Contains the implementation of all TFTP packet types
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

/**
 * @brief Basic definitions of the TFTP protocol
 **/
namespace Tftp {

//! The packet type of raw data
using RawTftpPacketType = std::vector< uint8_t>;

//! The address type for TFTP operations
//! shortened form of the IP address type (v4 + v6)
using IpAddressType = boost::asio::ip::address;

//! shortened form of the UDP address type (IP + UDP Port)
using UdpAddressType = boost::asio::ip::udp::endpoint;

/**
 * @brief TFTP version information
 **/
enum class TftpVersion
{
  //! TFTP Version 2 (RFC1350)
  TFTP_VERSION_2,
  //! TFTP Version 2 with Options Extension (RFC1350 + RFC2347)
  TFTP_VERSION_2_WITH_OPTIONS_EXTENSION,

  //! Invalid entry
  TFTP_VERSION_LAST
};

//! TFTP role enumeration
enum class TftpRole
{
  TFTP_ROLE_CLIENT, //!< TFTP client role
  TFTP_ROLE_SERVER, //!< TFTP server role

  TFTP_ROLE_LAST    //!< invalid value
};

//! TFTP request type
enum class TftpRequestType
{
  ReadRequest,
  WriteRequest,
  Invalid
};
//! @brief Phases of TFTP transfer
enum class TftpTransferPhase
{
  //! Initialisation phase before any request has been sent/ received.
  TFTP_PHASE_INITIALIZATION,
  //! TFTP request phase RRQ/WRQ and wait for ACK.
  TFTP_PHASE_REQUEST,
  //! TFTP option negotiation phase wait for OACK.
  TFTP_PHASE_OPTION_NEGOTIATION,
  //! TFTP data transfer phase.
  TFTP_PHASE_DATA_TRANSFER,
  //! TFTP transfer phase unknown.
  TFTP_PHASE_UNKNOWN
};

//! Default TFTP port.
constexpr uint16_t DEFAULT_TFTP_PORT = 69;

//! The default TFTP receive timeout in seconds (2 seconds)
constexpr unsigned int DEFAULT_TFTP_RECEIVE_TIMEOUT = 2;

//! Number of retries performed, when no ACK has been received
constexpr unsigned int DEFAULT_TFTP_RETRIES = 1;

/**
 * @brief TFTP Packet types.
 *
 * All packet types, except TFTP_PACKET_OPTIONS_ACKNOWLEDGEMENT (6), are
 * defined within RFC 1350. The packet type
 * TFTP_PACKET_OPTIONS_ACKNOWLEDGEMENT (6) is described within RFC 2347.
 **/
enum class PacketType : uint16_t
{
  READ_REQUEST            = 1, //!< Read request (RRQ)
  WRITE_REQUEST           = 2, //!< Write request (WRQ)
  DATA                    = 3, //!< Data (DATA)
  ACKNOWLEDGEMENT         = 4, //!< Acknowledgement (ACK)
  ERROR                   = 5, //!< Error (ERROR)
  OPTIONS_ACKNOWLEDGEMENT = 6, //!< Options Acknowledgement (OACK)

  INVALID                      //!< Invalid value
};

//! Maximum size of data field in data package (without blksize option)
constexpr unsigned int DEFAULT_DATA_SIZE = 512;

//! Size of TFTP header in data package
constexpr unsigned int DEFAULT_TFTP_DATA_PACKET_HEADER_SIZE = 4;

//! Maximum size of TFTP package (without blksize option)
constexpr unsigned int DEFAULT_MAX_PACKET_SIZE =
	DEFAULT_DATA_SIZE + DEFAULT_TFTP_DATA_PACKET_HEADER_SIZE;

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
  NOT_DEFINED                     = 0,
  //! File not found
  FILE_NOT_FOUND                  = 1,
  //! Access violation.
  ACCESS_VIOLATION                = 2,
  //! Disk full or allocation exceeded.
  DISK_FULL_OR_ALLOCATION_EXCEEDS = 3,
  //! Illegal TFTP operation.
  ILLEGAL_TFTP_OPERATION          = 4,
  //! Unknown transfer ID.
  UNKNOWN_TRANSFER_ID             = 5,
  //! File already exists.
  FILE_ALLREADY_EXISTS            = 6,
  //! No such user.
  NO_SUCH_USER                    = 7,
  //! TFTP options refused during option negotiation
  TFTP_OPTION_REFUSED             = 8
};

//! Enumeration of all known TFTP options
enum class TftpOptions
{
  //! Block size option (RFC 2348)
  BLOCKSIZE,
  //! Timeout option (RFC 2349)
  TIMEOUT,
  //! Transfer size option (RFC 2349)
  TRANSFER_SIZE
};

//! Minimum TFTP block size option as defined within RFC 2348
constexpr unsigned int TFTP_OPTION_BLOCKSIZE_MIN = 8;
//! Maximum TFTP block size option as defined within RFC 2348
constexpr unsigned int TFTP_OPTION_BLOCKSIZE_MAX = 65464;

//! Minimum TFTP timeout option as defined within RFC 2349
constexpr unsigned int TFTP_OPTION_TIMEOUT_MIN = 1;
//! maximum TFTP timeout option as defined within RFC 2349
constexpr unsigned int TFTP_OPTION_TIMEOUT_MAX = 255;

// Forward declarations
class TftpConfiguration;
class TftpReceiveDataOperationHandler;
class TftpTransmitDataOperationHandler;
class TftpPacketHandler;

}

#endif
