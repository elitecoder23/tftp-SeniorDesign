// SPDX-License-Identifier: MPL-2.0
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

#include "tftp/server/Server.hpp"

#include <boost/asio/io_context.hpp>

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
class TFTP_EXPORT TftpServer
{
  public:
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
     *   I/O context used for communication.
     *
     * @return TFTP Server Instance.
     **/
    [[nodiscard]] static TftpServerPtr instance(
      boost::asio::io_context &ioContext );

    //! Destructor
    virtual ~TftpServer() = default;

    /**
     * @brief Set TFTP Request Received Handler
     *
     * @param[in] requestHandler
     *   TFTP Request Handler
     *
     * @return *this for chaining.
     **/
    virtual TftpServer& requestHandler(
      ReceivedTftpRequestHandler requestHandler ) = 0;

    /**
     * @brief Set Address where the TFTP server should listen on.
     *
     * If address is not set @ref DefaultLocalEndpoint is used.
     *
     * @param serverAddress
     *   Address where the TFTP server should listen on.
     *
     * @return *this for chaining.
     **/
    virtual TftpServer& serverAddress(
      boost::asio::ip::udp::endpoint serverAddress ) = 0;

    /**
     * @brief Returns the effective local endpoint.
     *
     * Is used to determine the local endpoint, when an automatic local endpoint
     * is selected.
     *
     * @note
     * The return value is valid after calling @ref start(), when the port is
     * bound, actually.
     *
     * @return Local endpoint.
     **/
    [[nodiscard]] virtual boost::asio::ip::udp::endpoint
    localEndpoint() const = 0;

    /**
     * @brief Starts the TFTP Server.
     *
     * This routine starts the server receive operation.
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
     * The error operation is executed (Error Packet sent) immediately.
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
