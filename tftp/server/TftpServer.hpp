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

#include <tftp/TftpOptionsConfiguration.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

#include <string>
#include <functional>
#include <optional>
#include <chrono>

namespace Tftp::Server {

/**
 * @brief TFTP %Server.
 *
 * Waits on the specified port for a valid TFTP request and calls the
 * appropriate call-back, which has to handle the request.
 *
 * If not expected packets or invalid packets are received an error is sent
 * back to the sender.
 *
 * Valid requests are TFTP Read Request (RRQ) and TFTP Write Request (WRQ)
 **/
class TftpServer
{
  public:
    //! TFTP Server Configuration
    struct ServerConfiguration
    {
      //! TFTP Timeout, when no timeout option is negotiated in seconds.
      std::chrono::seconds tftpTimeout;
      //! Number of retries.
      uint16_t tftpRetries;
      //! If set to true, wait after transmission of the final ACK for potential
      //! retries.
      //! Used by TFTP WRQ Operation
      bool dally;
      //! TFTP Request Received Handler
      ReceivedTftpRequestHandler handler;
      //! Address where the TFTP server should listen on.
      std::optional< boost::asio::ip::udp::endpoint > serverAddress;
    };

    //! TFTP Server Read Operation Configuration
    struct ReadOperationConfiguration
    {
      //! Handler, which will be called on various events.
      TransmitDataHandlerPtr dataHandler;
      //! Handler which is called on completion of the operation.
      OperationCompletedHandler completionHandler;
      //! Address of the remote endpoint (TFTP Client).
      boost::asio::ip::udp::endpoint remote;
      //! TFTP Options Configuration.
      //! Will be used for TFTP Options Negotiation.
      TftpOptionsConfiguration optionsConfiguration;
      //! TFTP Client Options.
      //! Will be negotiated within TFTP Server Request Operation
      Packets::Options clientOptions;
      //! Additional Options, which have been already negotiated.
      Packets::Options additionalNegotiatedOptions;
      //! local endpoint, where the server handles the request from.
      std::optional< boost::asio::ip::udp::endpoint > local;
    };

    //! TFTP Server Write Operation Configuration
    struct WriteOperationConfiguration
    {
      //! Handler, which will be called on various events.
      ReceiveDataHandlerPtr dataHandler;
      //! Handler which is called on completion of the operation.
      OperationCompletedHandler completionHandler;
      //! Address of the remote endpoint (TFTP Client).
      boost::asio::ip::udp::endpoint remote;
      //! TFTP Options Configuration.
      //! Will be used for TFTP Options Negotiation.
      TftpOptionsConfiguration optionsConfiguration;
      //! TFTP Client Options.
      //! Will be negotiated within TFTP Server Request Operation
      Packets::Options clientOptions;
      //! Additional Options, which have been already negotiated.
      Packets::Options additionalNegotiatedOptions;
      //! local endpoint, where the server handles the request from.
      std::optional< boost::asio::ip::udp::endpoint > local;
    };

    /**
     * @brief Default UDP Endpoint, Where the TFTP Server Listens.
     *
     * The default local end-point where the TFTP server is listening is the
     * default TFTP Port any IP address.
     **/
    static const boost::asio::ip::udp::endpoint DefaultLocalEndpoint;

    /**
     * @brief Creates a TFTP Server Instance.
     *
     * @param[in] ioContext
     *   I/O context used for Communication.
     * @param[in] configuration
     *   TFTP Server Configuration
     *
     * @return Created TFTP Server Instance.
     **/
    [[nodiscard]] static TftpServerPtr instance(
      boost::asio::io_context &ioContext,
      ServerConfiguration configuration );

    //! Destructor
    virtual ~TftpServer() noexcept = default;

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
     * @brief Creates a TFTP Server Operation (TFTP RRQ), which reads data from
     *   disk and sends them to a TFTP Client.
     *
     * Data is obtained from @p dataHandler and transmitted to TFTP Client.
     *
     * @param[in] configuration
     *   Read Operation Configuration.
     *
     * @return TFTP server write operation.
     **/
    [[nodiscard]] virtual OperationPtr readOperation(
      ReadOperationConfiguration configuration ) = 0;

    /**
     * @brief Creates a TFTP Server Operation (TFTP WRQ), which receives data
     * from a TFTP Client and weites them to disk.
     *
     * Data is received form the client and written to @p dataHandler.
     *
     * @param[in] configuration
     *   Write Operation Configuration.
     *
     * @return TFTP Server Read Operation.
     **/
    [[nodiscard]] virtual OperationPtr writeOperation(
      WriteOperationConfiguration configuration ) = 0;

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
      Packets::ErrorCode errorCode,
      std::string errorMessage = {} ) = 0;

    /**
     * @copydoc errorOperation(const boost::asio::ip::udp::endpoint&,Packets::ErrorCode,std::string)
     *
     * @param[in] local
     *   Communication source
     **/
    virtual void errorOperation(
      const boost::asio::ip::udp::endpoint &remote,
      const boost::asio::ip::udp::endpoint &local,
      Packets::ErrorCode errorCode,
      std::string errorMessage = {} ) = 0;

  protected:
    //! Constructor
    TftpServer() = default;
};

}

#endif
