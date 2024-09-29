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

#include <chrono>
#include <functional>
#include <optional>
#include <string>

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
     * @name TFTP Server Configuration
     * @{
     **/

    /**
     * @brief Set TFTP Request Received Handler
     *
     * @param[in] handler
     *   TFTP Request Handler
     *
     * @return *this for chaining.
     **/
    virtual TftpServer& requestHandler(
      ReceivedTftpRequestHandler handler ) = 0;

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

    /** @} **/

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
     * @name Default Operation Configuration Operations
     * @{
     **/

    /**
     * @brief Updates Default TFTP Timeout.
     *
     * TFTP Timeout, when no timeout option is negotiated in seconds.
     * If the _TFTP Timeout_ parameter is not set, the TFTP defaults are used.
     *
     * If this option is set, every created operation will be initialised with
     * the value.
     *
     * @param[in] timeout
     *   TFTP timeout.
     *
     * @return @p *this for chaining.
     **/
    virtual TftpServer& tftpTimeoutDefault( std::chrono::seconds timeout ) = 0;

    /**
     * @brief Updates the Default Number of TFTP Packet Retries.
     *
     * If the _TFTP Packet Retries_ parameter is not set, the TFTP defaults are
     * used.
     *
     * If this option is set, every created operation will be initialised with
     * the value.
     *
     * @param[in] retries
     *   Number of TFTP Packet Retries.
     *
     * @return @p *this for chaining.
     **/
    virtual TftpServer& tftpRetriesDefault( uint16_t retries ) = 0;

    /**
     * @brief Updates the Default Dally Parameter.
     *
     * If the _dally_ option is set, the operation waits after transmission of
     * the last _Acknowledgment_ packet for potential retry operations.
     *
     * If this option is set, every created operation will be initialised with
     * the value.
     *
     * @param[in] dally
     *   If set to @p true, the @p dally handling is activated.
     *
     * @return @p *this for chaining.
     **/
    virtual TftpServer& dallyDefault( bool dally ) = 0;

    /**
     * @brief Updates Default TFTP Options Configuration.
     *
     * If no TFTP Option configuration is provided, the defaults are used.
     *
     * If this option is set, every created operation will be initialised with
     * the value.
     *
     * @param[in] optionsConfiguration
     *   TFTP Options Configuration.
     *
     * @return @p *this for chaining.
     **/
    virtual TftpServer& optionsConfigurationDefault(
      TftpOptionsConfiguration optionsConfiguration ) = 0;

    /**
     * @brief Updates the local address to use as connection source.
     *
     * To set a fixed IP-address and leave the UDP port up to the IP-Stack,
     * set the port to `0`.
     *
     * @param[in] local
     *   Local IP address
     *
     * @return @p *this for chaining.
     **/
    virtual TftpServer& localDefault( boost::asio::ip::address local ) = 0;

    /** @} **/

    /**
     * @brief Starts the TFTP Server.
     *
     * This routine starts the server receive operation.
     * This routine returns immediately.
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
     * @brief Creates a TFTP %Server %Operation (TFTP RRQ), which reads data from
     *   disk and sends them to a TFTP Client.
     *
     * Data is obtained from @p dataHandler and transmitted to TFTP Client.
     *
     * @return TFTP server read operation.
     **/
    [[nodiscard]] virtual ReadOperationPtr readOperation() = 0;

    /**
     * @brief Creates a TFTP Server Operation (TFTP WRQ), which receives data
     * from a TFTP Client and weites them to disk.
     *
     * Data is received form the client and written to @p dataHandler.
     *
     * @return TFTP server write operation.
     **/
    [[nodiscard]] virtual WriteOperationPtr writeOperation() = 0;

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
    //! Constructor.
    TftpServer() = default;
};

}

#endif
