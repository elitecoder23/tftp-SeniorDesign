/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Server::WriteRequestOperationImpl.
 **/

#ifndef TFTP_SERVER_WRITEREQUESTOPERATIONIMPL_HPP
#define TFTP_SERVER_WRITEREQUESTOPERATIONIMPL_HPP

#include <tftp/server/Server.hpp>

#include <tftp/server/implementation/OperationImpl.hpp>

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/BlockNumber.hpp>

#include <tftp/TftpOptionsConfiguration.hpp>

#include <boost/asio.hpp>

#include <string>

namespace Tftp::Server {

/**
 * @brief TFTP %Server Write Request %Operation (TFTP WRQ).
 *
 * In this operation a client has requested to write a file, which is
 * transmitted form the client to the server.
 *
 * This operation is initiated by a client TFTP write request (WRQ)
 **/
class WriteRequestOperationImpl : public OperationImpl
{
  public:
    /**
     * @brief Constructs the class.
     *
     * @param[in] ioContext
     *   The I/O context used for communication.
     * @param[in] tftpTimeout
     *   TFTP Timeout, when no timeout option is negotiated in seconds.
     * @param[in] tftpRetries
     *   Number of retries.
     * @param[in] dally
     *   If set to true, wait after transmission of the final ACK for potential
     *   retries.
     * @param[in] dataHandler
     *   Handler, which will be called on various events.
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
     * @param[in] remote
     *   Address of the remote endpoint (TFTP client).
     * @param[in] optionsConfiguration
     *   TFTP Options Configuration.
     * @param[in] clientOptions
     *   Server TFTP options used for operation.
     * @param[in] additionalNegotiatedOptions
     *   Additional Options, which have been already negotiated.
     **/
    WriteRequestOperationImpl(
      boost::asio::io_context &ioContext,
      uint8_t tftpTimeout,
      uint16_t tftpRetries,
      bool dally,
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const TftpOptionsConfiguration &optionsConfiguration,
      const Options &clientOptions,
      const Options &additionalNegotiatedOptions );

    /**
     * @copydoc WriteRequestOperationImpl(boost::asio::io_context&,uint8_t,uint16_t,bool,ReceiveDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,const TftpOptionsConfiguration&,const Options&,const Options&)
     *
     * @param[in] local
     *   local endpoint, where the server handles the request from.
     **/
    WriteRequestOperationImpl(
      boost::asio::io_context &ioContext,
      uint8_t tftpTimeout,
      uint16_t tftpRetries,
      bool dally,
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const TftpOptionsConfiguration &optionsConfiguration,
      const Options &clientOptions,
      const Options &additionalNegotiatedOptions,
      const boost::asio::ip::udp::endpoint &local );

    /**
     * @brief Standard destructor.
     **/
    ~WriteRequestOperationImpl() noexcept override = default;

    /**
     * @brief executes the operation.
     *
     * Sends response to read request and waits for answers.
     **/
    void start();

  private:
    //! @copydoc OperationImpl::finished()
    void finished(
      TransferStatus status,
      ErrorInfo &&errorInfo = {}) noexcept final;

    /**
     * @copydoc Packets::PacketHandler::dataPacket
     *
     * The received data packet is checked and the
     * TftpReadOperationHandler::receivedData() operation of the registered
     * handler is called.
     **/
    void dataPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::DataPacket &dataPacket ) final;

    /**
     * @copydoc Packets::PacketHandler::acknowledgementPacket
     *
     * Acknowledgement packets are not expected and handled as invalid.
     * An error is sent back, and the operation is cancelled.
     **/
    void acknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::AcknowledgementPacket &acknowledgementPacket ) final;

    //! Dally Option
    const bool dally;
    //! Handler which will be called on various events.
    ReceiveDataHandlerPtr dataHandler;
    //! TFTP Options Configuration
    TftpOptionsConfiguration optionsConfiguration;
    //! Options for the transfer
    Options clientOptions;
    //! Additional Negotiated Options
    Options additionalNegotiatedOptions;
    //! Size of the data-section in the TFTP DATA packet - changed during option negotiation.
    uint16_t receiveDataSize;
    //! Holds the last received block number.
    Packets::BlockNumber lastReceivedBlockNumber;
};

}

#endif
