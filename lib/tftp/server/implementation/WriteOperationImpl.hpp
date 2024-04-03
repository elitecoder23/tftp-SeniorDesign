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

#include <tftp/server/Server.hpp>
#include <tftp/server/TftpServer.hpp>
#include <tftp/server/WriteOperationConfiguration.hpp>

#include <tftp/server/implementation/OperationImpl.hpp>

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/BlockNumber.hpp>

#include <tftp/TftpOptionsConfiguration.hpp>

#include <boost/asio.hpp>

#include <string>
#include <chrono>

namespace Tftp::Server {

/**
 * @brief TFTP %Server Write %Operation (TFTP WRQ).
 *
 * In this operation a client has requested to write a file, which is
 * transmitted form the client to the server.
 *
 * This operation is initiated by a client TFTP write request (WRQ)
 **/
class WriteOperationImpl final : public OperationImpl
{
  public:
    /**
     * @brief Constructs the class.
     *
     * @param[in] ioContext
     *   I/O context used for communication.
     * @param[in] configuration
     *   Write Operation Configuration.
     **/
    WriteOperationImpl(
      boost::asio::io_context &ioContext,
      WriteOperationConfiguration configuration );

    /**
     * @brief Standard destructor.
     **/
    ~WriteOperationImpl() override = default;

    //! @copydoc OperationImpl::start()
    void start() override;

  private:
    //! @copydoc OperationImpl::finished()
    void finished(
      TransferStatus status,
      ErrorInfo &&errorInfo = {} ) noexcept override;

    /**
     * @copydoc Packets::PacketHandler::dataPacket
     *
     * The received data packet is checked and the
     * TftpReadOperationHandler::receivedData() operation of the registered
     * handler is called.
     **/
    void dataPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::DataPacket &dataPacket ) override;

    /**
     * @copydoc Packets::PacketHandler::acknowledgementPacket
     *
     * Acknowledgement packets are not expected and handled as invalid.
     * An error is sent back, and the operation is cancelled.
     **/
    void acknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::AcknowledgementPacket &acknowledgementPacket ) override;

    //! TFTP Server Write Operation Configuration
    WriteOperationConfiguration configurationV;
    //! Size of the data-section in the TFTP DATA packet - changed during option negotiation.
    uint16_t receiveDataSize{ Packets::DefaultDataSize };
    //! Holds the last received block number.
    Packets::BlockNumber lastReceivedBlockNumber{ 0U };
};

}

#endif
