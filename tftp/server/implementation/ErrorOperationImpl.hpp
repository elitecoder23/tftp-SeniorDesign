/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Server::ErrorOperation.
 **/

#ifndef TFTP_SERVER_ERROROPERATION_HPP
#define TFTP_SERVER_ERROROPERATION_HPP

#include <tftp/Tftp.hpp>

#include <tftp/server/Server.hpp>
#include <tftp/server/Operation.hpp>

#include <tftp/packets/ErrorPacket.hpp>

#include <boost/asio.hpp>

#include <string>

namespace Tftp::Server {

/**
 * @brief This operation can be used to transfer an error message back to
 *  the initiator of an TFTP request.
 **/
class ErrorOperationImpl
{
  public:
    /**
     * @brief Initialises the error operation.
     *
     * @param[in] ioContext
     *   The I/O context used for communication.
     * @param[in] remote
     *   Where the error packet shall be transmitted to.
     * @param[in] errorCode
     *   The error code of the error packet.
     * @param[in] errorMessage
     *   The error message of the packet.
     *
     * @throw CommunicationException
     *   On system_error
     **/
    ErrorOperationImpl(
      boost::asio::io_context &ioContext,
      const boost::asio::ip::udp::endpoint &remote,
      ErrorCode errorCode,
      std::string_view errorMessage);

    /**
     * @brief Initialises the error operation.
     *
     * @param[in] ioContext
     *   The I/O context used for communication.
     * @param[in] remote
     *   Where the error packet shall be transmitted to.
     * @param[in] local
     *   Optional parameter to define the communication source
     * @param[in] errorCode
     *   The error code of the error packet.
     * @param[in] errorMessage
     *   The error message of the packet.
     *
     * @throw CommunicationException
     *   On system_error
     **/
    ErrorOperationImpl(
      boost::asio::io_context &ioContext,
      const boost::asio::ip::udp::endpoint &remote,
      const boost::asio::ip::udp::endpoint &local,
      ErrorCode errorCode,
      std::string_view errorMessage);

    //! Destructor.
    ~ErrorOperationImpl() noexcept = default;

    /**
     * @brief Executes error operation
     *
     * send error packet
     **/
    void operator()();

  private:
    //! Communication socket
    boost::asio::ip::udp::socket socket;
    //! Error code
    ErrorCode errorCode;
    //! Error message
    std::string_view errorMessage;
};

}

#endif
