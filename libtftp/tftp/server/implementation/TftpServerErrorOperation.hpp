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
 * @brief Declaration of class Tftp::Server::TftpServerErrorOperation.
 **/

#ifndef TFTP_SERVER_TFTPSERVERERROROPERATION_HPP
#define TFTP_SERVER_TFTPSERVERERROROPERATION_HPP

#include <tftp/Tftp.hpp>
#include <tftp/server/implementation/TftpServerBaseErrorOperation.hpp>

#include <string>

namespace Tftp {
namespace Server {

/**
 * @brief This operation can be used to transfer an error message back to
 *  the initiator of an TFTP request.
 **/
class TftpServerErrorOperation: public TftpServerBaseErrorOperation
{
  public:
    using string = std::string;

    /**
     * @brief Initialises the error operation.
     *
     * @param[in] clientAddress
     *   Where the error packet shall be transmitted to.
     * @param[in] errorCode
     *   The error code of the error packet.
     * @param[in] errorMessage
     *   The error message of the packet.
     * @param[in] from
     *   Optional parameter to define the communication source
     *
     * @throw CommunicationException
     **/
    TftpServerErrorOperation(
      const UdpAddressType &clientAddress,
      const UdpAddressType &from,
      ErrorCode errorCode,
      const string &errorMessage);

    /**
     * @brief Initialises the error operation.
     *
     * @param[in] clientAddress
     *   Where the error packet shall be transmitted to.
     * @param[in] errorCode
     *   The error code of the error packet.
     * @param[in] errorMessage
     *   The error message of the packet.
     *
     * @throw CommunicationException
     **/
    TftpServerErrorOperation(
      const UdpAddressType &clientAddress,
      ErrorCode errorCode,
      const string &errorMessage);

    /**
     * @brief Executes the error operation
     **/
    void operator()();

  private:
    //! The error code
    const ErrorCode errorCode;
    //! The error message
    const string errorMessage;
};

}
}

#endif
