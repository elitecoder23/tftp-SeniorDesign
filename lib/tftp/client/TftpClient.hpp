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
 * @brief Declaration of Class Tftp::Client::TftpClient.
 **/

#ifndef TFTP_CLIENT_TFTPCLIENT_HPP
#define TFTP_CLIENT_TFTPCLIENT_HPP

#include <tftp/client/Client.hpp>

#include <boost/asio/io_context.hpp>

namespace Tftp::Client {

/**
 * @brief TFTP %Client.
 *
 * This class acts as factory for creating client operations, like read
 * requests or write requests.
 **/
class TFTP_EXPORT TftpClient
{
  public:
    /**
     * @brief Creates a TFTP %Client Instance.
     *
     * With the instance you can create any client operation instances.
     *
     * @param[in] ioContext
     *   I/O context used for communication.
     *
     * @return Created TFTP Client Instance.
     **/
    [[nodiscard]] static TftpClientPtr instance(
      boost::asio::io_context &ioContext );

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
