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
 * @brief Declaration of Struct Tftp::Client::ReadOperationConfiguration.
 **/

#ifndef TFTP_CLIENT_READOPERATIONCONFIGURATION_HPP
#define TFTP_CLIENT_READOPERATIONCONFIGURATION_HPP

#include "tftp/client/Client.hpp"

#include "tftp/TftpOptionsConfiguration.hpp"

#include <boost/asio/ip/udp.hpp>

#include <optional>
#include <chrono>
#include <string>

namespace Tftp::Client {

//! TFTP Client Read Operation Configuration
struct TFTP_EXPORT ReadOperationConfiguration
{
  //! TFTP Timeout, when no timeout option is negotiated in seconds.
  std::chrono::seconds tftpTimeout;
  //! Number of retries.
  uint16_t tftpRetries;
  //! If set to true, wait after transmission of the final ACK for potential
  //! retries.
  bool dally;
  //! TFTP Options Configuration.
  TftpOptionsConfiguration optionsConfiguration;

  //! Option negotiation handler.
  OptionNegotiationHandler optionNegotiationHandler;
  //! Handler which is called on completion of the operation.
  OperationCompletedHandler completionHandler;
  //! Handler for Received Data.
  ReceiveDataHandlerPtr dataHandler;

  //! Which file shall be requested
  std::string filename;
  //! Transfer Mode
  Packets::TransferMode mode;
  //! Additional TFTP options sent to the server.
  Packets::Options additionalOptions;
  //! Where the connection should be established to.
  boost::asio::ip::udp::endpoint remote;
  //! Parameter to define the communication source
  std::optional< boost::asio::ip::udp::endpoint > local;

  /**
   * @brief Initialises the configuration.
   *
   * @param[in] configuration
   *   TFTP Configuration
   * @param[in] optionsConfiguration
   *   TFTP Options Configuration
   * @param[in] optionNegotiationHandler
   *   TFTP Options Negotiation Handler
   * @param[in] completionHandler
   *   Operation Completion Handler
   * @param[in] dataHandler
   *   Receive Data Handler
   * @param[in] filename
   *   Request Filename
   * @param[in] mode
   *   Request Mode
   * @param[in] additionalOptions
   *   Additional Options
   * @param[in] remote
   *   Request remote address
   * @param[in] local
   *   Request local address
   **/
  ReadOperationConfiguration(
    const TftpConfiguration &configuration,
    TftpOptionsConfiguration optionsConfiguration,
    OptionNegotiationHandler optionNegotiationHandler,
    OperationCompletedHandler completionHandler,
    ReceiveDataHandlerPtr dataHandler,
    std::string filename,
    Packets::TransferMode mode,
    Packets::Options additionalOptions,
    boost::asio::ip::udp::endpoint remote,
    std::optional< boost::asio::ip::udp::endpoint > local = {} );

  /**
   * @brief Initialises the configuration.
   *
   * @param[in] tftpTimeout
   *   TFTP Timeout
   * @param[in] tftpRetries
   *   TFTP Retries
   * @param[in] dally
   *   TFTP Dally Parameter
   * @param[in] optionsConfiguration
   *   TFTP Options Configuration
   * @param[in] optionNegotiationHandler
   *   TFTP Options Negotiation Handler
   * @param[in] completionHandler
   *   Operation Completion Handler
   * @param[in] dataHandler
   *   Receive Data Handler
   * @param[in] filename
   *   Request Filename
   * @param[in] mode
   *   Request Mode
   * @param[in] additionalOptions
   *   Additional Options
   * @param[in] remote
   *   Request remote address
   * @param[in] local
   *   Request local address
   **/
  ReadOperationConfiguration(
    std::chrono::seconds tftpTimeout,
    uint16_t tftpRetries,
    bool dally,
    TftpOptionsConfiguration optionsConfiguration,
    OptionNegotiationHandler optionNegotiationHandler,
    OperationCompletedHandler completionHandler,
    ReceiveDataHandlerPtr dataHandler,
    std::string filename,
    Packets::TransferMode mode,
    Packets::Options additionalOptions,
    boost::asio::ip::udp::endpoint remote,
    std::optional< boost::asio::ip::udp::endpoint > local = {} );
};

}

#endif
