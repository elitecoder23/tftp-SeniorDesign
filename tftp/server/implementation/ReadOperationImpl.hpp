/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Server::ReadOperationImpl.
 **/

#ifndef TFTP_SERVER_READOPERATIONIMPL_HPP
#define TFTP_SERVER_READOPERATIONIMPL_HPP

#include <tftp/server/Server.hpp>

#include <tftp/server/TftpServer.hpp>
#include <tftp/server/implementation/OperationImpl.hpp>

#include <tftp/packets/BlockNumber.hpp>

#include <tftp/TftpOptionsConfiguration.hpp>

#include <boost/asio.hpp>

#include <string>
#include <chrono>

namespace Tftp::Server {

/**
 * @brief TFTP %Server Read %Operation (TFTP RRQ).
 *
 * In this operation a client has requested to read a file, which is
 * transmitted form the server to the client.
 * Therefore, the server performs a write operation.
 *
 * This operation is initiated by a client TFTP read request (RRQ)
 **/
class ReadOperationImpl final : public OperationImpl
{
  public:
    /**
     * @brief Initialises the TFTP server write operation instance.
     *
     * @param[in] ioContext
     *   I/O context used for communication.
     * @param[in] configuration
     *   Read Operation Configuration.
     **/
    ReadOperationImpl(
      boost::asio::io_context &ioContext,
      TftpServer::ReadOperationConfiguration configuration );

    //! Destructor
    ~ReadOperationImpl() noexcept final = default;

    //! @copydoc OperationImpl::start()
    void start() final;

  private:
    //! @copydoc OperationImpl::finished()
    void finished(
      TransferStatus status,
      ErrorInfo &&errorInfo = {} ) noexcept final;

    /**
     * @brief Sends a data packet to the client.
     *
     * The Data packet is assembled by calling the registered handler
     * operation TftpWriteOperationHandler::sendData().
     * If the last data packet will be sent, the internal flag will be set
     * appropriate.
     **/
    void sendData();

    /**
     * @copydoc Packets::PacketHandler::dataPacket
     *
     * Data packets are not expected and handled as invalid.
     * An error is sent back and the operation is cancelled.
     **/
    void dataPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::DataPacket &dataPacket) final;

    /**
     * @copydoc Packets::PacketHandler::acknowledgementPacket
     *
     * The acknowledgement packet is checked and the next data sequence is
     * handled.
     **/
    void acknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::AcknowledgementPacket &acknowledgementPacket) final;

    //! TFTP Server Read Operation Configuration
    TftpServer::ReadOperationConfiguration configurationV;

    //! Contains the negotiated block size option.
    uint16_t transmitDataSize{ Packets::DefaultDataSize };
    //! Indicates, if the last data packet has been transmitted (closing).
    bool lastDataPacketTransmitted{ false };
    //! Stored last transmitted block number.
    Packets::BlockNumber lastTransmittedBlockNumber{ 0U };
    //! Stored last received block number.
    Packets::BlockNumber lastReceivedBlockNumber{ 0U };
};

}

#endif
