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
 * @brief Declaration of Class Tftp::Server::WriteOperationImpl.
 **/

#ifndef TFTP_SERVER_WRITEOPERATIONIMPL_HPP
#define TFTP_SERVER_WRITEOPERATIONIMPL_HPP

#include "tftp/server/Server.hpp"
#include "tftp/server/WriteOperation.hpp"

#include "tftp/server/implementation/OperationImpl.hpp"

#include "tftp/packets/BlockNumber.hpp"
#include "tftp/packets/TftpOptions.hpp"

#include "tftp/TftpOptionsConfiguration.hpp"

#include <boost/asio.hpp>

#include <chrono>
#include <string>

namespace Tftp::Server {

/**
 * @brief TFTP %Server Write %Operation (TFTP WRQ).
 *
 * In this operation a client has requested to write a file, which is
 * transmitted form the client to the server.
 *
 * This operation is initiated by a client TFTP write request (WRQ)
 **/
class WriteOperationImpl final :
  public WriteOperation,
  private OperationImpl
{
  public:
    /**
     * @brief Initialises the operation.
     *
     * @param[in] ioContext
     *   I/O context used for communication.
     **/
    explicit WriteOperationImpl( boost::asio::io_context &ioContext );

    //! Destructor
    ~WriteOperationImpl() override = default;

    //! @copydoc WriteOperation::start()
    void start() override;

    //! @copydoc WriteOperation::gracefulAbort()
    void gracefulAbort(
      Packets::ErrorCode errorCode,
      std::string errorMessage = {} ) override;

    //! @copydoc WriteOperation::abort()
    void abort() override;

    //! @copydoc WriteOperation::errorInfo()
    [[nodiscard]] const ErrorInfo& errorInfo() const override;

    //! @copydoc WriteOperation::tftpTimeout()
    WriteOperation& tftpTimeout( std::chrono::seconds timeout ) override;

    //! @copydoc WriteOperation::tftpRetries()
    WriteOperation& tftpRetries( uint16_t retries ) override;

    //! @copydoc WriteOperation::dally()
    WriteOperation& dally( bool dally ) override;

    //! @copydoc WriteOperation::optionsConfiguration()
    WriteOperation& optionsConfiguration(
      TftpOptionsConfiguration optionsConfiguration ) override;

    //! @copydoc WriteOperation::completionHandler()
    WriteOperation& completionHandler(
      OperationCompletedHandler handler ) override;

    //! @copydoc WriteOperation::dataHandler()
    WriteOperation& dataHandler( ReceiveDataHandlerPtr handler ) override;

    //! @copydoc WriteOperation::remote()
    WriteOperation& remote( boost::asio::ip::udp::endpoint remote ) override;

    //! @copydoc WriteOperation::local()
    WriteOperation& local( boost::asio::ip::udp::endpoint local ) override;

    //! @copydoc WriteOperation::clientOptions()
    WriteOperation& clientOptions( Packets::TftpOptions clientOptions ) override;

    //! @copydoc WriteOperation::additionalNegotiatedOptions()
    WriteOperation& additionalNegotiatedOptions(
      Packets::Options additionalNegotiatedOptions ) override;

  private:
    //! @copydoc OperationImpl::finished()
    void finished(
      TransferStatus status,
      ErrorInfo &&errorInfo = {} ) noexcept override;

    /**
     * @copydoc Packets::PacketHandler::dataPacket()
     *
     * The received data packet is checked and the
     * TftpReadOperationHandler::receivedData() operation of the registered
     * handler is called.
     **/
    void dataPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::DataPacket &dataPacket ) override;

    /**
     * @copydoc Packets::PacketHandler::acknowledgementPacket()
     *
     * Acknowledgement packets are not expected and handled as invalid.
     * An error is sent back, and the operation is cancelled.
     **/
    void acknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::AcknowledgementPacket &acknowledgementPacket ) override;

    //! If set to true, wait after transmission of the final ACK for potential
    //! retries.
    bool dallyV{ false };
    //! TFTP Options Configuration.
    TftpOptionsConfiguration optionsConfigurationV;
    //! Handler for Received Data.
    ReceiveDataHandlerPtr dataHandlerV;
    //! Received TFTP Client Options.
    Packets::TftpOptions clientOptionsV;
    //! Additional Options, which have been already negotiated.
    Packets::Options additionalNegotiatedOptionsV;

    //! Size of the data-section in the TFTP DATA packet - changed during option negotiation.
    uint16_t receiveDataSize{ Packets::DefaultDataSize };
    //! Last received block number.
    Packets::BlockNumber lastReceivedBlockNumber{ 0U };
};

}

#endif
