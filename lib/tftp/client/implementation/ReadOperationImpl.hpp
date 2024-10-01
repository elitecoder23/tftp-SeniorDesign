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
 * @brief Declaration of Class Tftp::Client::ReadOperationImpl.
 **/

#ifndef TFTP_CLIENT_READOPERATIONIMPL_HPP
#define TFTP_CLIENT_READOPERATIONIMPL_HPP

#include "tftp/client/Client.hpp"
#include "tftp/client/ReadOperation.hpp"

#include "tftp/client/implementation/OperationImpl.hpp"

#include "tftp/packets/BlockNumber.hpp"

#include "tftp/TftpOptionsConfiguration.hpp"

#include <chrono>

namespace Tftp::Client {

//! TFTP %Client Read %Operation (TFTP RRQ).
class ReadOperationImpl final :
  public ReadOperation,
  private OperationImpl
{
  public:
    /**
     * @brief Initialises the operation.
     *
     * @param[in] ioContext
     *   I/O context used for communication.
     **/
    explicit ReadOperationImpl( boost::asio::io_context &ioContext );

    //! Destructor
    ~ReadOperationImpl() override = default;

    //! @copydoc ReadOperation::request()
    void request() override;

    //! @copydoc ReadOperation::gracefulAbort()
    void gracefulAbort(
      Packets::ErrorCode errorCode,
      std::string errorMessage = {} ) override;

    //! @copydoc ReadOperation::abort()
    void abort() override;

    //! @copydoc ReadOperation::errorInfo() const
    [[nodiscard]] const Packets::ErrorInfo& errorInfo() const override;

    //! @copydoc ReadOperation::tftpTimeout()
    ReadOperation& tftpTimeout( std::chrono::seconds timeout ) override;

    //! @copydoc ReadOperation::tftpRetries()
    ReadOperation& tftpRetries( uint16_t retries ) override;

    //! @copydoc ReadOperation::dally()
    ReadOperation& dally( bool dally ) override;

    //! @copydoc ReadOperation::optionsConfiguration()
    ReadOperation& optionsConfiguration(
      TftpOptionsConfiguration optionsConfiguration ) override;

    //! @copydoc ReadOperation::additionalOptions()
    ReadOperation& additionalOptions(
      Packets::Options additionalOptions ) override;

    //! @copydoc ReadOperation::optionNegotiationHandler()
    ReadOperation& optionNegotiationHandler(
      OptionNegotiationHandler handler ) override;

    //! @copydoc ReadOperation::completionHandler()
    ReadOperation& completionHandler(
      OperationCompletedHandler handler ) override;

    //! @copydoc ReadOperation::dataHandler()
    ReadOperation& dataHandler( ReceiveDataHandlerPtr handler ) override;

    //! @copydoc ReadOperation::filename()
    ReadOperation& filename( std::string filename ) override;

    //! @copydoc ReadOperation::mode()
    ReadOperation& mode( Packets::TransferMode mode ) override;

    //! @copydoc ReadOperation::remote()
    ReadOperation& remote( boost::asio::ip::udp::endpoint remote ) override;

    //! @copydoc ReadOperation::local()
    ReadOperation& local( boost::asio::ip::udp::endpoint local ) override;

  private:
    //! @copydoc OperationImpl::finished()
    void finished(
      TransferStatus status,
      Packets::ErrorInfo &&errorInfo = {} ) noexcept override;

    /**
     * @copydoc Packets::PacketHandler::dataPacket()
     *
     * The TFTP DATA packet is decoded and checked.
     * If everything is fine, handler is called with extracted data and the
     * reception operation is continued.
     **/
    void dataPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::DataPacket &dataPacket ) override;

    /**
     * @copydoc Packets::PacketHandler::acknowledgementPacket()
     *
     * ACK packets are not expected for this operation.
     * They are rejected by error transmission.
     **/
    void acknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::AcknowledgementPacket &acknowledgementPacket ) override;

    /**
     * @copydoc Packets::PacketHandler::optionsAcknowledgementPacket()
     *
     * Checks received Options for validity and finalises the option negotiation.
     **/
    void optionsAcknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket ) override;

    //! TFTP Options Configuration.
    TftpOptionsConfiguration optionsConfigurationV;
    //! Additional TFTP options sent to the server.
    Packets::Options additionalOptionsV;
    //! Option Negotiation Handler
    OptionNegotiationHandler optionNegotiationHandlerV;
    //! Handler for Received Data.
    ReceiveDataHandlerPtr dataHandlerV;
    //! Which file shall be requested
    std::string filenameV;
    //! Transfer Mode
    Packets::TransferMode modeV{ Packets::TransferMode::Invalid };
    //! If set to true, wait after transmission of the final ACK for potential
    //! retries.
    bool dallyV{ false };

    //! flag to hold information if OACK has been received (used when first data packet is received)
    bool oackReceived{ false };
    //! Size of the data-section in the TFTP DATA packet - changed during option negotiation.
    uint16_t receiveDataSize{ Packets::DefaultDataSize };
    //! Last received block number.
    Packets::BlockNumber lastReceivedBlockNumber{ 0U };
};

}

#endif
