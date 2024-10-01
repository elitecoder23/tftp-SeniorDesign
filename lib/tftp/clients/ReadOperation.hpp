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
 * @brief Declaration of Class Tftp::Clients::ReadOperation.
 **/

#ifndef TFTP_CLIENTS_READOPERATION_HPP
#define TFTP_CLIENTS_READOPERATION_HPP

#include "tftp/clients/Clients.hpp"
#include "tftp/clients/Operation.hpp"

namespace Tftp::Clients {

/**
 * @brief TFTP %Client Read %Operation (TFTP RRQ).
 *
 * After executed, the class sends the TFTP RRQ packet to the destination and
 * waits for answer.
 * Received data is handled by the ReceiveDataHandler.
 **/
class TFTP_EXPORT ReadOperation : public Operation
{
  public:
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

    //! @copydoc Operation::additionalOptions()
    ReadOperation& additionalOptions(
      Packets::Options additionalOptions ) override = 0;

    //! @copydoc Operation::optionNegotiationHandler()
    ReadOperation& optionNegotiationHandler(
      OptionNegotiationHandler handler ) override = 0;

    //! @copydoc Operation::completionHandler()
    ReadOperation& completionHandler(
      OperationCompletedHandler handler ) override = 0;

    //! @copydoc Operation::filename()
    ReadOperation& filename( std::string filename ) override = 0;

    //! @copydoc Operation::mode()
    ReadOperation& mode( Packets::TransferMode mode ) override = 0;

    //! @copydoc Operation::remote()
    ReadOperation& remote( boost::asio::ip::udp::endpoint remote ) override = 0;

    //! @copydoc Operation::local()
    ReadOperation& local( boost::asio::ip::udp::endpoint local ) override = 0;

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
    virtual ReadOperation& dally( bool dally ) = 0;

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
    virtual ReadOperation& dataHandler( ReceiveDataHandlerPtr handler ) = 0;

    /** @} **/
};

}

#endif
