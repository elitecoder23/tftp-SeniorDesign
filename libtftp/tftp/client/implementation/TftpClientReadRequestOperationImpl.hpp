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
 * @brief Declaration of class Tftp::Client::TftpClientReadRequestOperationImpl.
 **/

#ifndef TFTP_CLIENT_TFTPCLIENTREADREQUESTOPERATIONIMPL_HPP
#define TFTP_CLIENT_TFTPCLIENTREADREQUESTOPERATIONIMPL_HPP

#include <tftp/client/Client.hpp>
#include <tftp/client/implementation/TftpClientOperationImpl.hpp>
#include <tftp/packet/BlockNumber.hpp>

namespace Tftp {
namespace Client {

/**
 * @brief Class which handles a TFTP Read Request on client side.
 *
 * After executed, the class sends the TFTP RRQ packet to the destination and
 * waits for answer.
 * Received data is handled by the TftpReadOperationHandler given at
 * construction time.
 **/
class TftpClientReadRequestOperationImpl : public TftpClientOperationImpl
{
  public:
    /**
     * @brief Constructor of TftpClientReadOperation
     *
     * @param[in] handler
     *   Handler for received data.
     * @param[in] tftpClient
     *   The TFTP client.
     * @param[in] serverAddress
     *   Where the connection should be established to.
     * @param[in] filename
     *   Which file shall be requested
     * @param[in] mode
     *   The transfer mode
     * @param[in] from
     *   communication source
     **/
    TftpClientReadRequestOperationImpl(
      ReceiveDataOperationHandler &handler,
      const TftpClientInternal &tftpClient,
      const UdpAddressType &serverAddress,
      const string &filename,
      TransferMode mode,
      const UdpAddressType &from);

    /**
     * @brief Constructor of TftpClientReadOperation
     *
     * @param[in] handler
     *   Handler for received data.
     * @param[in] tftpClient
     *   The TFTP client.
     * @param[in] serverAddress
     *   Where the connection should be established to.
     * @param[in] filename
     *   Which file shall be requested
     * @param[in] mode
     *   The transfer mode
     **/
    TftpClientReadRequestOperationImpl(
      ReceiveDataOperationHandler &handler,
      const TftpClientInternal &tftpClient,
      const UdpAddressType &serverAddress,
      const string &filename,
      TransferMode mode);

    /**
     * @copybrief TftpClientOperationImpl::operator()()
     *
     * Assembles and transmit TFTP RRQ packet and start parent receive loop.
     **/
    virtual void operator()() override final;

  protected:
    /**
     * @copydoc TftpPacketHandler::handleDataPacket()
     *
     * The TFTP DATA packet is decoded and checked.
     * If everything is fine, handler is called with extracted data and the
     * receive operation is continued.
     **/
    virtual void handleDataPacket(
      const UdpAddressType &from,
      const DataPacket &dataPacket) override final;

    /**
     * @copydoc TftpPacketHandler::handleAcknowledgementPacket()
     *
     * ACK packets are not expected for this operation.
     * They are rejected by error transmission
     **/
    virtual void handleAcknowledgementPacket(
      const UdpAddressType &from,
      const AcknowledgementPacket &acknowledgementPacket) override final;

    /**
     * @copydoc TftpPacketHandler::handleOptionsAcknowledgementPacket()
     *
     **/
    virtual void handleOptionsAcknowledgementPacket(
      const UdpAddressType &from,
      const OptionsAcknowledgementPacket &optionsAcknowledgementPacket) override final;

  private:
    using BlockNumber = Tftp::Packet::BlockNumber;

    //! Registered handler.
    ReceiveDataOperationHandler &handler;
    //! Size of the data-section in the TFTP DATA packet - changed during option negotiation.
    uint16_t receiveDataSize;
    //! last received block number.
    BlockNumber lastReceivedBlockNumber;
};

}
}

#endif
