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
class ErrorOperation: public Operation
{
  public:
    /**
     * @brief Initialises the error operation.
     *
     * @param[in] ioContext
     *   The I/O context used for communication.
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
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
    ErrorOperation(
      boost::asio::io_context &ioContext,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const boost::asio::ip::udp::endpoint &local,
      ErrorCode errorCode,
      std::string_view errorMessage);

    //! Destructor.
    ~ErrorOperation() noexcept final = default;

    /**
     * @copydoc Operation::start()
     *
     * Executes the error operation
     **/
    void start() final;

    //! @copydoc Operation::gracefulAbort
    void gracefulAbort(
      ErrorCode errorCode,
      std::string_view errorMessage = {}) final;

    //! @copydoc Operation::abort
    void abort() final;

    //! @copydoc Operation::errorInfo
    [[nodiscard]] const ErrorInfo& errorInfo() const final;

  private:
    /**
     * @brief Sends the given error packet.
     *
     * @param[in] error
     *   The error packet.
     **/
    void sendError( const Packets::ErrorPacket &error);

    //! Completion handler
    OperationCompletedHandler completionHandler;
    //! Communication socket
    boost::asio::ip::udp::socket socket;
    //! Error code
    const ErrorInfo errorInfoV;
};

}

#endif
