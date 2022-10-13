/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Server::Operation.
 **/

#ifndef TFTP_SERVER_OPERATION_HPP
#define TFTP_SERVER_OPERATION_HPP

#include <tftp/server/Server.hpp>
#include <tftp/packets/Packets.hpp>

#include <optional>
#include <string_view>

namespace Tftp::Server {

/**
 * @brief TFTP %Server %Operation.
 *
 * This class is specialised for the two kinds of TFTP operations
 * (Read Operation, Write Operation).
 **/
class Operation
{
  public:
    //! Error Info Type
    using ErrorInfo = std::optional< Packets::ErrorPacket >;

    //! Constructor
    virtual ~Operation() noexcept = default;

    /**
     * @brief Executes the TFTP Server Operation.
     *
     * Sends response to request and waits for answers.
     **/
    virtual void start() = 0;

    /**
     * @brief Aborts the Operation Gracefully.
     *
     * With an graceful abort the transfer is canceled by sending an error
     * packet to the client, signalling the abort of the transfer.
     *
     * @param[in] errorCode
     *   Abort error code.
     * @param errorMessage
     *   Abort error message.
     **/
    virtual void gracefulAbort(
      Packets::ErrorCode errorCode,
      std::string errorMessage = {} ) = 0;

    /**
     * @brief Aborts the Operation Immediately.
     *
     * No error message is sent.
     **/
    virtual void abort() = 0;

    /**
     * @brief Returns the Error Information of this Operation
     *
     * @return The error info of this operation
     * @retval ErrorInfo()
     *   When operation has no error.
     **/
    [[nodiscard]] virtual const ErrorInfo& errorInfo() const = 0;
};

}

#endif
