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
 * @brief Declaration of Class Tftp::Clients::ClientImpl.
 **/

#ifndef TFTP_CLIENTS_CLIENTIMPL_HPP
#define TFTP_CLIENTS_CLIENTIMPL_HPP

#include <tftp/clients/Clients.hpp>
#include <tftp/clients/Client.hpp>

#include <tftp/TftpOptionsConfiguration.hpp>

#include <boost/asio/io_context.hpp>

#include <optional>

namespace Tftp::Clients {

/**
 * @brief TFTP %Client.
 *
 * This factory class creates on request the concrete client operations.
 **/
class ClientImpl final : public Client
{
  public:
    /**
     * @brief Creates an Instance of the TFTP %Client.
     *
     * @param[in] ioContext
     *   I/O context used for Communication.
     **/
    explicit ClientImpl( boost::asio::io_context &ioContext );

    /**
     * @brief Destructor
     **/
    ~ClientImpl() override;

    //! @copydoc Client::tftpTimeoutDefault()
    Client& tftpTimeoutDefault( std::chrono::seconds timeout ) override;

    //! @copydoc Client::tftpRetriesDefault()
    Client& tftpRetriesDefault( uint16_t retries ) override;

    //! @copydoc Client::dallyDefault()
    Client& dallyDefault( bool dally ) override;

    //! @copydoc Client::optionsConfigurationDefault()
    Client& optionsConfigurationDefault(
      TftpOptionsConfiguration optionsConfiguration ) override;

    //! @copydoc Client::additionalOptions()
    Client& additionalOptions(
      Packets::Options additionalOptions ) override;

    //! @copydoc Client::localDefault()
    Client& localDefault( boost::asio::ip::address local ) override;

    //! @copydoc Client::readOperation()
    ReadOperationPtr readOperation() override;

    //! @copydoc Client::writeOperation()
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
