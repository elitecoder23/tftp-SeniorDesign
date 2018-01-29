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
    /**
     * @brief Initialises the error operation.
     *
     * @param[in] ioService
     *   The IO service used for communication.
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
     **/
    ErrorOperation(
      boost::asio::io_service &ioService,
      OperationCompletedHandler completionHandler,
      const UdpAddressType &remote,
      const UdpAddressType &local,
      ErrorCode errorCode,
      const std::string &errorMessage);

    //! @copydoc ErrorOperation(boost::asio::io_service&,OperationCompletedHandler,const UdpAddressType&,const UdpAddressType&,ErrorCode,const std::string&)
    ErrorOperation(
      boost::asio::io_service &ioService,
      OperationCompletedHandler completionHandler,
      UdpAddressType &&remote,
      UdpAddressType &&local,
      ErrorCode errorCode,
      std::string &&errorMessage);

    //! Default destructor.
    virtual ~ErrorOperation() noexcept;

    /**
     * @copydoc Operation::start()
     *
     * Executes the error operation
     **/
    void start() final;

    //! @copydoc Operation::gracefulAbort
    void gracefulAbort(
      ErrorCode errorCode,
      std::string &&errorMessage = {}) final;

    //! @copydoc Operation::abort
    void abort() final;

    //! @copydoc Operation::errorInfo
    const ErrorInfo& errorInfo() const final;

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
    const ErrorInfo errorInfoV;
};

}
}

#endif
