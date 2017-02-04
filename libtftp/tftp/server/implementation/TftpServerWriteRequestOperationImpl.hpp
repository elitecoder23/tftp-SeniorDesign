/*
 * $Date$
 * $Revision$
 */
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::Server::TftpServerWriteRequestOperationImpl.
 **/

#ifndef TFTP_SERVER_TFTPSERVERREADOPERATION_HPP
#define TFTP_SERVER_TFTPSERVERREADOPERATION_HPP

#include <tftp/server/Server.hpp>

#include <tftp/server/implementation/TftpServerOperationImpl.hpp>

#include <tftp/packet/Packet.hpp>
#include <tftp/packet/BlockNumber.hpp>

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
class TftpServerWriteRequestOperationImpl: public TftpServerOperationImpl
{
  public:
    /**
     * @brief Constructs the class.
     *
     * @param[in] handler
     *   Handler, which will be called on various events.
     * @param[in] tftpServerInternal
     *   The TFTP internal server.
     * @param[in] clientAddress
     *   Address of the remote endpoint (TFTP client).
     * @param[in] clientOptions
     *   Received option list from client.
     * @param[in] serverAddress
     *   local endpoint, where the server handles the request from.
     **/
    TftpServerWriteRequestOperationImpl(
      ReceiveDataOperationHandler &handler,
      const TftpServerInternal &tftpServerInternal,
      const UdpAddressType &clientAddress,
      const OptionList &clientOptions,
      const UdpAddressType &serverAddress);

    /**
     * @brief Constructs the class.
     *
     * @param[in] handler
     *   Handler, which will be called on various events.
     * @param[in] tftpServerInternal
     *   The TFTP internal server.
     * @param[in] clientAddress
     *   Address of the remote endpoint (TFTP client).
     * @param[in] clientOptions
     *   Received option list from client.
     **/
    TftpServerWriteRequestOperationImpl(
      ReceiveDataOperationHandler &handler,
      const TftpServerInternal &tftpServerInternal,
      const UdpAddressType &clientAddress,
      const OptionList &clientOptions);

    /**
     * @brief Standard destructor.
     **/
    virtual ~TftpServerWriteRequestOperationImpl( void) noexcept = default;

    /**
     * @brief Executes the operation.
     **/
    virtual void operator()() override;

  private:
    /**
     * @copydoc TftpPacketHandler::handleDataPacket
     *
     * The received data packet is checked and the
     * TftpReadOperationHandler::receviedData() operation of the registered
     * handler is called.
     **/
    virtual void handleDataPacket(
      const UdpAddressType &from,
      const DataPacket &dataPacket) override;

    /**
     * @copydoc TftpPacketHandler::handleAcknowledgementPacket
     *
     * Acknowledgement packets are not expected and handled as invalid.
     * An error is sent back and the operation is cancelled.
     **/
    virtual void handleAcknowledgementPacket(
      const UdpAddressType &from,
      const AcknowledgementPacket &acknowledgementPacket) override;

  private:
    using BlockNumber = Tftp::Packet::BlockNumber;

    //! Handler which will be called on various events.
    ReceiveDataOperationHandler &handler;
    //! Size of the data-section in the TFTP DATA packet - changed during option negotiation.
    uint16_t receiveDataSize;
    //! Holds the last received block number.
    BlockNumber lastReceivedBlockNumber;
};

}
}

#endif
