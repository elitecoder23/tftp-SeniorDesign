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
#include <tftp/options/Options.hpp>

#include <boost/asio/ip/udp.hpp>

#include <memory>
#include <functional>

/**
 * @brief TFTP Server Implementation.
 *
 * The user of this library uses following interface classes to interact with
 * it:
 * @li \c TftpServer - The main entry point (is also an factory for all other
 *   classes,
 * @li \c NewRequestHandler - This interface must be implemented by the user
 *   of this library to handle new TFTP requests.
 * @li \c Operation - Class interface to execute the operations. The actual
 *   operations can be created by utilising the TftpServer class instances.
 * @li \c TftpReadOperationHandler and \c TftpWriteOperationHandler - This
 *   interface class must be implemented by the user of this library to make
 *   use of the TFTP server operations.
 **/
namespace Tftp::Server {

// Forward declarations
class TftpServer;

//! TFTP Server Instance Pointer
using TftpServerPtr = std::shared_ptr< TftpServer>;

class Operation;
//! TFTP Server Operation Instance Pointer
using OperationPtr = std::shared_ptr< Operation>;

/**
 * @brief Function Handler Definition.
 *
 * @param[in] remote
 *   Remote Endpoint
 * @param[in] requestType
 *   TFTP Request Type
 * @param[in] filename
 *   Filename
 * @param[in] mode
 *   Transfer Mode
 * @param[in] options
 *   TFTP Options
 **/
using ReceivedTftpRequestHandler =
  std::function< void(
    const boost::asio::ip::udp::endpoint &remote,
    RequestType requestType,
    std::string_view filename,
    TransferMode mode,
    const Options::OptionList &options)>;

}

#endif
