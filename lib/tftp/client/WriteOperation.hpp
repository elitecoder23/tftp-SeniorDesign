/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Client::WriteOperation.
 **/

#ifndef TFTP_CLIENT_WRITEOPERATION_HPP
#define TFTP_CLIENT_WRITEOPERATION_HPP

#include "tftp/client/Client.hpp"

#include "tftp/client/Operation.hpp"

namespace Tftp::Client {

//! TFTP Client Write Operation (WRQ)
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
     * @param[in] timeout
     *   TFTP timeout.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& tftpTimeout( std::chrono::seconds timeout ) = 0;

    /**
     * @brief Updates the NUmber of TFTP Packet Retries.
     *
     * @param[in] retries
     *   Number of TFTP Packet Retries.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& tftpRetries( uint16_t retries ) = 0;


    /**
     * @brief Updates TFTP Options Configuration
     *
     * @param[in] optionsConfiguration
     *   TFTP Options Configuration.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& optionsConfiguration(
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
    virtual WriteOperation& additionalOptions(
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
    virtual WriteOperation& optionNegotiationHandler(
      OptionNegotiationHandler optionNegotiationHandler ) = 0;

    /**
     * @brief Updates the Operation Completed Handler
     *
     * @param[in] completionHandler
     *   Handler which is called on completion of the operation.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& completionHandler(
      OperationCompletedHandler completionHandler ) = 0;

    /**
     * @brief Updates the Transmit Data Handler.
     *
     * @param[in] dataHandler
     *   Handler for Send Data.
     *
     * @return @return @p *this for chaining.
     **/
    virtual WriteOperation& dataHandler(
      TransmitDataHandlerPtr dataHandler ) = 0;

    /**
     * @brief Updates the Request filename
     *
     * @param[in] filename
     *   Which file shall be requested
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& filename( std::string filename ) = 0;

    /**
     * @brief Updates TFTP Transfer Mode
     *
     * @param[in] mode
     *   Transfer Mode
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& mode( Packets::TransferMode mode ) = 0;

    /**
     * @brief Updates the remote (server address)
     *
     * @param[in] remote
     *   Where the connection should be established to.
     *
     * @return @return @p *this for chaining.
     **/
    virtual WriteOperation& remote( boost::asio::ip::udp::endpoint remote ) = 0;

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
    virtual WriteOperation& local( boost::asio::ip::udp::endpoint local ) = 0;

    /** @} **/
};

}

#endif
