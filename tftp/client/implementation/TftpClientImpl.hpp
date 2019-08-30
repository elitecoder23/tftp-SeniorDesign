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
#include <tftp/client/implementation/TftpClientInternal.hpp>

#include <tftp/options/OptionList.hpp>

#include <tftp/TftpConfiguration.hpp>

#include <boost/asio.hpp>

namespace Tftp::Client {

/**
 * @brief Implementation of TFTP %Client Factory Interface.
 *
 * This factory class creates on request the concrete client operations.
 **/
class TftpClientImpl : public TftpClientInternal
  {
  public:
    /**
     * @brief Creates the concrete TFTP %Client.
     *
     * @param[in] configuration
     *   The TFTP Configuration
     **/
    explicit TftpClientImpl( const TftpConfiguration &configuration);

    //! @copydoc TftpClient::entry
    void entry() noexcept final;

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

    //! @copydoc TftpClientInternal::configuration
    [[nodiscard]] const TftpConfiguration& configuration() const final;

  private:
    //! The stored TFTP client configuration
    const TftpConfiguration configurationV;
    //! The I/O context, which handles the asynchronous receive operation
    boost::asio::io_context ioContext;
    //! I/O context dummy work to keep I/O context loops running
    boost::asio::io_context::work work;
};

}

#endif
