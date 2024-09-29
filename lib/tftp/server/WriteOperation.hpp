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
 * @brief Declaration of Class Tftp::Server::WriteOperation.
 **/

#ifndef TFTP_SERVER_WRITEOPERATION_HPP
#define TFTP_SERVER_WRITEOPERATION_HPP

#include "tftp/server/Server.hpp"
#include "tftp/server/Operation.hpp"

namespace Tftp::Server {

/**
 * @brief TFTP %Server Write %Operation (TFTP WRQ).
 *
 * In this operation a client has requested to write a file, which is
 * transmitted form the client to the server.
 *
 * This operation is initiated by a client TFTP write request (WRQ)
 **/
class TFTP_EXPORT WriteOperation : public Operation
{
  public:
    //! Destructor.
    ~WriteOperation() override = default;

    /**
     * @name Configuration Operations
     * @{
     **/

    //! @copydoc Operation::tftpTimeout()
    WriteOperation& tftpTimeout( std::chrono::seconds timeout ) override = 0;

    //! @copydoc Operation::tftpRetries()
    WriteOperation& tftpRetries( uint16_t retries ) override = 0;

    /**
     * @brief Updates the Dally Parameter.
     *
     * If the _dally_ option is set, the operation waits after transmission of
     * the last _Acknowledgment_ packet for potential retry operations.
     *
     * @param[in] dally
     *   If set to @p true, the @p dally handling is activated.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& dally( bool dally ) = 0;

    //! @copydoc Operation::optionsConfiguration()
    WriteOperation& optionsConfiguration(
      TftpOptionsConfiguration optionsConfiguration ) override = 0;

    //! @copydoc Operation::completionHandler()
    WriteOperation& completionHandler(
      OperationCompletedHandler handler ) override = 0;

    /**
     * @brief Updates the Receive Data Handler.
     *
     * This handler is required.
     * If not provided the operation will fail.
     *
     * @param[in] handler
     *   Handler for Received Data.
     *
     * @return @p *this for chaining.
     **/
    virtual WriteOperation& dataHandler( ReceiveDataHandlerPtr handler ) = 0;

    //! @copydoc Operation::remote()
    WriteOperation& remote(
      boost::asio::ip::udp::endpoint remote ) override = 0;

    //! @copydoc Operation::local()
    WriteOperation& local( boost::asio::ip::udp::endpoint local ) override = 0;

    //! @copydoc Operation::clientOptions()
    WriteOperation& clientOptions(
      Packets::TftpOptions clientOptions ) override = 0;

    //! @copydoc Operation::additionalNegotiatedOptions()
    WriteOperation& additionalNegotiatedOptions(
      Packets::Options additionalNegotiatedOptions ) override = 0;

    /** @} **/
};

}

#endif
