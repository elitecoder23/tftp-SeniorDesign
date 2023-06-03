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
 * @brief Declaration of Class Tftp::Client::ReadOperationImpl.
 **/

#ifndef TFTP_CLIENT_READOPERATIONIMPL_HPP
#define TFTP_CLIENT_READOPERATIONIMPL_HPP

#include <tftp/client/Client.hpp>
#include <tftp/client/ReadOperationConfiguration.hpp>
#include <tftp/client/implementation/OperationImpl.hpp>

#include <tftp/packets/BlockNumber.hpp>

#include <tftp/TftpOptionsConfiguration.hpp>

#include <chrono>

namespace Tftp::Client {

/**
 * @brief TFTP %Client Read %Operation (TFTP RRQ).
 *
 * After executed, the class sends the TFTP RRQ packet to the destination and
 * waits for answer.
 * Received data is handled by the ReceiveDataHandler given at construction
 * time.
 **/
class ReadOperationImpl : public OperationImpl
{
  public:
    /**
     * @brief Initialises the operation.
     *
     * @param[in] ioContext
     *   I/O context used for communication.
     * @param[in] configuration
     *   Read Operation Configuration.
     **/
    ReadOperationImpl(
      boost::asio::io_context &ioContext,
      ReadOperationConfiguration configuration );

    //! @copydoc OperationImpl::request
    void request() final;

  protected:
    //! @copydoc OperationImpl::finished
    void finished(
      TransferStatus status,
      ErrorInfo &&errorInfo = {} ) noexcept final;

    /**
     * @copydoc Packets::PacketHandler::dataPacket()
     *
     * The TFTP DATA packet is decoded and checked.
     * If everything is fine, handler is called with extracted data and the
     * reception operation is continued.
     **/
    void dataPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::DataPacket &dataPacket ) final;

    /**
     * @copydoc Packets::PacketHandler::acknowledgementPacket()
     *
     * ACK packets are not expected for this operation.
     * They are rejected by error transmission
     **/
    void acknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::AcknowledgementPacket &acknowledgementPacket ) final;

    /**
     * @copydoc Packets::PacketHandler::optionsAcknowledgementPacket()
     *
     * Checks received Options for validity and finalises the option negotiation.
     **/
    void optionsAcknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket ) final;

  private:
    //! Read Operation Configuration
    ReadOperationConfiguration configurationV;

    //! flag to hold information if OACK has been received (used when first data packet is received)
    bool oackReceived{ false };
    //! Size of the data-section in the TFTP DATA packet - changed during option negotiation.
    uint16_t receiveDataSize{ Packets::DefaultDataSize };
    //! last received block number.
    Packets::BlockNumber lastReceivedBlockNumber{ 0U };
};

}

#endif
