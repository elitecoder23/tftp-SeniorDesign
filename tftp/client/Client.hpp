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

//! TFTP %Client Instance Pointer
using TftpClientPtr = std::shared_ptr< TftpClient >;

//! TFTP %Client %Operation Instance Pointer
using OperationPtr = std::shared_ptr< Operation >;

/**
 * @brief TFTP Client Option Negotiation Handler
 *
 * @param[in] serverOptions
 *   Received Options from TFTP Server
 *
 * @return If the TFTP Option Negotiation was successful.
 **/
using OptionNegotiationHandler = std::function<
  bool( const Packets::Options &serverOptions ) >;

}

#endif
