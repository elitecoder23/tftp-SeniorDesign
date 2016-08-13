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
 * @brief Declaration of class Tftp::Server::TftpServerReadRequestOperationImpl.
 **/

#ifndef TFTP_SERVER_TFTPSERVERWRITEOPERATION_HPP
#define TFTP_SERVER_TFTPSERVERWRITEOPERATION_HPP

#include <tftp/Tftp.hpp>
#include <tftp/server/implementation/TftpServerOperationImpl.hpp>
#include <tftp/packet/BlockNumber.hpp>

#include <boost/asio.hpp>

#include <string>

namespace Tftp {
namespace Server {

/**
 * @brief TFTP server write operation.
 *
 * In this operation a client has requested to read a file, which is
 * transmitted form the server to the client. Therefore the server performs
 * a write operation.
 *
 * This operation is initiated by a client TFTP read request (RRQ)
 **/
class TftpServerReadRequestOperationImpl: public TftpServerOperationImpl
{
  public:
    /**
     * @brief Initialises the TFTP server write operation instance.
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
    TftpServerReadRequestOperationImpl(
      TftpTransmitDataOperationHandler &handler,
      const TftpServerInternal &tftpServerInternal,
      const UdpAddressType &clientAddress,
      const OptionList &clientOptions,
      const UdpAddressType &serverAddress);

    /**
     * @brief Initialises the TFTP server write operation instance.
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
    TftpServerReadRequestOperationImpl(
      TftpTransmitDataOperationHandler &handler,
      const TftpServerInternal &tftpServerInternal,
      const UdpAddressType &clientAddress,
      const OptionList &clientOptions);

    //! Desctructor
    virtual ~TftpServerReadRequestOperationImpl( void) noexcept = default;

    /**
     * @brief executes the operation.
     **/
    virtual void operator ()( void) override;

  private:
    /**
     * @brief Sends a data packet to the client.
     *
     * The Data packet is assembled by calling the registered handler
     * operation TftpWriteOperationHandler::sendData().
     * If the last data packet will be sent, the internal flag will be set
     * approbate.
     **/
    void sendData( void);

    /**
     * @copydoc TftpPacketHandler::handleDataPacket
     *
     * Data packets are not expected and handled as invalid.
     * An error is sent back and the operation is cancelled.
     **/
    virtual void handleDataPacket(
      const UdpAddressType &from,
      const DataPacket &dataPacket) override;

    /**
     * @copydoc TftpPacketHandler::handleAcknowledgementPacket
     *
     * The acknowledgement packet is checked and the next data sequence is
     * handled.
     **/
    virtual void handleAcknowledgementPacket(
      const UdpAddressType &from,
      const AcknowledgementPacket &acknowledgementPacket) override;

  private:
    using BlockNumber = Packet::BlockNumber;

    //! The handler which is called during operation.
    TftpTransmitDataOperationHandler &handler;
    //! contains the negotiated blocksize option.
    uint16_t transmitDataSize;
    //! indicates, if the last data packet has been transmitted (closing).
    bool lastDataPacketTransmitted;
    //! The stored last transmitted block number.
    BlockNumber lastTransmittedBlockNumber;
};

}
}

#endif
