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
 * @brief Declaration of Class Tftp::Clients::Client.
 **/

#ifndef TFTP_CLIENTS_CLIENT_HPP
#define TFTP_CLIENTS_CLIENT_HPP

#include <tftp/clients/Clients.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>

namespace Tftp::Clients {

/**
 * @brief TFTP %Client.
 *
 * This class acts as factory for creating client operations, like read
 * requests or write requests.
 *
 * An instance is created by calling @ref Client::instance().
 **/
class TFTP_EXPORT Client
{
  public:
    /**
     * @brief Creates a TFTP %Client Instance.
     *
     * With the instance you can create any client operation instances.
     *
     * @param[in] ioContext
     *   I/O context used for communication.
     *
     * @return Created TFTP Client Instance.
     **/
    [[nodiscard]] static ClientPtr instance(
      boost::asio::io_context &ioContext );

    //! Destructor
    virtual ~Client() noexcept = default;

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
    virtual Client& tftpTimeoutDefault( std::chrono::seconds timeout ) = 0;

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
    virtual Client& tftpRetriesDefault( uint16_t retries ) = 0;

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
    virtual Client& dallyDefault( bool dally ) = 0;

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
    virtual Client& optionsConfigurationDefault(
      TftpOptionsConfiguration optionsConfiguration ) = 0;

    /**
     * @brief Updates Default additional TFTP Options
     *
     * This operation can be used to add addition options to the TFTP request.
     * By default, no additional Options are sent to the server.
     * If the negotiated options, sent by the server shall be checked an
     * _Option Negotiation Handler_ must be provided.
     *
     * If this option is set, every created operation will be initialised with
     * the value.
     *
     * @param[in] additionalOptions
     *   Additional TFTP options sent to the server.
     *
     * @return @p *this for chaining.
     **/
    virtual Client& additionalOptions(
      Packets::Options additionalOptions ) = 0;

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
    virtual Client& localDefault( boost::asio::ip::address local ) = 0;

    /** @} **/

    /**
     * @brief Creates a TFTP Client Read %Operation (RRQ).
     *
     * @return TFTP client read operation instance.
     *
     * @sa @ref ReadOperation
     **/
    [[nodiscard]] virtual ReadOperationPtr readOperation() = 0;

    /**
     * @brief Creates a TFTP Client Write %Operation (WRQ).
     *
     * @return TFTP client write operation instance.
     *
     * @sa @ref WriteOperation
     **/
    [[nodiscard]] virtual WriteOperationPtr writeOperation() = 0;

  protected:
    //! Constructor.
    Client() = default;
};

}

#endif
