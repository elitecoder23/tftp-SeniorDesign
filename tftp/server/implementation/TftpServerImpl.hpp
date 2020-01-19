/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Server::TftpServerImpl.
 **/

#ifndef TFTP_SERVER_TFTPSERVERIMPL_HPP
#define TFTP_SERVER_TFTPSERVERIMPL_HPP

#include <tftp/server/Server.hpp>
#include <tftp/server/TftpServer.hpp>

#include <tftp/options/OptionList.hpp>

#include <tftp/PacketHandler.hpp>

#include <boost/asio.hpp>

#include <string>
#include <map>

namespace Tftp::Server {

/**
 * @brief TFTP Server Implementation.
 *
 * Waits on the specified port for a valid TFTP request and calls the
 * appropriate call-back, which has to handle the request.
 *
 * If not expected packets or invalid packets are received a error is send
 * back to the sender.
 *
 * Valid requests are TFTP Read Request (RRQ) and TFTP Write Request (WRQ)
 **/
class TftpServerImpl:
  public TftpServer,
  private PacketHandler
{
  public:
    /**
     * @brief Creates an instance of the TFTP Server.
     *
     * @param[in] handler
     *   The TFTP request received handler.
     * @param[in] tftpTimeout
     *   TFTP Timeout, when no timeout option is negotiated in seconds.
     * @param[in] tftpRetries
     *   Number of retries.
     * @param[in] serverAddress
     *   Address where the FTP server should listen on.
     *
     * @throw CommunicationException
     *   When a error occurs during socket initialisation.
     **/
    TftpServerImpl(
      ReceivedTftpRequestHandler handler,
      uint8_t tftpTimeout,
      uint16_t tftpRetries,
      const boost::asio::ip::udp::endpoint &serverAddress);

    /**
     * @brief Destructor
     **/
    ~TftpServerImpl() noexcept override;

    //! @copydoc TftpServer::entry()
    void entry() noexcept final;

    //! @copydoc TftpServer::start()
    void start() final;

    //! @copydoc TftpServer::stop
    void stop() final;

    //! @copydoc TftpServer::readRequestOperation(TransmitDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,const Options::OptionList&)
    OperationPtr readRequestOperation(
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const Options::OptionList &negotiatedOptions) final;

    //! @copydoc TftpServer::readRequestOperation(TransmitDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,const Options::OptionList&,const boost::asio::ip::udp::endpoint&)
    OperationPtr readRequestOperation(
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const Options::OptionList& negotiatedOptions,
      const boost::asio::ip::udp::endpoint &local) final;

    //! @copydoc TftpServer::writeRequestOperation(ReceiveDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,const Options::OptionList&)
    OperationPtr writeRequestOperation(
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const Options::OptionList &negotiatedOptions) final;

    //! @copydoc TftpServer::writeRequestOperation(ReceiveDataHandlerPtr,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,const Options::OptionList&,const boost::asio::ip::udp::endpoint&)
    OperationPtr writeRequestOperation(
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const Options::OptionList& negotiatedOptions,
      const boost::asio::ip::udp::endpoint &local) final;

    //! @copydoc TftpServer::errorOperation(const boost::asio::ip::udp::endpoint&,ErrorCode,std::string_view)
    void errorOperation(
      const boost::asio::ip::udp::endpoint &remote,
      ErrorCode errorCode,
      std::string_view errorMessage = {}) final;

    //! @copydoc TftpServer::errorOperation(const boost::asio::ip::udp::endpoint&,const boost::asio::ip::udp::endpoint&,ErrorCode,std::string_view)
    void errorOperation(
      const boost::asio::ip::udp::endpoint &remote,
      const boost::asio::ip::udp::endpoint &local,
      ErrorCode errorCode,
      std::string_view errorMessage = {}) final;

  private:
    /**
     * @brief Waits for an incoming response from the server.
     *
     * @throw CommunicationException
     *   On IO error.
     **/
    void receive();

    /**
     * @brief Called, when data is received.
     *
     * @param[in] errorCode
     *   error status of operation.
     * @param[in] bytesTransferred
     *   Number of bytes transfered.
     *
     * @throw CommunicationException
     *   On communication failure.
     **/
    void receiveHandler(
      const boost::system::error_code& errorCode,
      std::size_t bytesTransferred);

    /**
     * @copydoc PacketHandler::readRequestPacket
     *
     * The packet is decoded, and when valid the handler
     * NewRequestHandler::receviedReadRequest() is called, which actually
     * handles the request.
     **/
    void readRequestPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::ReadRequestPacket &readRequestPacket) final;

    /**
     * @copydoc PacketHandler::writeRequestPacket
     *
     * The packet is decoded, and when valid the handler
     * NewRequestHandler::receviedWriteRequest() is called, which actually
     * handles the request.
     **/
    void writeRequestPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::WriteRequestPacket &writeRequestPacket) final;

    /**
     * @copydoc PacketHandler::dataPacket
     *
     * The TFTP server does not expect this packet.
     * This packet is responded with an TFTP Error Packet.
     **/
    void dataPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::DataPacket &dataPacket) final;

    /**
     * @copydoc PacketHandler::acknowledgementPacket
     *
     * The TFTP server does not expect this packet.
     * This packet is responded with an TFTP Error Packet.
     **/
    void acknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::AcknowledgementPacket &acknowledgementPacket) final;

    /**
     * @copydoc PacketHandler::errorPacket
     *
     * The TFTP server does not expect this packet.
     * This packet is responded with an TFTP Error Packet.
     **/
    void errorPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::ErrorPacket &errorPacket) final;

    /**
     * @copydoc PacketHandler::optionsAcknowledgementPacket
     *
     * The TFTP server does not expect this packet. This packet is responded
     * with an TFTP Error Packet.
     **/
    void optionsAcknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket) final;

    /**
     * @copydoc PacketHandler::invalidPacket
     *
     * The TFTP server does not expect this packet.
     * This packet is ignored.
     **/
    void invalidPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const RawTftpPacket &rawPacket) final;

  private:
    //! Registered request handler
    ReceivedTftpRequestHandler handler;
    //! TFTP Receive Timeout
    const uint8_t tftpTimeout;
    //! TFTP Retries
    const uint16_t tftpRetries;
    //! Server address to listen on
    const boost::asio::ip::udp::endpoint serverAddress;

    //! TFTP server ASIO context
    boost::asio::io_context ioContext;
    //! TFTP server dummy work to prevent I/O context from exiting.
    boost::asio::io_context::work work;
    //! TFTP well known socket
    boost::asio::ip::udp::socket socket;

    //! Buffer, which holds the received TFTP packet.
    RawTftpPacket receivePacket;
    //! The remote endpoint on receive.
    boost::asio::ip::udp::endpoint remoteEndpoint;
};

}

#endif
