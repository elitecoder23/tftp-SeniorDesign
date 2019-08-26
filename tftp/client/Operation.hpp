/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Client::Operation.
 **/

#ifndef TFTP_CLIENT_OPERATION_HPP
#define TFTP_CLIENT_OPERATION_HPP

#include <tftp/client/Client.hpp>
#include <tftp/packets/Packets.hpp>

#include <string>
#include <optional>

namespace Tftp::Client {

/**
 * @brief base class for TFTP client operations.
 *
 * This class is specialised for the two kinds of TFTP operations
 * (Read Operation, Write Operation).
 **/
class Operation
{
  public:
    //! Error Information
    using ErrorInfo = std::optional< Packets::ErrorPacket>;

    //! Destructor.
    virtual ~Operation() noexcept = default;

    /**
     * @brief Executes the TFTP client operation.
     *
     * This routines starts the client communication.
     *
     * It returns immediately after sending the request.
     **/
    virtual void start() = 0;

    /**
     * @brief Aborts the operation gracefully.
     *
     * Sends an error packet at next possible time point.
     *
     * @param[in] errorCode
     *   The TFTP error code.
     * @param[in] errorMessage
     *   An additional error message.
     **/
    virtual void gracefulAbort(
      ErrorCode errorCode,
      std::string_view errorMessage = {}) = 0;

    /**
     * @brief Immediately cancels the transfer.
     **/
    virtual void abort() = 0;

    /**
     * @brief Returns the error information.
     *
     * @return The error information
     * @retval ErrorInfo()
     *   If no error occurred.
     **/
    [[nodiscard]] virtual const ErrorInfo& errorInfo() const = 0;
};

}

#endif
