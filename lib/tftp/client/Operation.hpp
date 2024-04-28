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
 * @brief Declaration of Class Tftp::Client::Operation.
 **/

#ifndef TFTP_CLIENT_OPERATION_HPP
#define TFTP_CLIENT_OPERATION_HPP

#include "tftp/client/Client.hpp"

#include "tftp/packets/Packets.hpp"

#include <string>
#include <optional>

namespace Tftp::Client {

/**
 * @brief TFTP %Client %Operation.
 *
 * This class is specialised for the two kinds of TFTP operations
 * (Read Operation, Write Operation).
 **/
class TFTP_EXPORT Operation
{
  public:
    //! Error Information
    using ErrorInfo = std::optional< Packets::ErrorPacket >;

    //! Destructor.
    virtual ~Operation() = default;

    /**
     * @brief Executes the TFTP Client Operation.
     *
     * It prepares the FTP Request Packet, sends it to the remote endpoint and
     * start the reception loop.
     *
     * It returns immediately after sending the request.
     **/
    virtual void request() = 0;

    /**
     * @brief Aborts the Operation Gracefully.
     *
     * Sends an error packet at next possible time point.
     *
     * @param[in] errorCode
     *   The TFTP error code.
     * @param[in] errorMessage
     *   An additional error message.
     **/
    virtual void gracefulAbort(
      Packets::ErrorCode errorCode,
      std::string errorMessage = {} ) = 0;

    /**
     * @brief Immediately Cancels the Transfer.
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
