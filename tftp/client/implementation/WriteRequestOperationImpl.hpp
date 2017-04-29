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
 * @brief Declaration of class Tftp::Client::WriteRequestOperationImpl.
 **/

#ifndef TFTP_CLIENT_WRITEREQUESTOPERATIONIMPL_HPP
#define TFTP_CLIENT_WRITEREQUESTOPERATIONIMPL_HPP

#include <tftp/client/Client.hpp>
#include <tftp/client/implementation/OperationImpl.hpp>
#include <tftp/packets/BlockNumber.hpp>

namespace Tftp {
namespace Client {

/**
 * @brief Class which handles a TFTP Write Request on client side.
 *
 * After executed, the class sends the TFTP WRQ packet to the destination
 * and waits for answer.
 * Data is handled by the TftpWriteOperationHandler given at construction time.
 **/
class WriteRequestOperationImpl : public OperationImpl
{
  public:
    /**
     * @brief Initialises the operation.
     *
     * @param[in] ioService
     *   The IO service used for communication.
     * @param[in] dataHandler
     *   Handler for data.
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
     * @param[in] tftpClient
     *   The TFTP client.
     * @param[in] serverAddress
     *   Where the connection should be established to.
     * @param[in] filename
     *   Which file shall be requested
     * @param[in] mode
     *   The transfer mode
     * @param[in] from
     *   Optional parameter to define the communication source
     **/
    WriteRequestOperationImpl(
      boost::asio::io_service &ioService,
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const TftpClientInternal &tftpClient,
      const UdpAddressType &serverAddress,
      const string &filename,
      TransferMode mode,
      const UdpAddressType &from);

    /**
     * @brief Initialises the operation.
     *
     * @param[in] ioService
     *   The IO service used for communication.
     * @param[in] dataHandler
     *   Handler for data.
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
     * @param[in] tftpClient
     *   The TFTP client.
     * @param[in] serverAddress
     *   Where the connection should be established to.
     * @param[in] filename
     *   Which file shall be requested
     * @param[in] mode
     *   The transfer mode
     **/
    WriteRequestOperationImpl(
      boost::asio::io_service &ioService,
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const TftpClientInternal &tftpClient,
      const UdpAddressType &serverAddress,
      const string &filename,
      TransferMode mode);

    /**
     * @copybrief OperationImpl::start()
     *
     * Assembles and transmit TFTP WRQ packet and start parent receive loop.
     **/
    virtual void start() override;

  protected:
    //! @copydoc OperationImpl::finished
    virtual void finished(
      TransferStatus status,
      ErrorInfo &&errorInfo = {}) noexcept override final;

    /**
     * @brief Sends the data to the host.
     *
     * This operation requests the data from the handler, generates the TFTP
     * DATA packet and sends them to the host.
     **/
    void sendData();

    /**
     * @copydoc PacketHandler::handleDataPacket()
     *
     * @throw InvalidPacketException
     *   Always, because an this packet is invalid.
     **/
    virtual void handleDataPacket(
      const UdpAddressType &from,
      const Packets::DataPacket &dataPacket) override final;

    /**
     * @copydoc PacketHandler::handleAcknowledgementPacket()
     *
     * @throw InvalidPacketException
     *   Invalid block number
     **/
    virtual void handleAcknowledgementPacket(
      const UdpAddressType &from,
      const Packets::AcknowledgementPacket &acknowledgementPacket) override final;

    /**
     * @copydoc PacketHandler::handleOptionsAcknowledgementPacket()
     *
     * @throw InvalidPacketException
     *   Empty option list
     * @throw OptionNegotiationException
     *   Option negotiation failed
     **/
    virtual void handleOptionsAcknowledgementPacket(
      const UdpAddressType &from,
      const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket) override final;

  private:
    using BlockNumber = Packets::BlockNumber;

    //! The handler, which is called
    TransmitDataHandlerPtr dataHandler;
    //! Size of the data-section in the TFTP DATA packet - changed during option negotiation.
    uint16_t transmitDataSize;
    //! Is set, when the last data packet has been transmitted
    bool lastDataPacketTransmitted;
    //! The block number of the last transmitted data packet.
    BlockNumber lastTransmittedBlockNumber;
};

}
}

#endif
