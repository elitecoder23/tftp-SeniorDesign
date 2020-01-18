/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Client::ReadRequestOperationImpl.
 **/

#ifndef TFTP_CLIENT_READREQUESTOPERATIONIMPL_HPP
#define TFTP_CLIENT_READREQUESTOPERATIONIMPL_HPP

#include <tftp/client/Client.hpp>
#include <tftp/client/implementation/OperationImpl.hpp>
#include <tftp/packets/BlockNumber.hpp>

namespace Tftp::Client {

/**
 * @brief Class which handles a TFTP Read Request on client side.
 *
 * After executed, the class sends the TFTP RRQ packet to the destination and
 * waits for answer.
 * Received data is handled by the TftpReadOperationHandler given at
 * construction time.
 **/
class ReadRequestOperationImpl : public OperationImpl
{
  public:
    /**
     * @brief Initialises the operation.
     *
     * @param[in] ioContext
     *   The I/O context used for communication.
     * @param[in] tftpTimeout
     *   TFTP Timeout, when no timeout option is negotiated in seconds.
     * @param[in] tftpRetries
     *   Number of retries.
     * @param[in] optionNegotiationHandler
     *   Option negotiation handler.
     * @param[in] dataHandler
     *   Handler for received data.
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
     * @param[in] remote
     *   Where the connection should be established to.
     * @param[in] filename
     *   Which file shall be requested
     * @param[in] mode
     *   The transfer mode
     * @param[in] clientOptions
     *   Client TFTP options used for option negotiation.
     **/
    ReadRequestOperationImpl(
      boost::asio::io_context &ioContext,
      uint8_t tftpTimeout,
      uint16_t tftpRetries,
      bool handleTransferSizeOption,
      OptionNegotiationHandler optionNegotiationHandler,
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      TransferMode mode,
      const Options::OptionList &clientOptions);

    /**
     * @copydoc ReadRequestOperationImpl(boost::asio::io_context&,OptionNegotiationHandler,ReceiveDataHandlerPtr,OperationCompletedHandler,const TftpClientInternal&,const boost::asio::ip::udp::endpoint&,std::string_view,TransferMode,const Options::OptionList&)
     *
     * @param[in] local
     *   communication source
     **/
    ReadRequestOperationImpl(
      boost::asio::io_context &ioContext,
      uint8_t tftpTimeout,
      uint16_t tftpRetries,
      bool handleTransferSizeOption,
      OptionNegotiationHandler optionNegotiationHandler,
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      std::string_view filename,
      TransferMode mode,
      const Options::OptionList &clientOptions,
      const boost::asio::ip::udp::endpoint &local);

    /**
     * @copybrief OperationImpl::start()
     *
     * Assembles and transmit TFTP RRQ packet and start parent receive loop.
     **/
    void start() final;

  protected:
    //! @copydoc OperationImpl::finished
    void finished(
      TransferStatus status,
      ErrorInfo &&errorInfo = {}) noexcept final;

    /**
     * @copydoc PacketHandler::dataPacket()
     *
     * The TFTP DATA packet is decoded and checked.
     * If everything is fine, handler is called with extracted data and the
     * receive operation is continued.
     **/
    void dataPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::DataPacket &dataPacket) final;

    /**
     * @copydoc PacketHandler::acknowledgementPacket()
     *
     * ACK packets are not expected for this operation.
     * They are rejected by error transmission
     **/
    void acknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::AcknowledgementPacket &acknowledgementPacket) final;

    /**
     * @copydoc PacketHandler::optionsAcknowledgementPacket()
     *
     * Checks received Options for validity and finalises the option negotiation.
     **/
    void optionsAcknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket) final;

  private:
    //! Indicates if the TFTP Transfer Size Option shall be used
    const bool handleTransferSizeOption;
    //! Option Negotiation Handler
    OptionNegotiationHandler optionNegotiationHandler;
    //! Registered handler.
    ReceiveDataHandlerPtr dataHandler;

    //! Filename of the transfer
    const std::string filename;
    //! Transfer mode (OCTETT/ NETASCII/ MAIL/ ...)
    const TransferMode mode;
    //! Options for the transfer
    Options::OptionList clientOptions;

    //! flag to hold information if OACK has been received (used when first data packet is received)
    bool oackReceived;
    //! Size of the data-section in the TFTP DATA packet - changed during option negotiation.
    uint16_t receiveDataSize;
    //! last received block number.
    Packets::BlockNumber lastReceivedBlockNumber;
};

}

#endif
