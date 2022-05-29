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
     * @param[in] ioContext
     *   The I/O context used for communication.
     * @param[in] configuration
     *   TFTP Client Configuration
     **/
    explicit TftpClientImpl(
      boost::asio::io_context &ioContext,
      ClientConfiguration configuration );

    //! @copydoc TftpClient::readOperation(ReadOperationConfiguration)
    OperationPtr readOperation(
      ReadOperationConfiguration configuration ) final;

    //! @copydoc TftpClient::writeOperation(WriteOperationConfiguration)
    OperationPtr writeOperation(
      WriteOperationConfiguration configuration  ) final;

  private:
    //! I/O context, which handles the asynchronous reception operation
    boost::asio::io_context &ioContext;
    //! TFTP Client Configuration
    ClientConfiguration configurationV;
};

}

#endif
