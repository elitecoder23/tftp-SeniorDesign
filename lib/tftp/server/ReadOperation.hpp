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
 * @brief Declaration of Class Tftp::Server::ReadOperation.
 **/

#ifndef TFTP_SERVER_READOPERATION_HPP
#define TFTP_SERVER_READOPERATION_HPP

#include "tftp/server/Server.hpp"
#include "tftp/server/Operation.hpp"

namespace Tftp::Server {

/**
 * @brief TFTP %Server Read %Operation (TFTP RRQ).
 *
 * In this operation a client has requested to read a file, which is
 * transmitted form the server to the client.
 * Therefore, the server performs a write operation.
 *
 * This operation is initiated by a client TFTP read request (RRQ)
 **/
class TFTP_EXPORT ReadOperation : public Operation
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
     * @param[in] timeout
     *   TFTP timeout.
     *
     * @return @p *this for chaining.
     **/
    virtual ReadOperation& tftpTimeout( std::chrono::seconds timeout ) = 0;

    /**
     * @brief Updates the NUmber of TFTP Packet Retries.
     *
     * @param[in] retries
     *   Number of TFTP Packet Retries.
     *
     * @return @p *this for chaining.
     **/
    virtual ReadOperation& tftpRetries( uint16_t retries ) = 0;

    /**
     * @brief Updates TFTP Options Configuration
     *
     * @param[in] optionsConfiguration
     *   TFTP Options Configuration.
     *
     * @return @p *this for chaining.
     **/
    virtual ReadOperation& optionsConfiguration(
      TftpOptionsConfiguration optionsConfiguration ) = 0;

    /**
     * @brief Updates the Operation Completed Handler
     *
     * @param[in] completionHandler
     *   Handler which is called on completion of the operation.
     *
     * @return @p *this for chaining.
     **/
    virtual ReadOperation& completionHandler(
      OperationCompletedHandler completionHandler ) = 0;

    /**
     * @brief Updates the Transmit Data Handler.
     *
     * @param[in] dataHandler
     *   Handler for Transmit Data.
     *
     * @return @return @p *this for chaining.
     **/
    virtual ReadOperation& dataHandler( TransmitDataHandlerPtr dataHandler ) = 0;

    /**
     * @brief Updates the remote (client address)
     *
     * @param[in] remote
     *   Where the connection should be established to.
     *
     * @return @return @p *this for chaining.
     **/
    virtual ReadOperation& remote( boost::asio::ip::udp::endpoint remote ) = 0;

    /**
     * @brief Updates the local address to use as connection source.
     *
     * to set a fixed IP-address and leave the UDP port up to the IP-Stack,
     * set the port to `0`.
     *
     * @param[in] local
     *   Parameter to define the communication source
     *
     * @return @return @p *this for chaining.
     **/
    virtual ReadOperation& local( boost::asio::ip::udp::endpoint local ) = 0;

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
    virtual ReadOperation& clientOptions(
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
    virtual ReadOperation& additionalNegotiatedOptions(
      Packets::Options additionalNegotiatedOptions ) = 0;

    /** @} **/
};

}

#endif
