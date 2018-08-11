/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::Server::TftpServer.
 *
 * The class TftpServer is the main entry for a TFTP server application.
 **/

#ifndef TFTP_SERVER_TFTPSERVER_HPP
#define TFTP_SERVER_TFTPSERVER_HPP

#include <tftp/server/Server.hpp>
#include <tftp/TftpConfiguration.hpp>
#include <tftp/options/Options.hpp>

#include <string>

namespace Tftp::Server {

/**
 * @brief Public interface of TFTP servers.
 *
 * Waits on the specified port for a valid TFTP request and calls the
 * approbate call-back, which has to handle the request.
 *
 * If not expected packets or invalid packets are received a error is send
 * back to the sender.
 *
 * Valid requests are TFTP Read Request (RRQ) and TFTP Write Request (WRQ)
 **/
class TftpServer
{
  public:
    /**
     * @brief The default address, where the server listens.
     *
     * The default local end-point where the TFTP server is listening is the
     * default TFTP Port any IP address.
     **/
    static const boost::asio::ip::udp::endpoint DefaultLocalEndpoint;

    /**
     * @brief Creates an instance of the TFTP server.
     *
     * @param[in] handler
     *   The request handler.
     * @param[in] configuration
     *   The TFTP Configuration
     * @param[in] additionalOptions
     *   Additional Options, which shall be used as TFTP server option list.
     * @param[in] serverAddress
     *   Address where the FTP server should listen on.
     *
     * @return The created TFTP server instance.
     **/
    static TftpServerPtr instance(
      ReceivedTftpRequestHandler handler,
      const TftpConfiguration &configuration = {},
      const Options::OptionList& additionalOptions = {},
      const boost::asio::ip::udp::endpoint &serverAddress = DefaultLocalEndpoint);

    //! Default destructor
    virtual ~TftpServer() noexcept = default;

    /**
     * @brief Entry of the TFTP Server.
     *
     * This routines enters the I/O context loop.
     * The start routine will be leaved, when an FATAL error occurred or
     * the server has been stopped by calling stop().
     *
     * This entry can be called multiple time to allow parallel transfer handling
     **/
    virtual void entry() noexcept = 0;

    /**
     * @brief Starts the TFTP Server.
     *
     * This routines starts the server receive operation.
     * This routine returns immediately.
     *
     * It can be called before entry() is called.
     **/
    virtual void start() = 0;

    /**
     * @brief Stops the TFTP Server.
     **/
    virtual void stop() = 0;

    /**
     * @brief Creates a TFTP write operation (TFTP RRQ)
     *
     * @param[in] dataHandler
     *   Handler, which will be called on various events.
     * @param[in] completionHandler
     *   The handler which is called on completion of the operation.
     * @param[in] remote
     *   Address of the remote endpoint (TFTP client).
     * @param[in] clientOptions
     *   Received option list from client.
     * @param[in] local
     *   local endpoint, where the server handles the request from.
     *
     * @return The TFTP server write operation.
     **/
    virtual OperationPtr readRequestOperation(
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const Options::OptionList &clientOptions,
      const boost::asio::ip::udp::endpoint &local) = 0;

    /**
     * @brief Creates a TFTP read operation (TFTP WRQ)
     *
     * @param[in] dataHandler
     *   Handler, which will be called on various events.
     * @param[in] completionHandler
     *   The handler which is called on completion of the operation.
     * @param[in] remote
     *   Address of the remote endpoint (TFTP client).
     * @param[in] clientOptions
     *   Received option list from client.
     * @param[in] local
     *   local endpoint, where the server handles the request from.
     *
     * @return The TFTP server read operation.
     **/
    virtual OperationPtr writeRequestOperation(
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const Options::OptionList &clientOptions,
      const boost::asio::ip::udp::endpoint &local) = 0;

    /**
     * @brief Creates a TFTP error operation.
     *
     * @param[in] completionHandler
     *   The handler which is called on completion of the operation.
     * @param[in] remote
     *   Where the error packet shall be transmitted to.
     * @param[in] local
     *   Optional parameter to define the communication source
     * @param[in] errorCode
     *   The error code of the error packet.
     * @param[in] errorMessage
     *   The error message of the packet.
     *
     * @return The created TFTP Error operation.
     **/
    virtual OperationPtr errorOperation(
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const boost::asio::ip::udp::endpoint &local,
      ErrorCode errorCode,
      const std::string &errorMessage) = 0;

    //! @copydoc errorOperation(OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,const boost::asio::ip::udp::endpoint&,ErrorCode,const std::string&)
    virtual OperationPtr errorOperation(
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const boost::asio::ip::udp::endpoint &local,
      ErrorCode errorCode,
      std::string &&errorMessage = {}) = 0;

  protected:
    //! Protected constructor
    TftpServer() = default;
};

}

#endif
