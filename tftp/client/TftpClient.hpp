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

#include <boost/asio/ip/udp.hpp>

#include <string_view>

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
    /**
     * @brief Creates a TFTP %Client Instance.
     *
     * With the instance you can create any client operation instances.
     *
     * @param[in] tftpTimeout
     *   TFTP Timeout, when no timeout option is negotiated in seconds.
     * @param[in] tftpRetries
     *   Number of retries.
     *
     * @return Created TFTP Client Instance.
     **/
    static TftpClientPtr instance(
      uint8_t tftpTimeout,
      uint16_t tftpRetries );

    //! Destructor
    virtual ~TftpClient() noexcept = default;

    /**
     * @brief Entry of the TFTP Client.
     *
     * This routines enters the IO loop.
     * The start routine will be left, when an FATAL error occurred or
     * the server has been stopped by calling stop().
     *
     * This entry can be called multiple time to allow parallel transfer
     * handling.
     **/
    virtual void entry() = 0;

    /**
     * @brief Stops the TFTP Client.
     *
     * This routine returns immediately.
     *
     * It can be called before entry() is called.
     **/
    virtual void stop() = 0;

    /**
     * @brief Initialises the Operation.
     *
     * @param[in] optionNegotiationHandler
     *   Option negotiation handler.
     * @param[in] dataHandler
     *   Handler for received data.
     * @param[in] completionHandler
     *   The handler which is called on completion of the operation.
     * @param[in] remote
     *   Where the connection should be established to.
     * @param[in] filename
     *   Which file shall be requested
     * @param[in] mode
     *   Transfer Mode
     * @param[in] optionsConfiguration
     *   TFTP Options Configuration.
     * @param[in] additionalOptions
     *   Additional TFTP options sent to the server.
     *
     * @return The client operation instance.
     **/
    virtual OperationPtr readRequestOperation(
      OptionNegotiationHandler optionNegotiationHandler,
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      TransferMode mode,
      const TftpOptionsConfiguration &optionsConfiguration,
      const Options &additionalOptions ) = 0;

    /**
     * @copydoc readRequestOperation(OptionNegotiationHandler,ReceiveDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,std::string_view,TransferMode,const TftpOptionsConfiguration&,const Options&)
     *
     * @param[in] local
     *   Parameter to define the communication source
     **/
    virtual OperationPtr readRequestOperation(
      OptionNegotiationHandler optionNegotiationHandler,
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      TransferMode mode,
      const TftpOptionsConfiguration &optionsConfiguration,
      const Options &additionalOptions,
      const boost::asio::ip::udp::endpoint &local ) = 0;

    /**
     * @brief Creates an write request operation (TFTP WRQ).
     *
     * @param[in] optionNegotiationHandler
     *   Option negotiation handler.
     * @param[in] dataHandler
     *   Handler for data.
     * @param[in] completionHandler
     *   Handler which is called on completion of the operation.
     * @param[in] remote
     *   Where the connection should be established to.
     * @param[in] filename
     *   Which file shall be requested
     * @param[in] mode
     *   Transfer Mode
     * @param[in] optionsConfiguration
     *   TFTP Options Configuration.
     * @param[in] additionalOptions
     *   Additional TFTP options sent to the server.
     *
     * @return TFTP Client Operation Instance.
     **/
    virtual OperationPtr writeRequestOperation(
      OptionNegotiationHandler optionNegotiationHandler,
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      TransferMode mode,
      const TftpOptionsConfiguration &optionsConfiguration,
      const Options &additionalOptions ) = 0;

    /**
     * @copydoc writeRequestOperation(OptionNegotiationHandler,TransmitDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,std::string_view,TransferMode,const TftpOptionsConfiguration&,const Options&)
     *
     * @param[in] local
     *   Parameter to define the communication source
     **/
    virtual OperationPtr writeRequestOperation(
      OptionNegotiationHandler optionNegotiationHandler,
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      TransferMode mode,
      const TftpOptionsConfiguration &optionsConfiguration,
      const Options &additionalOptions,
      const boost::asio::ip::udp::endpoint &local ) = 0;

  protected:
    //! Constructor.
    TftpClient() = default;
};

}

#endif
