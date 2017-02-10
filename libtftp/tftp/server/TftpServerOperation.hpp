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
 * @brief Declaration of class Tftp::Server::TftpServerOperation.
 **/

#ifndef TFTP_SERVER_TFTPSERVEROPERATION_HPP
#define TFTP_SERVER_TFTPSERVEROPERATION_HPP

#include <tftp/server/Server.hpp>

#include <string>

namespace Tftp {
namespace Server {

/**
 * @brief base class for TFTP server operations.
 *
 * This class is specialised for the two kinds of TFTP operations
 * (Read Operation, Write Operation).
 **/
class TftpServerOperation
{
  public:
    using string = std::string;

    //! Default Constructor
    virtual ~TftpServerOperation() noexcept = default;

    /**
     * @brief Executes the TFTP client operation.
     *
     * This routines starts the client communication loop.
     **/
    virtual void operator()() = 0;

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
      const string &errorMessage = string()) = 0;

    /**
     * @brief Aborts the operation immediately.
     *
     * No error message is sent.
     **/
    virtual void abort() = 0;

};
}
}

#endif
