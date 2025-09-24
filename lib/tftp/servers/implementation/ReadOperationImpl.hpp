// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Servers::ReadOperationImpl.
 **/

#ifndef TFTP_SERVERS_READOPERATIONIMPL_HPP
#define TFTP_SERVERS_READOPERATIONIMPL_HPP

#include <tftp/servers/Servers.hpp>
#include <tftp/servers/ReadOperation.hpp>

#include <tftp/servers/implementation/OperationImpl.hpp>

#include <tftp/packets/BlockNumber.hpp>
#include <tftp/packets/TftpOptions.hpp>

#include <tftp/TftpOptionsConfiguration.hpp>

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_context.hpp>

#include <chrono>
#include <string>

namespace Tftp::Servers {

/**
 * @brief TFTP %Server Read %Operation (TFTP RRQ).
 *
 * In this operation, a client has requested to read a file, which is transmitted from the server to the client.
 * Therefore, the server performs a write operation.
 *
 * This operation is initiated by a client TFTP read request (RRQ)
 **/
class ReadOperationImpl final : public ReadOperation, private OperationImpl
{
  public:
    /**
     * @brief Initialises the TFTP read operation instance.
     *
     * @param[in] ioContext
     *   I/O context used for communication.
     **/
    explicit ReadOperationImpl( boost::asio::io_context &ioContext );

    //! Destructor.
    ~ReadOperationImpl() override = default;

    //! @copydoc ReadOperation::tftpTimeout()
    ReadOperation& tftpTimeout( std::chrono::seconds timeout ) override;

    //! @copydoc ReadOperation::tftpRetries()
    ReadOperation& tftpRetries( uint16_t retries ) override;

    //! @copydoc ReadOperation::optionsConfiguration()
    ReadOperation& optionsConfiguration( TftpOptionsConfiguration optionsConfiguration ) override;

    //! @copydoc ReadOperation::completionHandler()
    ReadOperation& completionHandler( OperationCompletedHandler handler ) override;

    //! @copydoc ReadOperation::dataHandler()
    ReadOperation& dataHandler( TransmitDataHandlerPtr handler ) override;

    //! @copydoc ReadOperation::remote()
    ReadOperation& remote( boost::asio::ip::udp::endpoint remote ) override;

    //! @copydoc ReadOperation::local()
    ReadOperation& local( boost::asio::ip::udp::endpoint local ) override;

    //! @copydoc ReadOperation::clientOptions()
    ReadOperation& clientOptions( Packets::TftpOptions clientOptions ) override;

    //! @copydoc ReadOperation::additionalNegotiatedOptions()
    ReadOperation& additionalNegotiatedOptions( Packets::Options additionalNegotiatedOptions ) override;

    //! @copydoc ReadOperation::start()
    void start() override;

    //! @copydoc ReadOperation::gracefulAbort()
    void gracefulAbort( Packets::ErrorCode errorCode, std::string errorMessage = {} ) override;

    //! @copydoc ReadOperation::abort()
    void abort() override;

    //! @copydoc ReadOperation::errorInformation() const
    [[nodiscard]] const Packets::ErrorInformation& errorInformation() const override;

  private:
    //! @copydoc OperationImpl::finished()
    void finished( TransferStatus status, Packets::ErrorInformation errorInformation = {} ) noexcept override;

    /**
     * @brief Sends a data packet to the client.
     *
     * The Data packet is assembled by calling the registered handler operation TftpWriteOperationHandler::sendData().
     * If the last data packet is sent, the internal flag will be set appropriately.
     **/
    void sendData();

    /**
     * @copydoc Packets::PacketHandler::dataPacket
     *
     * Data packets are not expected and handled as invalid.
     * An error is sent back and the operation is cancelled.
     **/
    void dataPacket( const boost::asio::ip::udp::endpoint &remote, const Packets::DataPacket &dataPacket ) override;

    /**
     * @copydoc Packets::PacketHandler::acknowledgementPacket()
     *
     * The acknowledgement packet is checked and the next data sequence is
     * handled.
     **/
    void acknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::AcknowledgementPacket &acknowledgementPacket ) override;

    //! TFTP Options Configuration.
    TftpOptionsConfiguration optionsConfigurationV;
    //! Handler for Transmit Data.
    TransmitDataHandlerPtr dataHandlerV;
    //! Received TFTP Client Options.
    Packets::TftpOptions clientOptionsV;
    //! Additional Options, which have been already negotiated.
    Packets::Options additionalNegotiatedOptionsV;

    //! Contains the negotiated block size option.
    uint16_t transmitDataSize{ Packets::DefaultDataSize };
    //! Indicates if the last data packet has been transmitted (closing).
    bool lastDataPacketTransmitted{ false };
    //! Block number of the last transmitted data packet.
    Packets::BlockNumber lastTransmittedBlockNumber{ 0U };
    //! Last received block number.
    Packets::BlockNumber lastReceivedBlockNumber{ 0U };
};

}

#endif
