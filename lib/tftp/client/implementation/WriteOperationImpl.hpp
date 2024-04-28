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
#include "tftp/client/WriteOperationConfiguration.hpp"
#include "tftp/client/implementation/OperationImpl.hpp"

#include "tftp/packets/BlockNumber.hpp"

#include "tftp/TftpOptionsConfiguration.hpp"

#include <chrono>

namespace Tftp::Client {

/**
 * @brief TFTP %Client Write %Operation (TFTP WRQ).
 *
 * After executed, the class sends the TFTP WRQ packet to the destination
 * and waits for answer.
 * Data is handled by the TransmitDataHandler given at construction time.
 **/
class WriteOperationImpl final : public OperationImpl
{
  public:
    /**
     * @brief Initialises the operation.
     *
     * @param[in] ioContext
     *   I/O context used for communication.
     * @param[in] configuration
     *   Write Operation Configuration.
     **/
    WriteOperationImpl(
      boost::asio::io_context &ioContext,
      WriteOperationConfiguration configuration );

    //! @copydoc OperationImpl::request
    void request() final;

  protected:
    //! @copydoc OperationImpl::finished
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

  private:
    //! Write Operation Configuration
    WriteOperationConfiguration configurationV;

    //! Size of the data-section in the TFTP DATA packet - changed during option negotiation.
    uint16_t transmitDataSize{ Packets::DefaultDataSize };
    //! Is set, when the last data packet has been transmitted
    bool lastDataPacketTransmitted{ false };
    //! Block number of the last transmitted data packet.
    Packets::BlockNumber lastTransmittedBlockNumber{ 0U };
    //! Stored last received block number.
    Packets::BlockNumber lastReceivedBlockNumber{ 0U };
    //! Transfer Size obtained from Data Handler
    std::optional< uint64_t > transferSize{};
};

}

#endif
