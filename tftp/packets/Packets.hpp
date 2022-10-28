/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Tftp::Packets.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Tftp::Packets.
 **/

#ifndef TFTP_PACKETS_PACKETS_HPP
#define TFTP_PACKETS_PACKETS_HPP

#include <tftp/Tftp.hpp>

#include <map>
#include <vector>
#include <span>
#include <cstdint>
#include <string>
#include <string_view>

/**
 * @brief TFTP %Packets.
 *
 * The base for the packets is the class TftpPacket.
 * For raw-data to TFTP packet handling, the class PacketFactory with its
 * static methods can be used.
 **/
namespace Tftp::Packets {


class Packet;
class ReadRequestPacket;
class WriteRequestPacket;
class DataPacket;
class AcknowledgementPacket;
class ErrorPacket;
class OptionsAcknowledgementPacket;

class BlockNumber;

class PacketHandler;

struct TftpOptions;

/**
 * @brief TFTP Packet Types.
 *
 * All packet types, except TFTP_PACKET_OPTIONS_ACKNOWLEDGEMENT (6), are
 * defined within RFC 1350.
 * The packet type TFTP_PACKET_OPTIONS_ACKNOWLEDGEMENT (6) is described within
 * RFC 2347.
 **/
enum class PacketType : uint16_t
{
  ReadRequest            = 1U,   //!< Read request (RRQ)
  WriteRequest           = 2U,   //!< Write request (WRQ)
  Data                   = 3U,   //!< Data (DATA)
  Acknowledgement        = 4U,   //!< Acknowledgement (ACK)
  Error                  = 5U,   //!< Error (ERROR)
  OptionsAcknowledgement = 6U,   //!< Options Acknowledgement (OACK)

  Invalid                = 0xFFU //!< Invalid value
};

//! Maximum size of data field in data package (without blksize option)
constexpr uint16_t DefaultDataSize{ 512U };

//! Size of TFTP header in data package (Opcode + Block Number)
constexpr uint16_t DefaultTftpDataPacketHeaderSize{ 4U };

//! Maximum size of TFTP package (without blksize option)
constexpr uint16_t DefaultMaxPacketSize{
  DefaultDataSize + DefaultTftpDataPacketHeaderSize };

//! TFTP Transfer Modes.
enum class TransferMode
{
  OCTET,    //!< OCTET transfer mode (binary)
  NETASCII, //!< NETASCII transfer mode.
  MAIL,     //!< MAIL transfer mode (deprecated).

  Invalid   //!< Invalid value
};

/**
 * @brief TFTP Error Codes as Defined within the RFCs.
 *
 * The error codes, except the ErrorCode::TftpOptionRefused (8) are
 * described within RFC 1350.
 * The error code ErrorCode::TftpOptionRefused (8) is described within
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
  FileAlreadyExists           = 6U,
  //! No such user.
  NoSuchUser                  = 7U,
  //! TFTP options refused during option negotiation
  TftpOptionRefused           = 8U,

  Invalid                     = 0xFFFFU
};

//! Enumeration of all known TFTP Options
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
constexpr uint16_t BlockSizeOptionMin{ 8U };
//! Maximum TFTP block size option as defined within RFC 2348
constexpr uint16_t BlockSizeOptionMax{ 65464U };

//! Minimum TFTP timeout option as defined within RFC 2349
constexpr uint8_t TimeoutOptionMin{ 1U };
//! maximum TFTP timeout option as defined within RFC 2349
constexpr uint8_t TimeoutOptionMax{ 255U };

//! Raw TFTP Packet.
using RawTftpPacket = std::vector< uint8_t >;
//! Raw TFTP Packet as std::span
using RawTftpPacketSpan = std::span< uint8_t >;
//! Constant Raw TFTP Packet as std::span
using ConstRawTftpPacketSpan = std::span< const uint8_t >;

//! Raw Options.
using RawOptions = std::vector< uint8_t >;
//! Constant Raw TFTP Options as std::span
using RawOptionsSpan = std::span< const uint8_t >;
//! TFTP Options (Maps Option Name to Option Value)
using Options = std::map< std::string, std::string, std::less< > >;

}

#endif
