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
 * @brief Declaration of Class Tftp::Server::WriteOperation.
 **/

#ifndef TFTP_SERVER_WRITEOPERATION_HPP
#define TFTP_SERVER_WRITEOPERATION_HPP

#include "tftp/server/Server.hpp"
#include "tftp/server/Operation.hpp"

#include <boost/asio/ip/udp.hpp>

namespace Tftp::Server {

/**
 * @brief TFTP %Server Write %Operation (TFTP WRQ).
 *
 * In this operation a client has requested to write a file, which is
 * transmitted form the client to the server.
 *
 * This operation is initiated by a client TFTP write request (WRQ)
 **/
class TFTP_EXPORT WriteOperation : public Operation
{
  public:
    /**
     * @name Configuration Operations
     * @{
     **/

    /**
     * @brief Updates TFTP Timeout.
     *
     * TFTP Timeout, when no timeout option is negotiated in seconds.
     *
     * If the _TFTP Timeout_ parameter is not set, the TFTP defaults are used.
     *
     * @param[in] timeout
     *   TFTP timeout.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& tftpTimeout( std::chrono::seconds timeout ) = 0;

    /**
     * @brief Updates the Number of TFTP Packet Retries.
     *
     * If the _TFTP Packet Retries_ parameter is not set, the TFTP defaults are
     * used.
     *
     * @param[in] retries
     *   Number of TFTP Packet Retries.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& tftpRetries( uint16_t retries ) = 0;

    /**
     * @brief Updates the Dally Parameter.
     *
     * If the _dally_ option is set, the operation waits after transmission of
     * the last _Acknowledgment_ packet for potential retry operations.
     *
     * @param[in] dally
     *   If set to @p true, the @p dally handling is activated.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& dally( bool dally ) = 0;

    /**
     * @brief Updates TFTP Options Configuration.
     *
     * If no TFTP Option configuration is provided, the defaults are used.
     *
     * @param[in] optionsConfiguration
     *   TFTP Options Configuration.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& optionsConfiguration(
      TftpOptionsConfiguration optionsConfiguration ) = 0;

    /**
     * @brief Updates the Operation Completed Handler.
     *
     * @param[in] completionHandler
     *   Handler which is called on completion of the operation.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& completionHandler(
      OperationCompletedHandler completionHandler ) = 0;

    /**
     * @brief Updates the Receive Data Handler.
     *
     * This handler is required.
     * If not provided the operation will fail.
     *
     * @param[in] dataHandler
     *   Handler for Received Data.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& dataHandler( ReceiveDataHandlerPtr dataHandler ) = 0;

    /**
     * @brief Updates the remote (client address).
     *
     * @param[in] remote
     *   Where the connection should be established to.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& remote( boost::asio::ip::udp::endpoint remote ) = 0;

    /**
     * @brief Updates the local address to use as connection source.
     *
     * To set a fixed IP-address and leave the UDP port up to the IP-Stack,
     * set the port to `0`.
     *
     * @param[in] local
     *   Parameter to define the communication source
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& local( boost::asio::ip::udp::endpoint local ) = 0;

    /**
     * @brief Updates the Client Options
     *
     * This options will be negotiated within TFTP Server Request Operation.
     *
     * @param[in] clientOptions
     *   Received TFTP Client Options
     *
     * @return @return @p *this for chaining.
     **/
    virtual WriteOperation& clientOptions(
      Packets::TftpOptions clientOptions ) = 0;

    /**
     * @brief Updates additional negotiated TFTP Options
     *
     * By default, no additional Options are sent to the client.
     *
     * @param[in] additionalNegotiatedOptions
     *   Additional negotiated TFTP options sent to the client.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& additionalNegotiatedOptions(
      Packets::Options additionalNegotiatedOptions ) = 0;

    /** @} **/
};

}

#endif
