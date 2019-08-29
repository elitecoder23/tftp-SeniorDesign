/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Vlass Tftp::Server::TftpServer.
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
 * @brief Public Interface of TFTP Servers.
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
     * @brief Creates a TFTP Server Instance.
     *
     * @param[in] handler
     *   The request handler.
     * @param[in] configuration
     *   The TFTP Configuration
     * @param[in] serverAddress
     *   Address where the FTP server should listen on.
     *
     * @return The created TFTP server instance.
     **/
    static TftpServerPtr instance(
      ReceivedTftpRequestHandler handler,
      const TftpConfiguration &configuration = {},
      const boost::asio::ip::udp::endpoint &serverAddress = DefaultLocalEndpoint);

    //! Destructor
    virtual ~TftpServer() noexcept = default;

    /**
     * @brief Entry of TFTP Server.
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
     *   Received TFTP options from client.
     * @param[in] serverOptions
     *   Server TFTP options used for option negotiation.
     *
     * @return The TFTP server write operation.
     **/
    virtual OperationPtr readRequestOperation(
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const Options::Options &clientOptions,
      const Options::OptionList& serverOptions) = 0;

    /**
     * @copydoc readRequestOperation(TransmitDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,const Options::Options&,const Options::OptionList&)
     *
     * @param[in] local
     *   local endpoint, where the server handles the request from.
     **/
    virtual OperationPtr readRequestOperation(
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const Options::Options &clientOptions,
      const Options::OptionList& serverOptions,
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
     *   Received TFTP options from client.
     * @param[in] serverOptions
     *   Server TFTP options used for option negotiation.
     *
     * @return The TFTP server read operation.
     **/
    virtual OperationPtr writeRequestOperation(
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const Options::Options &clientOptions,
      const Options::OptionList& serverOptions) = 0;

    /**
     * @copydoc writeRequestOperation(ReceiveDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,const Options::Options&,const Options::OptionList&)
     *
     * @param[in] local
     *   local endpoint, where the server handles the request from.
     **/
    virtual OperationPtr writeRequestOperation(
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const Options::Options &clientOptions,
      const Options::OptionList& serverOptions,
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
      std::string_view errorMessage = {}) = 0;

  protected:
    //! Constructor
    TftpServer() = default;
};

}

#endif
