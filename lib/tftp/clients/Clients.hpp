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
 * @brief Declaration of Namespace Tftp::Clients.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Tftp::Clients.
 **/

#ifndef TFTP_CLIENTS_CLIENTS_HPP
#define TFTP_CLIENTS_CLIENTS_HPP

#include <tftp/Tftp.hpp>

#include <tftp/packets/Packets.hpp>

#include <functional>
#include <memory>

/**
 * @brief TFTP %Client.
 *
 * The main entry point for users of this part of the TFTP library is the
 * class @ref Client.
 * With an instance of this class the user can create TFTP client read and write
 * operations.
 *
 * @sa @ref Client
 * @sa @ref ReadOperation
 * @sa @ref WriteOperation
 **/
namespace Tftp::Clients {

// Forward declarations
class Client;
class Operation;
class ReadOperation;
class WriteOperation;

//! TFTP %Client Instance Pointer
using ClientPtr = std::shared_ptr< Client >;

//! TFTP %Client %Operation Instance Pointer
using OperationPtr = std::shared_ptr< Operation >;

//! TFTP %Client Read %Operation Instance Pointer
using ReadOperationPtr = std::shared_ptr< ReadOperation >;

//! TFTP %Client Write %Operation Instance Pointer
using WriteOperationPtr = std::shared_ptr< WriteOperation >;

/**
 * @brief TFTP %Client Option Negotiation Handler
 *
 * When the TFTP %Client operation receives the negotiated options from the TFTP
 * %Server, all TFTP specific Options (i.e. block size, transfer size, and
 * timeout) are handled there and checked for valid values.
 * Additional TFTP options are provided to this callback and must be checked
 * here.
 * The handler must remove handled options from this list.
 * The TFTP %Client %Operation will reject the option negotiation, if
 * @p serverOptions is not empty after the handler returns.
 *
 * @param[in,out] serverOptions
 *   Additional Options received from TFTP %Server.
 *   Will be negotiated by this handler and removed from this list.
 *
 * @return If the TFTP Option Negotiation was successful.
 * @retval true
 *   Additional option negotiation was successful.
 *   If @p serverOptions is empty, the TFTP operation continues
 * @retval false
 *   Additional option negotiation was failed.
 **/
using OptionNegotiationHandler = std::function<
  bool( Packets::Options &serverOptions ) >;

/**
 * @brief Operation Completed handler, which indicates, if the transfer is
 *   completed.
 *
 * @param[in] transferStatus
 *   Status of operation.
 **/
using OperationCompletedHandler = std::function< void(
  TransferStatus transferStatus ) >;

}

#endif
