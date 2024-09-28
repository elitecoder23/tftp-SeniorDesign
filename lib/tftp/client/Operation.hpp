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
 * @brief Declaration of Class Tftp::Client::Operation.
 **/

#ifndef TFTP_CLIENT_OPERATION_HPP
#define TFTP_CLIENT_OPERATION_HPP

#include "tftp/client/Client.hpp"

#include "tftp/packets/Packets.hpp"

#include <boost/asio/ip/udp.hpp>

#include <string>

namespace Tftp::Client {

/**
 * @brief TFTP %Client %Operation.
 *
 * This class is specialised for the two kinds of TFTP operations
 * (Read Operation, Write Operation).
 **/
class TFTP_EXPORT Operation
{
  public:
    //! Destructor.
    virtual ~Operation() = default;

    /**
     * @name Configuration Operations
     * @{
     **/

    /**
     * @brief Updates TFTP Timeout.
     *
     * TFTP Timeout, when no timeout option is negotiated in seconds.
     * If the _TFTP Timeout_ parameter is not set, the TFTP defaults are used.
     *
     * @param[in] timeout
     *   TFTP timeout.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& tftpTimeout( std::chrono::seconds timeout ) = 0;

    /**
     * @brief Updates the NUmber of TFTP Packet Retries.
     *
     * If the _TFTP Packet Retries_ parameter is not set, the TFTP defaults are
     * used.
     *
     * @param[in] retries
     *   Number of TFTP Packet Retries.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& tftpRetries( uint16_t retries ) = 0;

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
    virtual Operation& optionsConfiguration(
      TftpOptionsConfiguration optionsConfiguration ) = 0;

    /**
     * @brief Updates additional TFTP Options
     *
     * This operation can be used to add addition options to the TFTP request.
     * By default, no additional Options are sent to the server.
     * If the negotiated options, sent by the server shall be checked an
     * _Option Negotiation Handler_ must be provided.
     *
     * @param[in] additionalOptions
     *   Additional TFTP options sent to the server.
     *
     * @return @p *this for chaining.
     *
     * @sa @ref ReadOperation::optionNegotiationHandler()
     **/
    virtual Operation& additionalOptions(
      Packets::Options additionalOptions ) = 0;

    /**
     * @brief Updates the Option Negotiation Handler.
     *
     * This handler is called for additional options received by TFTP Option
     * Acknowledgment to verify, if the answer of the TFTP server is accepted.
     * When no user-defined Option Negotiation Handler is registered, the
     * Options acknowledged by the server are accepted.
     *
     * @param[in] handler
     *   Option negotiation handler.
     *
     * @return @p *this for chaining.
     *
     * @sa @ref ReadOperation::additionalOptions()
     **/
    virtual Operation& optionNegotiationHandler(
      OptionNegotiationHandler handler ) = 0;

    /**
     * @brief Updates the Operation Completed Handler.
     *
     * @param[in] handler
     *   Handler which is called on completion of the operation.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& completionHandler(
      OperationCompletedHandler handler ) = 0;

    /**
     * @brief Updates the Request Filename.
     *
     * This parameter is required.
     *
     * @param[in] filename
     *   Which file shall be requested
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& filename( std::string filename ) = 0;

    /**
     * @brief Updates TFTP Transfer Mode
     *
     * This parameter is required.
     *
     * @param[in] mode
     *   Transfer Mode
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& mode( Packets::TransferMode mode ) = 0;

    /**
     * @brief Updates the remote (server address).
     *
     * This parameter is required.
     *
     * @param[in] remote
     *   Where the connection should be established to.
     *
     * @return @p *this for chaining.
     **/
    virtual Operation& remote( boost::asio::ip::udp::endpoint remote ) = 0;

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
    virtual Operation& local( boost::asio::ip::udp::endpoint local ) = 0;

    /** @} **/

    /**
     * @brief Executes the TFTP %Client %Operation.
     *
     * It prepares the TFTP request packet, sends it to the remote endpoint and
     * start the reception loop.
     *
     * It returns immediately after sending the request.
     **/
    virtual void request() = 0;

    /**
     * @brief Aborts the %Operation Gracefully.
     *
     * Sends an error packet at next possible time point.
     *
     * @param[in] errorCode
     *   TFTP error code.
     * @param[in] errorMessage
     *   An additional error message.
     **/
    virtual void gracefulAbort(
      Packets::ErrorCode errorCode,
      std::string errorMessage = {} ) = 0;

    /**
     * @brief Aborts the %Operation Immediately.
     *
     * No error message is sent.
     **/
    virtual void abort() = 0;

    /**
     * @brief Returns the Error Information of this %Operation.
     *
     * @return The error information of this operation
     * @retval ErrorInfo()
     *   If no error occurred.
     **/
    [[nodiscard]] virtual const ErrorInfo& errorInfo() const = 0;
};

}

#endif
