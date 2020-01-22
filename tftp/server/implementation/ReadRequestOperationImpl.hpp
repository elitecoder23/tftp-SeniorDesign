/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Server::ReadRequestOperationImpl.
 **/

#ifndef TFTP_SERVER_READREQUESTOPERATIONIMPL_HPP
#define TFTP_SERVER_READREQUESTOPERATIONIMPL_HPP

#include <tftp/Tftp.hpp>
#include <tftp/server/implementation/OperationImpl.hpp>
#include <tftp/packets/BlockNumber.hpp>

#include <boost/asio.hpp>

#include <string>

namespace Tftp::Server {

/**
 * @brief TFTP Server Write Operation (RRQ).
 *
 * In this operation a client has requested to read a file, which is
 * transmitted form the server to the client.
 * Therefore the server performs a write operation.
 *
 * This operation is initiated by a client TFTP read request (RRQ)
 **/
class ReadRequestOperationImpl: public OperationImpl
{
  public:
    /**
     * @brief Initialises the TFTP server write operation instance.
     *
     * @param[in] ioContext
     *   The I/O context used for communication.
     * @param[in] tftpTimeout
     *   TFTP Timeout, when no timeout option is negotiated in seconds.
     * @param[in] tftpRetries
     *   Number of retries.
     * @param[in] dataHandler
     *   Handler, which will be called on various events.
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
     * @param[in] remote
     *   Address of the remote endpoint (TFTP client).
     * @param[in] negotiatedOptions
     *   Server TFTP options used for operation.
     **/
    ReadRequestOperationImpl(
      boost::asio::io_context &ioContext,
      uint8_t tftpTimeout,
      uint16_t tftpRetries,
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const Options::OptionList &negotiatedOptions);

    /**
     * @copydoc ReadRequestOperationImpl(boost::asio::io_context&,uint8_t,uint16_t,TransmitDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,const Options::OptionList&)
     *
     * @param[in] local
     *   local endpoint, where the server handles the request from.
     **/
    ReadRequestOperationImpl(
      boost::asio::io_context &ioContext,
      uint8_t tftpTimeout,
      uint16_t tftpRetries,
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const Options::OptionList &negotiatedOptions,
      const boost::asio::ip::udp::endpoint &local);

    //! Destructor
    ~ReadRequestOperationImpl() noexcept final = default;

    /**
     * @brief executes the operation.
     *
     * Sends response to read request and waits for asnwers.
     **/
    void start();

  private:
    //! @copydoc OperationImpl::finished()
    void finished(
      TransferStatus status,
      ErrorInfo &&errorInfo = {}) noexcept final;

    /**
     * @brief Sends a data packet to the client.
     *
     * The Data packet is assembled by calling the registered handler
     * operation TftpWriteOperationHandler::sendData().
     * If the last data packet will be sent, the internal flag will be set
     * approbate.
     **/
    void sendData();

    /**
     * @copydoc PacketHandler::dataPacket
     *
     * Data packets are not expected and handled as invalid.
     * An error is sent back and the operation is cancelled.
     **/
    void dataPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::DataPacket &dataPacket) final;

    /**
     * @copydoc PacketHandler::acknowledgementPacket
     *
     * The acknowledgement packet is checked and the next data sequence is
     * handled.
     **/
    void acknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::AcknowledgementPacket &acknowledgementPacket) final;

  private:
    //! The handler which is called during operation.
    TransmitDataHandlerPtr dataHandler;
    //! Options for the transfer
    Options::OptionList negotiatedOptions;
    //! contains the negotiated blocksize option.
    uint16_t transmitDataSize;
    //! indicates, if the last data packet has been transmitted (closing).
    bool lastDataPacketTransmitted;
    //! The stored last transmitted block number.
    Packets::BlockNumber lastTransmittedBlockNumber;
};

}

#endif
