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
 * @brief Declaration of Struct Tftp::Server::ReadOperationConfiguration.
 **/

#ifndef TFTP_SERVER_READOPERATIONCONFIGURATION_HPP
#define TFTP_SERVER_READOPERATIONCONFIGURATION_HPP

#include <tftp/server/Server.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <tftp/TftpOptionsConfiguration.hpp>

#include <boost/asio/ip/udp.hpp>

#include <optional>
#include <chrono>

namespace Tftp::Server {

//! TFTP Server Read Operation Configuration
struct TFTP_EXPORT ReadOperationConfiguration
{
  //! TFTP Timeout, when no timeout option is negotiated in seconds.
  std::chrono::seconds tftpTimeout;
  //! Number of retries.
  uint16_t tftpRetries;
  //! TFTP Options Configuration.
  //! Will be used for TFTP Options Negotiation.
  TftpOptionsConfiguration optionsConfiguration;

  //! Handler which is called on completion of the operation.
  OperationCompletedHandler completionHandler;
  //! Handler, which will be called on various events.
  TransmitDataHandlerPtr dataHandler;

  //! Address of the remote endpoint (TFTP Client).
  boost::asio::ip::udp::endpoint remote;
  //! TFTP Client Options.
  //! Will be negotiated within TFTP Server Request Operation
  Packets::TftpOptions clientOptions;
  //! Additional Options, which have been already negotiated.
  Packets::Options additionalNegotiatedOptions;
  //! local endpoint, where the server handles the request from.
  std::optional< boost::asio::ip::udp::endpoint > local;

  /**
   * @brief Initialises the configuration.
   *
   * @param[in] configuration
   *   TFTP Configuration
   * @param[in] optionsConfiguration
   *   TFTP Options Configuration
   * @param[in] completionHandler
   *   Operation Completion Handler
   * @param[in] dataHandler
   *   Transmit Data Handler
   * @param[in] remote
   *   Request remote address
   * @param[in] clientOptions
   *   Received Client Options
   * @param[in] additionalNegotiatedOptions
   *   Negotiated Additional Options
   * @param[in] local
   *   Local Address
   **/
  ReadOperationConfiguration(
    const TftpConfiguration &configuration,
    TftpOptionsConfiguration optionsConfiguration,
    OperationCompletedHandler completionHandler,
    TransmitDataHandlerPtr dataHandler,
    boost::asio::ip::udp::endpoint remote,
    Packets::TftpOptions clientOptions,
    Packets::Options additionalNegotiatedOptions,
    std::optional< boost::asio::ip::udp::endpoint > local = {} );

  /**
   * @brief Initialises the configuration.
   *
   * @param[in] tftpTimeout
   *   TFTP Timeout
   * @param[in] tftpRetries
   *   TFTP Retries
   * @param[in] optionsConfiguration
   *   TFTP Options Configuration
   * @param[in] completionHandler
   *   Operation Completion Handler
   * @param[in] dataHandler
   *   Transmit Data Handler
   * @param[in] remote
   *   Request remote address
   * @param[in] clientOptions
   *   Received Client Options
   * @param[in] additionalNegotiatedOptions
   *   Negotiated Additional Options
   * @param[in] local
   *   Local Address
   **/
  ReadOperationConfiguration(
    std::chrono::seconds tftpTimeout,
    uint16_t tftpRetries,
    TftpOptionsConfiguration optionsConfiguration,
    OperationCompletedHandler completionHandler,
    TransmitDataHandlerPtr dataHandler,
    boost::asio::ip::udp::endpoint remote,
    Packets::TftpOptions clientOptions,
    Packets::Options additionalNegotiatedOptions,
    std::optional< boost::asio::ip::udp::endpoint > local = {} );
};

}

#endif
