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
 * @brief Declaration of class Tftp::Server::ErrorOperation.
 **/

#ifndef TFTP_SERVER_ERROROPERATION_HPP
#define TFTP_SERVER_ERROROPERATION_HPP

#include <tftp/Tftp.hpp>
#include <tftp/server/implementation/BaseErrorOperation.hpp>

#include <string>

namespace Tftp {
namespace Server {

/**
 * @brief This operation can be used to transfer an error message back to
 *  the initiator of an TFTP request.
 **/
class ErrorOperation: public BaseErrorOperation
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
     **/
    ErrorOperation(
      const UdpAddressType &clientAddress,
      const UdpAddressType &from,
      ErrorCode errorCode,
      const string &errorMessage);

    ErrorOperation(
      UdpAddressType &&clientAddress,
      UdpAddressType &&from,
      ErrorCode errorCode,
      string &&errorMessage);

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
    ErrorOperation(
      const UdpAddressType &clientAddress,
      ErrorCode errorCode,
      const string &errorMessage);

    ErrorOperation(
      UdpAddressType &&clientAddress,
      ErrorCode errorCode,
      string &&errorMessage);

    /**
     * @copydoc Operation::operator()()
     *
     * Executes the error operation
     **/
    virtual void operator()() override final;

  private:
    //! The error code
    const ErrorCode errorCode;
    //! The error message
    const string errorMessage;
};

}
}

#endif
