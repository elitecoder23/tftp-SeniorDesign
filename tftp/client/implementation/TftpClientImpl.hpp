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
 * @brief Implementation of TFTP Client factory interface.
 *
 * This factory class creates on request the concrete client operations.
 **/
class TftpClientImpl : public TftpClientInternal
  {
  public:
    /**
     * @brief Creates the concrete TFTP client.
     *
     * @param[in] configuration
     *   The TFTP Configuration
     * @param[in] additionalOptions
     *   Additional Options, which shall be used as TFTP client option list.
     **/
    TftpClientImpl(
      const TftpConfiguration &configuration,
      const Options::OptionList& additionalOptions);

    //!@copydoc TftpClient::entry
    void entry() noexcept final;

    //!@copydoc TftpClient::stop
    void stop() final;

    //!@copydoc TftpClient::readRequestOperation(ReceiveDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,const std::string&,TransferMode,const boost::asio::ip::udp::endpoint&)
    OperationPtr readRequestOperation(
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const std::string &filename,
      TransferMode mode,
      const boost::asio::ip::udp::endpoint &local) final;

    //!@copydoc TftpClient::writeRequestOperation(TransmitDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,const std::string&,TransferMode,const boost::asio::ip::udp::endpoint&)
    OperationPtr writeRequestOperation(
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const std::string &filename,
      TransferMode mode,
      const boost::asio::ip::udp::endpoint &local) final;

    //! @copydoc TftpClientInternal::configuration
    const TftpConfiguration& configuration() const final;

    //! @copydoc TftpClientInternal::options
    const Options::OptionList& options() const final;

  private:
    //! The stored TFTP client configuration
    const TftpConfiguration configurationV;
    //! The stored TFTP options
    const Options::OptionList optionsV;
    //! The I/O context, which handles the asynchronous receive operation
    boost::asio::io_context ioContext;
    //! I/O context dummy work to keep I/O context loops running
    boost::asio::io_context::work work;
};

}

#endif
