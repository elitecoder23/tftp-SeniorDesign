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
 * @brief Declaration of namespace Tftp::Server.
 **/

#ifndef TFTP_SERVER_SERVER_HPP
#define TFTP_SERVER_SERVER_HPP

#include <tftp/Tftp.hpp>
#include <tftp/options/Options.hpp>

#include <memory>
#include <functional>

namespace Tftp {
/**
 * @brief Implementation of TFTP servers.
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
namespace Server {

// Forward declarations
class NewRequestHandler;
class TftpServer;

//! Declaration of TFTP server operation instance pointer
using TftpServerPtr = std::shared_ptr< TftpServer>;

//! Declaration of TFTP server operation
using TftpServerOperation = std::function< void( void)>;

/**
 * @brief Prototype for new TFTP requests handler.
 *
 * @param[in] requestType
 *   The kind of request.
 * @param[in] from
 *   Where the request comes from (the TFTP client).
 * @param[in] filename
 *   Filename of file to read.
 * @param[in] mode
 *   Transfer mode.
 * @param[in] options
 *   List of received TFTP options.
 **/
typedef void (ReceivedTftpRequestHandlerType)(
  TftpRequestType requestType,
  const UdpAddressType &from,
  const std::string &filename,
  TransferMode mode,
  const Options::OptionList &options);

//! Function handler definition.
using ReceivedTftpRequestHandler = std::function< ReceivedTftpRequestHandlerType>;

}
}

#endif
