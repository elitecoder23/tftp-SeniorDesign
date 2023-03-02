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
 * The handler removes handled options from this list.
 * The Operation will reject option negotiation, if @p serverOptions is not
 * empty after handler call.
 *
 * @param[in,out] operation
 *   Operation which informs the callback.
 * @param[in,out] serverOptions
 *   Received Options from TFTP Server
 *
 * @return If the TFTP Option Negotiation was successful.
 **/
using OptionNegotiationHandler = std::function<
  bool( const OperationPtr &operation, Packets::Options &serverOptions ) >;

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
