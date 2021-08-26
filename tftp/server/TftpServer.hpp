/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Server::TftpServer.
 *
 * The class TftpServer is the main entry for a TFTP server application.
 **/

#ifndef TFTP_SERVER_TFTPSERVER_HPP
#define TFTP_SERVER_TFTPSERVER_HPP

#include <tftp/server/Server.hpp>

#include <string>
#include <functional>

namespace Tftp::Server {

/**
 * @brief TFTP %Server.
 *
 * Waits on the specified port for a valid TFTP request and calls the
 * appropriate call-back, which has to handle the request.
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
     * @param[in] tftpTimeout
     *   TFTP Timeout, when no timeout option is negotiated in seconds.
     * @param[in] tftpRetries
     *   Number of retries.
     * @param[in] serverAddress
     *   Address where the FTP server should listen on.
     *
     * @return Created TFTP Server Instance.
     **/
    static TftpServerPtr instance(
      ReceivedTftpRequestHandler handler,
      uint8_t tftpTimeout,
      uint16_t tftpRetries,
      const boost::asio::ip::udp::endpoint &serverAddress = DefaultLocalEndpoint );

    //! Destructor
    virtual ~TftpServer() noexcept = default;

    /**
     * @brief Entry of TFTP Server.
     *
     * This routines enters the I/O context loop.
     * The start routine will be left, when an FATAL error occurred or
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
     *
     * This operation does not stop TFTP Server transfers.
     * This must be handled by the caller.
     **/
    virtual void stop() = 0;

    /**
     * @brief Creates a TFTP Server Write Operation (TFTP RRQ)
     *
     * Data is obtained from @p dataHandler and transmitted to TFTP Client.
     *
     * @param[in] dataHandler
     *   Handler, which will be called on various events.
     * @param[in] completionHandler
     *   The handler which is called on completion of the operation.
     * @param[in] remote
     *   Address of the remote endpoint (TFTP Client).
     * @param[in] optionsConfiguration
     *   TFTP Options Configuration.
     *   Will be used for Options Negotiation.
     * @param[in] clientOptions
     *   TFTP Client Options.
     *   Will be negotiated within Server
     * @param[in] additionalNegotiatedOptions
     *   Additional Options, which have been already negotiated.
     *
     * @return TFTP server write operation.
     **/
    virtual OperationPtr readRequestOperation(
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const TftpOptionsConfiguration &optionsConfiguration,
      const Options &clientOptions,
      const Options &additionalNegotiatedOptions ) = 0;

    /**
     * @copydoc readRequestOperation(TransmitDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,const TftpOptionsConfiguration&,const Options&,const Options&)
     *
     * @param[in] local
     *   local endpoint, where the server handles the request from.
     **/
    virtual OperationPtr readRequestOperation(
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const TftpOptionsConfiguration &optionsConfiguration,
      const Options &clientOptions,
      const Options &additionalNegotiatedOptions,
      const boost::asio::ip::udp::endpoint &local ) = 0;

    /**
     * @brief Creates a TFTP Server Read Operation (TFTP WRQ)
     *
     * Data is received form the client and written to @p dataHandler.
     *
     * @param[in] dataHandler
     *   Handler, which will be called on various events.
     * @param[in] completionHandler
     *   Handler which is called on completion of the operation.
     * @param[in] remote
     *   Address of the remote endpoint (TFTP client).
     * @param[in] optionsConfiguration
     *   TFTP Options Configuration.
     *   Will be used for Options Negotiation.
     * @param[in] clientOptions
     *   TFTP Client Options.
     *   Will be negotiated within Server
     * @param[in] additionalNegotiatedOptions
     *   Additional Options, which have been already negotiated.
     *
     * @return TFTP server read operation.
     **/
    virtual OperationPtr writeRequestOperation(
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const TftpOptionsConfiguration &optionsConfiguration,
      const Options &clientOptions,
      const Options &additionalNegotiatedOptions ) = 0;

    /**
     * @copydoc writeRequestOperation(ReceiveDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,const TftpOptionsConfiguration&,const Options&,const Options&)
     *
     * @param[in] local
     *   local endpoint, where the server handles the request from.
     **/
    virtual OperationPtr writeRequestOperation(
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const TftpOptionsConfiguration &optionsConfiguration,
      const Options &clientOptions,
      const Options &additionalNegotiatedOptions,
      const boost::asio::ip::udp::endpoint &local ) = 0;

    /**
     * @brief Executes TFTP Error Operation.
     *
     * @param[in] remote
     *   Where the error packet shall be transmitted to.
     * @param[in] errorCode
     *   Error code of the error packet.
     * @param[in] errorMessage
     *   Error message of the packet.
     **/
    virtual void errorOperation(
      const boost::asio::ip::udp::endpoint &remote,
      ErrorCode errorCode,
      std::string_view errorMessage = {} ) = 0;

    /**
     * @copydoc errorOperation(const boost::asio::ip::udp::endpoint&,ErrorCode,std::string_view)
     *
     * @param[in] local
     *   Communication source
     **/
    virtual void errorOperation(
      const boost::asio::ip::udp::endpoint &remote,
      const boost::asio::ip::udp::endpoint &local,
      ErrorCode errorCode,
      std::string_view errorMessage = {} ) = 0;

  protected:
    //! Constructor
    TftpServer() = default;
};

}

#endif
