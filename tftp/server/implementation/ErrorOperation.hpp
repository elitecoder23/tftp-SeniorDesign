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

#include <tftp/server/Server.hpp>
#include <tftp/server/Operation.hpp>

#include <tftp/packets/ErrorPacket.hpp>

#include <boost/asio.hpp>

#include <string>

namespace Tftp {
namespace Server {

/**
 * @brief This operation can be used to transfer an error message back to
 *  the initiator of an TFTP request.
 **/
class ErrorOperation: public Operation
{
  public:
    //! string type
    using string = std::string;

    /**
     * @brief Initialises the error operation.
     *
     * @param[in] ioService
     *   The IO service used for communication.
     * @param[in] clientAddress
     *   Where the error packet shall be transmitted to.
     * @param[in] errorCode
     *   The error code of the error packet.
     * @param[in] errorMessage
     *   The error message of the packet.
     * @param[in] from
     *   Optional parameter to define the communication source
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
     **/
    ErrorOperation(
      boost::asio::io_service &ioService,
      const UdpAddressType &clientAddress,
      const UdpAddressType &from,
      ErrorCode errorCode,
      const string &errorMessage,
      OperationCompletedHandler completionHandler);

    /**
     * @brief Initialises the error operation.
     *
     * @param[in] ioService
     *   The IO service used for communication.
     * @param[in] clientAddress
     *   Where the error packet shall be transmitted to.
     * @param[in] errorCode
     *   The error code of the error packet.
     * @param[in] errorMessage
     *   The error message of the packet.
     * @param[in] from
     *   Optional parameter to define the communication source
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
     **/
    ErrorOperation(
      boost::asio::io_service &ioService,
      UdpAddressType &&clientAddress,
      UdpAddressType &&from,
      ErrorCode errorCode,
      string &&errorMessage,
      OperationCompletedHandler completionHandler);

    /**
     * @brief Initialises the error operation.
     *
     * @param[in] ioService
     *   The IO service used for communication.
     * @param[in] clientAddress
     *   Where the error packet shall be transmitted to.
     * @param[in] errorCode
     *   The error code of the error packet.
     * @param[in] errorMessage
     *   The error message of the packet.
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
     **/
    ErrorOperation(
      boost::asio::io_service &ioService,
      const UdpAddressType &clientAddress,
      ErrorCode errorCode,
      const string &errorMessage,
      OperationCompletedHandler completionHandler);

    /**
     * @brief Initialises the error operation.
     *
     * @param[in] ioService
     *   The IO service used for communication.
     * @param[in] clientAddress
     *   Where the error packet shall be transmitted to.
     * @param[in] errorCode
     *   The error code of the error packet.
     * @param[in] errorMessage
     *   The error message of the packet.
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
     **/
    ErrorOperation(
      boost::asio::io_service &ioService,
      UdpAddressType &&clientAddress,
      ErrorCode errorCode,
      string &&errorMessage,
      OperationCompletedHandler completionHandler);

    //! Default destructor.
    virtual ~ErrorOperation() noexcept;

    /**
     * @copydoc Operation::start()
     *
     * Executes the error operation
     **/
    virtual void start() override final;

    //! @copydoc Operation::gracefulAbort
    virtual void gracefulAbort(
      ErrorCode errorCode,
      const string &errorMessage = string()) override final;

    //! @copydoc Operation::abort
    virtual void abort() override final;

    //! @copydoc Operation::getErrorInfo
    virtual const ErrorInfo& getErrorInfo() const override final;

  private:
    /**
     * @brief Sends the given error packet.
     *
     * @param[in] error
     *   The error packet.
     **/
    void sendError( const Packets::ErrorPacket &error);

    //! The completion handler
    OperationCompletedHandler completionHandler;
    //! The communication socket
    boost::asio::ip::udp::socket socket;
    //! The error code
    const ErrorInfo errorInfo;
};

}
}

#endif
