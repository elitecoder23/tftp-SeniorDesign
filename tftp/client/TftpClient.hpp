/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Client::TftpClient.
 **/

#ifndef TFTP_CLIENT_TFTPCLIENT_HPP
#define TFTP_CLIENT_TFTPCLIENT_HPP

#include <tftp/client/Client.hpp>
#include <tftp/TftpOptionsConfiguration.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

#include <string_view>
#include <optional>

namespace Tftp::Client {

/**
 * @brief TFTP %Client.
 *
 * This class acts as factory for creating client operations, like read
 * requests or write requests.
 **/
class TftpClient
{
  public:
    //! TFTP Client Configuration
    struct ClientConfiguration
    {
      //! TFTP Timeout, when no timeout option is negotiated in seconds.
      uint8_t tftpTimeout;
      //! Number of retries.
      uint16_t tftpRetries;
      //! If set to true, wait after transmission of the final ACK for potential
      //! retries.
      //! Used by TFTP RRQ Operation
      bool dally;
    };

    //! TFTP Client Read Operation Configuration
    struct ReadOperationConfiguration
    {
      //! Option negotiation handler.
      OptionNegotiationHandler optionNegotiationHandler;
      //! Handler which is called on completion of the operation.
      OperationCompletedHandler completionHandler;
      //! Which file shall be requested
      std::string_view filename;
      //! Transfer Mode
      TransferMode mode;
      //! TFTP Options Configuration.
      TftpOptionsConfiguration optionsConfiguration;
      //! Additional TFTP options sent to the server.
      Options additionalOptions;
      //! Where the connection should be established to.
      boost::asio::ip::udp::endpoint remote;
      //! Parameter to define the communication source
      std::optional< boost::asio::ip::udp::endpoint > local;
      //! Handler for received data.
      ReceiveDataHandlerPtr dataHandler;
    };

    //! TFTP Client Write Operation Configuration
    struct WriteOperationConfiguration
    {
      //! Option negotiation handler.
      OptionNegotiationHandler optionNegotiationHandler;
      //! Handler which is called on completion of the operation.
      OperationCompletedHandler completionHandler;
      //! Which file shall be requested
      std::string_view filename;
      //! Transfer Mode
      TransferMode mode;
      //! TFTP Options Configuration.
      TftpOptionsConfiguration optionsConfiguration;
      //! Additional TFTP options sent to the server.
      Options additionalOptions;
      //! Where the connection should be established to.
      boost::asio::ip::udp::endpoint remote;
      //! Parameter to define the communication source
      std::optional< boost::asio::ip::udp::endpoint > local;
      //! Handler for received data.
      TransmitDataHandlerPtr dataHandler;
    };

    /**
     * @brief Creates a TFTP %Client Instance.
     *
     * With the instance you can create any client operation instances.
     *
     * @param[in] ioContext
     *   The I/O context used for communication.
     * @param[in] configuration
     *   TFTP Client Configuration
     *
     * @return Created TFTP Client Instance.
     **/
    [[nodiscard]] static TftpClientPtr instance(
      boost::asio::io_context &ioContext,
      ClientConfiguration configuration );

    //! Destructor
    virtual ~TftpClient() noexcept = default;

    /**
     * @brief Initialises a TFTP Client Read Operation (RRQ).
     *
     * @param[in] configuration
     *   Read Operation Configuration.
     *
     * @return Client Operation Instance.
     **/
    [[nodiscard]] virtual OperationPtr readOperation(
      ReadOperationConfiguration configuration ) = 0;

    /**
     * @brief Initialises a TFTP Client Write Operation (WRQ).
     *
     * @param[in] configuration
     *   Write Operation Configuration.
     *
     * @return TFTP Client Operation Instance.
     **/
    [[nodiscard]] virtual OperationPtr writeOperation(
      WriteOperationConfiguration configuration ) = 0;

  protected:
    //! Constructor.
    TftpClient() = default;
};

}

#endif
