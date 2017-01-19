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
 * @brief Declaration of class Tftp::Client::TftpClientOperation.
 **/

#ifndef TFTP_CLIENT_TFTPCLIENTOPERATION_HPP
#define TFTP_CLIENT_TFTPCLIENTOPERATION_HPP

#include <tftp/client/Client.hpp>

#include <string>

namespace Tftp {
namespace Client {

/**
 * @brief base class for TFTP client operations.
 *
 * This class is specialised for the two kinds of TFTP operations
 * (Read Operation, Write Operation).
 **/
class TftpClientOperation
{
  public:
    using string = std::string;

    //! Default destructor.
    virtual ~TftpClientOperation() noexcept = default;

    /**
     * @brief Executes the TFTP client operation.
     *
     * This routines starts the client communication loop.
     **/
    virtual void operator()() = 0;

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

    virtual RequestType getRequestType() const = 0;

    virtual const UdpAddressType& getServerAddress() const = 0;

    /**
     * @brief Returns the request filename.
     *
     * @return The request filename.
     **/
    virtual const string& getFilename() const = 0;

    /**
     * @brief Returns the transfer mode.
     *
     * @return The transfer mode.
     **/
    virtual TransferMode getMode() const = 0;
};

}
}

#endif
