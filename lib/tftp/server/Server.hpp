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
 * @brief Declaration of Namespace Tftp::Server.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Tftp::Server.
 **/

#ifndef TFTP_SERVER_SERVER_HPP
#define TFTP_SERVER_SERVER_HPP

#include <tftp/Tftp.hpp>

#include <tftp/packets/Packets.hpp>

#include <boost/asio/ip/udp.hpp>

#include <memory>
#include <functional>
#include <optional>
#include <string_view>

/**
 * @brief TFTP %Server.
 *
 * The user of this library uses following interface classes to interact with
 * it:
 * - @ref TftpServer
 *   The main entry point (is also a factory for all other classes).
 * - @ref ReceivedTftpRequestHandler
 *   This interface must be implemented by the user of this library to handle
 *   new TFTP requests.
 * - @ref Operation
 *   Class interface to execute the operations.
 *   The actual operations can be created by utilising the TftpServer class
 *   instances.
 * - @ref ReceiveDataHandler and @ref TransmitDataHandler
 *   This interface class must be implemented by the user of this library to
 *   make use of the TFTP server operations.
 **/
namespace Tftp::Server {

// Forward declarations
class TftpServer;

struct ReadOperationConfiguration;
struct WriteOperationConfiguration;

//! TFTP Server Instance Pointer
using TftpServerPtr = std::shared_ptr< TftpServer >;

class Operation;
//! TFTP Server Operation Instance Pointer
using OperationPtr = std::shared_ptr< Operation >;

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
 * @sa TftpServer::errorOperation()
 * @sa TftpServer::readOperation()
 * @sa TftpServer::writeOperation()
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
 * @param[in,out] operation
 *   Operation which informs the callback.
 * @param[in] transferStatus
 *   Status of operation.
 **/
using OperationCompletedHandler = std::function< void(
  const OperationPtr &operation,
  TransferStatus transferStatus ) >;

}

#endif
