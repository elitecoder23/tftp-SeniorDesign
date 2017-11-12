/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::Server::Operation.
 **/

#ifndef TFTP_SERVER_OPERATION_HPP
#define TFTP_SERVER_OPERATION_HPP

#include <tftp/server/Server.hpp>
#include <tftp/packets/Packets.hpp>

#include <boost/optional.hpp>

#include <string>

namespace Tftp {
namespace Server {

/**
 * @brief base class for TFTP server operations.
 *
 * This class is specialised for the two kinds of TFTP operations
 * (Read Operation, Write Operation).
 **/
class Operation
{
  public:
    //! Error info type
    using ErrorInfo = boost::optional< Packets::ErrorPacket>;
    //! string type
    using string = std::string;

    //! Default Constructor
    virtual ~Operation() noexcept = default;

    /**
     * @brief Executes the TFTP client operation.
     *
     * This routines starts the client communication loop.
     **/
    virtual void start() = 0;

    /**
     * @brief Aborts the operation gracefully.
     *
     * @param[in] errorCode
     *   Abort error code.
     * @param errorMessage
     *   Abort error message.
     **/
    virtual void gracefulAbort(
      ErrorCode errorCode,
      const string &errorMessage = {}) = 0;

    /**
     * @brief Aborts the operation immediately.
     *
     * No error message is sent.
     **/
    virtual void abort() = 0;

    /**
     * @brief Returns the error info of this operation
     *
     * @return The error info of this operation
     * @retval ErrorInfo()
     *   When operation has no error.
     **/
    virtual const ErrorInfo& getErrorInfo() const = 0;

};
}
}

#endif
