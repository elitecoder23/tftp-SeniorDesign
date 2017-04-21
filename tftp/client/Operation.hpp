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
 * @brief Declaration of class Tftp::Client::Operation.
 **/

#ifndef TFTP_CLIENT_OPERATION_HPP
#define TFTP_CLIENT_OPERATION_HPP

#include <tftp/client/Client.hpp>
#include <tftp/packets/Packets.hpp>

#include <boost/optional.hpp>

#include <string>

namespace Tftp {
namespace Client {

/**
 * @brief base class for TFTP client operations.
 *
 * This class is specialised for the two kinds of TFTP operations
 * (Read Operation, Write Operation).
 **/
class Operation
{
  public:
    using ErrorInfo = boost::optional< Packets::ErrorPacket>;
    using string = std::string;

    //! Default destructor.
    virtual ~Operation() noexcept = default;

    /**
     * @brief Executes the TFTP client operation.
     *
     * This routines starts the client communication.
     *
     * It returns immediately after sending the request.
     *
     **/
    virtual void start() = 0;

    /**
     * @brief Aborts the operation gracefully.
     *
     * Sends an error packet at next possible time point.
     *
     * @param errorCode
     * @param errorMessage
     **/
    virtual void gracefulAbort(
      ErrorCode errorCode,
      const string &errorMessage = string()) = 0;

    /**
     * @brief Immediately cancels the transfer.
     **/
    virtual void abort() = 0;

    virtual const ErrorInfo& getErrorInfo() const = 0;
};

}
}

#endif
