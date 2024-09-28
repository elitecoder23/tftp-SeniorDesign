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
 * @brief Declaration of Class Tftp::Client::WriteOperation.
 **/

#ifndef TFTP_CLIENT_WRITEOPERATION_HPP
#define TFTP_CLIENT_WRITEOPERATION_HPP

#include "tftp/client/Client.hpp"
#include "tftp/client/Operation.hpp"

namespace Tftp::Client {

/**
 * @brief TFTP %Client Write %Operation (TFTP WRQ).
 *
 * After executed, the class sends the TFTP WRQ packet to the destination and
 * waits for answer.
 * Data is handled by the TransmitDataHandler.
 **/
class TFTP_EXPORT WriteOperation : public Operation
{
  public:
    /**
     * @name Configuration Operations
     * @{
     **/

    //! @copydoc Operation::tftpTimeout()
    WriteOperation& tftpTimeout( std::chrono::seconds timeout ) override = 0;

    //! @copydoc Operation::tftpRetries()
    WriteOperation& tftpRetries( uint16_t retries ) override = 0;

    //! @copydoc Operation::optionsConfiguration()
    WriteOperation& optionsConfiguration(
      TftpOptionsConfiguration optionsConfiguration ) override = 0;

    //! @copydoc Operation::additionalOptions()
    WriteOperation& additionalOptions(
      Packets::Options additionalOptions ) override = 0;

    //! @copydoc Operation::optionNegotiationHandler()
    WriteOperation& optionNegotiationHandler(
      OptionNegotiationHandler handler ) override = 0;

    //! @copydoc Operation::completionHandler()
    WriteOperation& completionHandler(
      OperationCompletedHandler handler ) override = 0;

    //! @copydoc Operation::filename()
    WriteOperation& filename( std::string filename ) override = 0;

    //! @copydoc Operation::mode()
    WriteOperation& mode( Packets::TransferMode mode ) override = 0;

    //! @copydoc Operation::remote()
    WriteOperation& remote(
      boost::asio::ip::udp::endpoint remote ) override = 0;

    //! @copydoc Operation::local()
    WriteOperation& local( boost::asio::ip::udp::endpoint local ) override = 0;

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
    virtual WriteOperation& dataHandler( TransmitDataHandlerPtr handler ) = 0;

    /** @} **/
};

}

#endif
