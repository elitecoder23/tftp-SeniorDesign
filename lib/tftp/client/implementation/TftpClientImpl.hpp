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
 * @brief Declaration of Class Tftp::Client::TftpClientImpl.
 **/

#ifndef TFTP_CLIENT_TFTPCLIENTIMPL_HPP
#define TFTP_CLIENT_TFTPCLIENTIMPL_HPP

#include "tftp/client/Client.hpp"

#include "tftp/client/TftpClient.hpp"
#include "tftp/TftpOptionsConfiguration.hpp"

#include <boost/asio/io_context.hpp>

#include <optional>

namespace Tftp::Client {

/**
 * @brief TFTP %Client.
 *
 * This factory class creates on request the concrete client operations.
 **/
class TftpClientImpl final : public TftpClient
{
  public:
    /**
     * @brief Creates an Instance of the TFTP %Client.
     *
     * @param[in] ioContext
     *   I/O context used for Communication.
     **/
    explicit TftpClientImpl( boost::asio::io_context &ioContext );

    /**
     * @brief Destructor
     **/
    ~TftpClientImpl() override;

    //! @copydoc TftpClient::tftpTimeoutDefault()
    TftpClient& tftpTimeoutDefault( std::chrono::seconds timeout ) override;

    //! @copydoc TftpClient::tftpRetriesDefault()
    TftpClient& tftpRetriesDefault( uint16_t retries ) override;

    //! @copydoc TftpClient::dallyDefault()
    TftpClient& dallyDefault( bool dally ) override;

    //! @copydoc TftpClient::optionsConfigurationDefault()
    TftpClient& optionsConfigurationDefault(
      TftpOptionsConfiguration optionsConfiguration ) override;

    //! @copydoc TftpClient::additionalOptions()
    TftpClient& additionalOptions(
      Packets::Options additionalOptions ) override;

    //! @copydoc TftpClient::localDefault()
    TftpClient& localDefault( boost::asio::ip::address local ) override;

    //! @copydoc TftpClient::readOperation()
    ReadOperationPtr readOperation() override;

    //! @copydoc TftpClient::writeOperation()
    WriteOperationPtr writeOperation() override;

  private:
    //! I/O context, which handles the asynchronous reception operation
    boost::asio::io_context &ioContext;
    //! Default timeout for TFTP operations
    std::optional< std::chrono::seconds > tftpTimeoutDefaultV;
    //! Default number of retries for TFTP operations
    std::optional< uint16_t > tftpRetriesDefaultV;
    //! Default value for the DALLY option
    std::optional< bool > dallyDefaultV;
    //! Default value for the options configuration
    std::optional< TftpOptionsConfiguration > optionsConfigurationDefaultV;
    //! Additional options
    Packets::Options additionalOptionsV;
    //! Default local IP address
    boost::asio::ip::address localV;
};

}

#endif
