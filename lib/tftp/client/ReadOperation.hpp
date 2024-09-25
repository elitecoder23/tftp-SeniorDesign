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
 * @brief Declaration of Class Tftp::Client::ReadOperation.
 **/

#ifndef TFTP_CLIENT_READOPERATION_HPP
#define TFTP_CLIENT_READOPERATION_HPP

#include "tftp/client/Client.hpp"

#include "tftp/client/Operation.hpp"

namespace Tftp::Client {

//! TFTP Client Read Operation (RRQ)
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
     * @brief Updates the Dally Parameter.
     *
     * @param[in] dally
     *   If set to true, wait after transmission of the final ACK for potential
     *   retries.
     *
     * @return @return @p *this for chaining.
     **/
    virtual ReadOperation& dally( bool dally ) = 0;

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
     * @brief Updates additional TFTP Options
     *
     * By default, no additional Options are sent to the server.
     *
     * @param[in] additionalOptions
     *   Additional TFTP options sent to the server.
     *
     * @return @p *this for chaining.
     **/
    virtual ReadOperation& additionalOptions(
      Packets::Options additionalOptions ) = 0;

    /**
     * @brief Updates the Option Negotiation Handler.
     *
     * This handler is called for additional Options received by TFTP Option
     * Acknowledgment to verify, if the answer of the TFTP server is accepted.
     * When no user-defined Option Negotiation Handler is registered, the
     * Options acknowledged by the server are accepted.
     *
     * @param[in] optionNegotiationHandler
     *   Option negotiation handler.
     *
     * @return @p *this for chaining.
     **/
    virtual ReadOperation& optionNegotiationHandler(
      OptionNegotiationHandler optionNegotiationHandler ) = 0;

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
     * @brief Updates the Receive Data Handler.
     *
     * @param[in] dataHandler
     *   Handler for Received Data.
     *
     * @return @return @p *this for chaining.
     **/
    virtual ReadOperation& dataHandler( ReceiveDataHandlerPtr dataHandler ) = 0;

    /**
     * @brief Updates the Request filename
     *
     * @param[in] filename
     *   Which file shall be requested
     *
     * @return @p *this for chaining.
     **/
    virtual ReadOperation& filename( std::string filename ) = 0;

    /**
     * @brief Updates TFTP Transfer Mode
     *
     * @param[in] mode
     *   Transfer Mode
     *
     * @return @p *this for chaining.
     **/
    virtual ReadOperation& mode( Packets::TransferMode mode ) = 0;

    /**
     * @brief Updates the remote (server address)
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

    /** @} **/
};

}

#endif
