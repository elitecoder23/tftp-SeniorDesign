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
 * @brief Declaration of Class Tftp::Client::WriteOperationImpl.
 **/

#ifndef TFTP_CLIENT_WRITEOPERATIONIMPL_HPP
#define TFTP_CLIENT_WRITEOPERATIONIMPL_HPP

#include "tftp/client/Client.hpp"
#include "tftp/client/WriteOperation.hpp"

#include "tftp/client/implementation/OperationImpl.hpp"

#include "tftp/packets/BlockNumber.hpp"

#include "tftp/TftpOptionsConfiguration.hpp"

#include <chrono>

namespace Tftp::Client {

//! TFTP %Client Write %Operation (TFTP WRQ).
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

    //! @copydoc WriteOperation::request()
    void request() override;

    //! @copydoc WriteOperation::gracefulAbort()
    void gracefulAbort(
      Packets::ErrorCode errorCode,
      std::string errorMessage = {} ) override;

    //! @copydoc WriteOperation::abort()
    void abort() override;

    //! @copydoc WriteOperation::errorInfo() const
    [[nodiscard]] const ErrorInfo& errorInfo() const override;

    //! @copydoc WriteOperation::tftpTimeout()
    WriteOperation& tftpTimeout( std::chrono::seconds timeout ) override;

    //! @copydoc WriteOperation::tftpRetries()
    WriteOperation& tftpRetries( uint16_t retries ) override;

    //! @copydoc WriteOperation::optionsConfiguration()
    WriteOperation& optionsConfiguration(
      TftpOptionsConfiguration optionsConfiguration ) override;

    //! @copydoc WriteOperation::additionalOptions()
    WriteOperation& additionalOptions(
      Packets::Options additionalOptions ) override;

    //! @copydoc WriteOperation::optionNegotiationHandler()
    WriteOperation& optionNegotiationHandler(
      OptionNegotiationHandler handler ) override;

    //! @copydoc WriteOperation::completionHandler()
    WriteOperation& completionHandler(
      OperationCompletedHandler handler ) override;

    //! @copydoc WriteOperation::dataHandler()
    WriteOperation& dataHandler( TransmitDataHandlerPtr handler ) override;

    //! @copydoc WriteOperation::filename()
    WriteOperation& filename( std::string filename ) override;

    //! @copydoc WriteOperation::mode()
    WriteOperation& mode( Packets::TransferMode mode ) override;

    //! @copydoc WriteOperation::remote()
    WriteOperation& remote( boost::asio::ip::udp::endpoint remote ) override;

    //! @copydoc WriteOperation::local()
    WriteOperation& local( boost::asio::ip::udp::endpoint local ) override;

  private:
    //! @copydoc OperationImpl::finished()
    void finished(
      TransferStatus status,
      ErrorInfo &&errorInfo = {} ) noexcept override;

    /**
     * @brief Sends the data to the host.
     *
     * This operation requests the data from the handler, generates the TFTP
     * DATA packet and sends them to the host.
     **/
    void sendData();

    /**
     * @copydoc Packets::PacketHandler::dataPacket()
     *
     * @throw InvalidPacketException
     *   Always, because an this packet is invalid.
     **/
    void dataPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::DataPacket &dataPacket ) override;

    /**
     * @copydoc Packets::PacketHandler::acknowledgementPacket()
     *
     * @throw InvalidPacketException
     *   Invalid block number
     **/
    void acknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::AcknowledgementPacket &acknowledgementPacket ) override;

    /**
     * @copydoc Packets::PacketHandler::optionsAcknowledgementPacket()
     *
     * @throw InvalidPacketException
     *   Empty option list
     * @throw OptionNegotiationException
     *   Option negotiation failed
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
    //! Handler for Transmit Data.
    TransmitDataHandlerPtr dataHandlerV;
    //! Which file shall be requested
    std::string filenameV;
    //! Transfer Mode
    Packets::TransferMode modeV{ Packets::TransferMode::Invalid };

    //! Size of the data-section in the TFTP DATA packet - changed during option negotiation.
    uint16_t transmitDataSize{ Packets::DefaultDataSize };
    //! Indicates, if the last data packet has been transmitted (closing).
    bool lastDataPacketTransmitted{ false };
    //! Block number of the last transmitted data packet.
    Packets::BlockNumber lastTransmittedBlockNumber{ 0U };
    //! Last received block number.
    Packets::BlockNumber lastReceivedBlockNumber{ 0U };
    //! Transfer Size obtained from Data Handler
    std::optional< uint64_t > transferSize{};
};

}

#endif
