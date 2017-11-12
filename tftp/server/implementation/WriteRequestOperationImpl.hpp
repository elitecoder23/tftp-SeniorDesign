/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::Server::WriteRequestOperationImpl.
 **/

#ifndef TFTP_SERVER_WRITEREQUESTOPERATIONIMPL_HPP
#define TFTP_SERVER_WRITEREQUESTOPERATIONIMPL_HPP

#include <tftp/server/Server.hpp>

#include <tftp/server/implementation/OperationImpl.hpp>

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/BlockNumber.hpp>

#include <boost/asio.hpp>

#include <string>

namespace Tftp {
namespace Server {

/**
 * @brief TFTP server read operation.
 *
 * In this operation a client has requested to write a file, which is
 * transmitted form the client to the server.
 *
 * This operation is initiated by a client TFTP write request (WRQ)
 **/
class WriteRequestOperationImpl: public OperationImpl
{
  public:
    /**
     * @brief Constructs the class.
     *
     * @param[in] ioService
     *   The IO service used for communication.
     * @param[in] dataHandler
     *   Handler, which will be called on various events.
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
     * @param[in] tftpServerInternal
     *   The TFTP internal server.
     * @param[in] clientAddress
     *   Address of the remote endpoint (TFTP client).
     * @param[in] clientOptions
     *   Received option list from client.
     * @param[in] serverAddress
     *   local endpoint, where the server handles the request from.
     **/
    WriteRequestOperationImpl(
      boost::asio::io_service &ioService,
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const TftpServerInternal &tftpServerInternal,
      const UdpAddressType &clientAddress,
      const Options::OptionList &clientOptions,
      const UdpAddressType &serverAddress);

    /**
     * @brief Constructs the class.
     *
     * @param[in] ioService
     *   The IO service used for communication.
     * @param[in] dataHandler
     *   Handler, which will be called on various events.
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
     * @param[in] tftpServerInternal
     *   The TFTP internal server.
     * @param[in] clientAddress
     *   Address of the remote endpoint (TFTP client).
     * @param[in] clientOptions
     *   Received option list from client.
     **/
    WriteRequestOperationImpl(
      boost::asio::io_service &ioService,
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const TftpServerInternal &tftpServerInternal,
      const UdpAddressType &clientAddress,
      const Options::OptionList &clientOptions);

    /**
     * @brief Standard destructor.
     **/
    virtual ~WriteRequestOperationImpl() noexcept = default;

    /**
     * @brief Executes the operation.
     **/
    virtual void start() override final;

  private:
    //! @copydoc OperationImpl::finished()
    virtual void finished(
      TransferStatus status,
      ErrorInfo &&errorInfo = {}) noexcept override final;

    /**
     * @copydoc PacketHandler::handleDataPacket
     *
     * The received data packet is checked and the
     * TftpReadOperationHandler::receviedData() operation of the registered
     * handler is called.
     **/
    virtual void handleDataPacket(
      const UdpAddressType &from,
      const Packets::DataPacket &dataPacket) override;

    /**
     * @copydoc PacketHandler::handleAcknowledgementPacket
     *
     * Acknowledgement packets are not expected and handled as invalid.
     * An error is sent back and the operation is cancelled.
     **/
    virtual void handleAcknowledgementPacket(
      const UdpAddressType &from,
      const Packets::AcknowledgementPacket &acknowledgementPacket) override;

  private:
    //! Handler which will be called on various events.
    ReceiveDataHandlerPtr dataHandler;
    //! Size of the data-section in the TFTP DATA packet - changed during option negotiation.
    uint16_t receiveDataSize;
    //! Holds the last received block number.
    Packets::BlockNumber lastReceivedBlockNumber;
};

}
}

#endif
