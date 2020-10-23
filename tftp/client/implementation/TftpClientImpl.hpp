/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Client::TftpClientImpl.
 **/

#ifndef TFTP_CLIENT_TFTPCLIENTIMPL_HPP
#define TFTP_CLIENT_TFTPCLIENTIMPL_HPP

#include <tftp/client/Client.hpp>
#include <tftp/client/TftpClient.hpp>

#include <tftp/options/OptionList.hpp>

#include <boost/asio.hpp>

namespace Tftp::Client {

/**
 * @brief TFTP %Client.
 *
 * This factory class creates on request the concrete client operations.
 **/
class TftpClientImpl : public TftpClient
  {
  public:
    /**
     * @brief Creates the concrete TFTP %Client.
     *
     * @param[in] tftpTimeout
     *   TFTP Timeout, when no timeout option is negotiated in seconds.
     * @param[in] tftpRetries
     *   Number of retries.
     **/
    explicit TftpClientImpl(
      uint8_t tftpTimeout,
      uint16_t tftpRetries );

    //! @copydoc TftpClient::entry
    void entry() final;

    //! @copydoc TftpClient::stop
    void stop() final;

    //! @copydoc TftpClient::readRequestOperation(OptionNegotiationHandler,ReceiveDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,std::string_view,TransferMode,const Options::OptionList&)
    OperationPtr readRequestOperation(
      OptionNegotiationHandler optionNegotiationHandler,
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      TransferMode mode,
      const Options::OptionList &clientOptions) final;

    //! @copydoc TftpClient::readRequestOperation(OptionNegotiationHandler,ReceiveDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,std::string_view,TransferMode,const Options::OptionList&,const boost::asio::ip::udp::endpoint&)
    OperationPtr readRequestOperation(
      OptionNegotiationHandler optionNegotiationHandler,
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      TransferMode mode,
      const Options::OptionList &clientOptions,
      const boost::asio::ip::udp::endpoint &local) final;

    //! @copydoc TftpClient::writeRequestOperation(OptionNegotiationHandler,TransmitDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,std::string_view,TransferMode,const Options::OptionList&)
    OperationPtr writeRequestOperation(
      OptionNegotiationHandler optionNegotiationHandler,
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      TransferMode mode,
      const Options::OptionList &clientOptions) final;

    //! @copydoc TftpClient::writeRequestOperation(OptionNegotiationHandler,TransmitDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,std::string_view,TransferMode,const Options::OptionList&,const boost::asio::ip::udp::endpoint&)
    OperationPtr writeRequestOperation(
      OptionNegotiationHandler optionNegotiationHandler,
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      TransferMode mode,
      const Options::OptionList &clientOptions,
      const boost::asio::ip::udp::endpoint &local) final;

  private:
    //! TFTP Receive Timeout
    const uint8_t tftpTimeout;
    //! TFTP Retries
    const uint16_t tftpRetries;
    //! I/O context, which handles the asynchronous receive operation
    boost::asio::io_context ioContext;
    //! I/O context dummy work to keep I/O context loops running
    boost::asio::io_context::work work;
};

}

#endif
