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
 * @brief Declaration of Namespace Tftp::Servers.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Tftp::Servers.
 **/

#ifndef TFTP_SERVERS_SERVERS_HPP
#define TFTP_SERVERS_SERVERS_HPP

#include "tftp/Tftp.hpp"

#include "tftp/packets/Packets.hpp"

#include <boost/asio/ip/udp.hpp>

#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string_view>

/**
 * @brief TFTP %Server.
 *
 * The user of this library uses following interface classes to interact with
 * it:
 * - @ref Server
 *   The main entry point (is also a factory for all other classes).
 * - @ref ReceivedTftpRequestHandler
 *   This interface must be implemented by the user of this library to handle
 *   new TFTP requests.
 * - @ref Operation
 *   Class interface to execute the operations.
 *   The actual operations can be created by utilising the Server class
 *   instances.
 * - @ref ReceiveDataHandler and @ref TransmitDataHandler
 *   This interface class must be implemented by the user of this library to
 *   make use of the TFTP server operations.
 **/
namespace Tftp::Servers {

// Forward declarations
class Server;
class Operation;
class ReadOperation;
class WriteOperation;

//! TFTP %Server Instance Pointer.
using ServerPtr = std::shared_ptr< Server >;

//! TFTP %Server %Operation Instance Pointer.
using OperationPtr = std::shared_ptr< Operation >;

//! TFTP %Server Read %Operation Instance Pointer.
using ReadOperationPtr = std::shared_ptr< ReadOperation >;

//! TFTP %Server Write %Operation Instance Pointer.
using WriteOperationPtr = std::shared_ptr< WriteOperation >;

/**
 * @brief Received TFTP Request Handler.
 *
 * The registered handler is called, when a TFTP Server instance received a
 * TFTP request.
 * The handler can analyse it and responds with:
 * - TFTP %Server Error %Operation
 * - TFTP %Server Read Request
 * - TFTP %Server Write Request
 *
 * @param[in] remote
 *   Remote Endpoint
 * @param[in] requestType
 *   TFTP Request Type
 * @param[in] filename
 *   Filename
 * @param[in] mode
 *   Transfer Mode
 * @param[in] clientOptions
 *   Received TFTP %Client %Options (TFTP specific).
 *   Should be passed to server operation unmodified.
 * @param[in] additionalClientOptions
 *   Received TFTP %Client %Options (All Others).
 *   For additional Option Negotiation.
 *
 * @sa Server::errorOperation()
 * @sa Server::readOperation()
 * @sa Server::writeOperation()
 **/
using ReceivedTftpRequestHandler =
  std::function< void(
    const boost::asio::ip::udp::endpoint &remote,
    RequestType requestType,
    std::string_view filename,
    Packets::TransferMode mode,
    const Packets::TftpOptions &clientOptions,
    const Packets::Options &additionalClientOptions ) >;

/**
 * @brief Operation Completed handler, which indicates, if the transfer is
 *   completed.
 *
 * @param[in] transferStatus
 *   Status of operation.
 **/
using OperationCompletedHandler = std::function< void(
  TransferStatus transferStatus ) >;

/**
 * @brief Assembles and checks given filename.
 *
 * Checks for existence of @p baseDir.
 * Assemble `basedir/filename`.
 * Check that assembled file path is within @p baseDir.
 * Optionally check for existence of assembled file path.
 *
 * @param[in] baseDir
 *   Base directory.
 *   This directory must exist.
 * @param[in] filename
 *   Filename to check.
 * @param[in] mustExist
 *   If set to true `basedir/filename` must exist.
 *   Otherwise, don't care.
 *
 * @return The processed file path
 * @retval std::nullopt
 *   When filename is not valid
 **/
[[nodiscard]] TFTP_EXPORT std::optional< std::filesystem::path > checkFilename(
  const std::filesystem::path &baseDir,
  const std::filesystem::path &filename,
  bool mustExist = false );

}

#endif
