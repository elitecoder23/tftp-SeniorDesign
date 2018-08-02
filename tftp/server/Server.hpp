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

#include <boost/asio/ip/udp.hpp>

#include <memory>
#include <functional>

/**
 * @brief TFTP server implementation.
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

//! Declaration of TFTP server operation instance pointer
using TftpServerPtr = std::shared_ptr< TftpServer>;

class Operation;
//! Operation instance pointer
using OperationPtr = std::shared_ptr< Operation>;

/**
 * @brief Function handler definition.
 *
 * * requestType
 * * filename
 * * mode
 * * options
 * * remote
 **/
using ReceivedTftpRequestHandler =
  std::function< void (
    RequestType,
    const std::string&,
    TransferMode,
    const Options::OptionList&,
    const boost::asio::ip::udp::endpoint&)>;

}

#endif
