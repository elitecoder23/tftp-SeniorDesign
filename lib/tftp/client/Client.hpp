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
 * @brief Declaration of Namespace Tftp::Client.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Tftp::Client.
 **/

#ifndef TFTP_CLIENT_CLIENT_HPP
#define TFTP_CLIENT_CLIENT_HPP

#include <tftp/Tftp.hpp>

#include <tftp/packets/Packets.hpp>

#include <memory>
#include <functional>
#include <optional>

/**
 * @brief TFTP %Client.
 *
 * The main entry point for users of this part of the TFTP library is the
 * class TftpClient.
 * With an instance of this class the user can create TFTP client read and write
 * operations.
 **/
namespace Tftp::Client {

class TftpClient;
class Operation;

struct ReadOperationConfiguration;
struct WriteOperationConfiguration;

//! TFTP %Client Instance Pointer
using TftpClientPtr = std::shared_ptr< TftpClient >;

//! TFTP %Client %Operation Instance Pointer
using OperationPtr = std::shared_ptr< Operation >;

/**
 * @brief TFTP Client Option Negotiation Handler
 *
 * When the TFTP Client operation receives the negotiated options from the TFTP
 * Server, all TFTP specific Options (i.e. block size, transfer size, and
 * timeout) are handled there and checked for valid values.
 * Additional TFTP options are provided to this callback and must be checked
 * here.
 * The handler must remove handled options from this list.
 * The TFTP Client Operation will reject the option negotiation, if
 * @p serverOptions is not empty after the handler returns.
 *
 * @param[in,out] operation
 *   Operation which informs the callback.
 * @param[in,out] serverOptions
 *   Additional Options received from TFTP Server.
 *   WIll be negotiated by this handler and removed from this list
 *
 * @return If the TFTP Option Negotiation was successful.
 * @retval true
 *   Additional option negotiation was successful.
 *   If @p serverOptions is empty, the TFTP operation continues
 * @retval false
 *   Additional option negotiation was failed.
 **/
using OptionNegotiationHandler = std::function<
  bool( const OperationPtr &operation, Packets::Options &serverOptions ) >;

/**
 * @brief Handler which indicates, if the TFTP transfer is completed.
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
