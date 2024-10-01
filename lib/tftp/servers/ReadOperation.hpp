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
 * @brief Declaration of Class Tftp::Servers::ReadOperation.
 **/

#ifndef TFTP_SERVERS_READOPERATION_HPP
#define TFTP_SERVERS_READOPERATION_HPP

#include "tftp/servers/Servers.hpp"
#include "tftp/servers/Operation.hpp"

namespace Tftp::Servers {

/**
 * @brief TFTP %Server Read %Operation (TFTP RRQ).
 *
 * In this operation a client has requested to read a file, which is
 * transmitted form the server to the client.
 * Therefore, the server performs a write operation.
 *
 * This operation is initiated by a client TFTP read request (RRQ)
 **/
class TFTP_EXPORT ReadOperation : public Operation
{
  public:
    //! Destructor.
    ~ReadOperation() override = default;

    /**
     * @name Configuration Operations
     * @{
     **/

    //! @copydoc Operation::tftpTimeout()
    ReadOperation& tftpTimeout( std::chrono::seconds timeout ) override = 0;

    //! @copydoc Operation::tftpRetries()
    ReadOperation& tftpRetries( uint16_t retries ) override = 0;

    //! @copydoc Operation::optionsConfiguration()
    ReadOperation& optionsConfiguration(
      TftpOptionsConfiguration optionsConfiguration ) override = 0;

    //! @copydoc Operation::completionHandler()
    ReadOperation& completionHandler(
      OperationCompletedHandler handler ) override = 0;

    /**
     * @brief Updates the Transmit Data Handler.
     *
     * This handler is required.
     * If not provided the operation will fail.
     *
     * @param[in] handler
     *   Handler for Transmit Data.
     *
     * @return @p *this for chaining.
     **/
    virtual ReadOperation& dataHandler( TransmitDataHandlerPtr handler ) = 0;

    //! @copydoc Operation::remote()
    ReadOperation& remote( boost::asio::ip::udp::endpoint remote ) override = 0;

    //! @copydoc Operation::local()
    ReadOperation& local( boost::asio::ip::udp::endpoint local ) override = 0;

    //! @copydoc Operation::clientOptions()
    ReadOperation& clientOptions(
      Packets::TftpOptions clientOptions ) override = 0;

    //! @copydoc Operation::additionalNegotiatedOptions()
    ReadOperation& additionalNegotiatedOptions(
      Packets::Options additionalNegotiatedOptions ) override = 0;

    /** @} **/
};

}

#endif
