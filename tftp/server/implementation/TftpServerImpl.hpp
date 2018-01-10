/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::Server::TftpServerImpl.
 **/

#ifndef TFTP_SERVER_TFTPSERVERIMPL_HPP
#define TFTP_SERVER_TFTPSERVERIMPL_HPP

#include <tftp/server/Server.hpp>
#include <tftp/server/implementation/TftpServerInternal.hpp>

#include <tftp/options/OptionList.hpp>

#include <tftp/PacketHandler.hpp>

#include <boost/asio.hpp>

#include <string>
#include <map>

namespace Tftp {
namespace Server {

/**
 * @brief TFTP server implementation.
 *
 * Waits on the specified port for a valid TFTP request and calls the
 * approbate call-back, which has to handle the request.
 *
 * If not expected packets or invalid packets are received a error is send
 * back to the sender.
 *
 * Valid requests are TFTP Read Request (RRQ) and TFTP Write Request (WRQ)
 **/
class TftpServerImpl:
  public TftpServerInternal,
  private PacketHandler
{
  public:
    /**
     * @brief Creates an instance of the TFTP server.
     *
     * @param[in] handler
     *   The TFTP request received handler.
     * @param[in] configuration
     *   The TFTP Configuration
     * @param[in] additionalOptions
     *   Additional Options, which shall be used as TFTP server option list.
     * @param[in] serverAddress
     *   Address where the FTP server should listen on.
     *
     * @throw TftpException
     *   When a error occurs during socket initialisation.
     **/
    TftpServerImpl(
      ReceivedTftpRequestHandler handler,
      const TftpConfiguration &configuration,
      const Options::OptionList& additionalOptions,
      const UdpAddressType &serverAddress);

    /**
     * @brief Destructor
     **/
    virtual ~TftpServerImpl() noexcept;

    //! @copydoc TftpServer::entry()
    void entry() noexcept final;

    //! @copydoc TftpServer::start()
    void start() final;

    //! @copydoc TftpServer::stop
    void stop() final;

    //! @copydoc TftpServer::createReadRequestOperation(TransmitDataHandlerPtr,OperationCompletedHandler,const UdpAddressType&,const Options::OptionList&,const UdpAddressType&)
    OperationPtr createReadRequestOperation(
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const UdpAddressType &clientAddress,
      const Options::OptionList &clientOptions,
      const UdpAddressType &serverAddress) final;

    //! @copydoc TftpServer::createReadRequestOperation(TransmitDataHandlerPtr,OperationCompletedHandler,const UdpAddressType&,const Options::OptionList&)
    OperationPtr createReadRequestOperation(
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const UdpAddressType &clientAddress,
      const Options::OptionList &clientOptions) final;

    //! @copydoc TftpServer::createWriteRequestOperation(ReceiveDataHandlerPtr,OperationCompletedHandler,const UdpAddressType&,const Options::OptionList&,const UdpAddressType&)
    OperationPtr createWriteRequestOperation(
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const UdpAddressType &clientAddress,
      const Options::OptionList &clientOptions,
      const UdpAddressType &serverAddress) final;

    //! @copydoc TftpServer::createWriteRequestOperation(ReceiveDataHandlerPtr,OperationCompletedHandler,const UdpAddressType&,const Options::OptionList&)
    OperationPtr createWriteRequestOperation(
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const UdpAddressType &clientAddress,
      const Options::OptionList &clientOptions) final;

    //! @copydoc TftpServer::createErrorOperation(const UdpAddressType&,const UdpAddressType&,ErrorCode,const string&,OperationCompletedHandler)
    OperationPtr createErrorOperation(
      const UdpAddressType &clientAddress,
      const UdpAddressType &from,
      ErrorCode errorCode,
      const string &errorMessage,
      OperationCompletedHandler completionHandler = {}) final;

    //! @copydoc TftpServer::createErrorOperation(const UdpAddressType&,const UdpAddressType&,ErrorCode,string&&,OperationCompletedHandler)
    OperationPtr createErrorOperation(
      const UdpAddressType &clientAddress,
      const UdpAddressType &from,
      ErrorCode errorCode,
      string &&errorMessage = {},
      OperationCompletedHandler completionHandler = {}) final;

    //! @copydoc TftpServer::createErrorOperation(const UdpAddressType&,ErrorCode,const string&,OperationCompletedHandler)
    OperationPtr createErrorOperation(
      const UdpAddressType &clientAddress,
      ErrorCode errorCode,
      const string &errorMessage,
      OperationCompletedHandler completionHandler = {}) final;

    //! @copydoc TftpServer::createErrorOperation(const UdpAddressType&,ErrorCode,string&&,OperationCompletedHandler)
    OperationPtr createErrorOperation(
      const UdpAddressType &clientAddress,
      ErrorCode errorCode,
      string &&errorMessage,
      OperationCompletedHandler completionHandler = {}) final;

    //! @copydoc TftpServerInternal::getConfiguration
    const TftpConfiguration& getConfiguration() const final;

    //! @copydoc TftpServerInternal::getOptionList
    const Options::OptionList& getOptionList() const final;

  private:
    /**
     * @brief Waits for an incoming response from the server.
     **/
    void receive();

    /**
     * @brief Called, when data is received.
     *
     * @param[in] errorCode
     *   error status of operation.
     * @param[in] bytesTransferred
     *   Number of bytes transfered.
     **/
    void receiveHandler(
      const boost::system::error_code& errorCode,
      std::size_t bytesTransferred);

    /**
     * @copydoc PacketHandler::handleReadRequestPacket
     *
     * The packet is decoded, and when valid the handler
     * NewRequestHandler::receviedReadRequest() is called, which actually
     * handles the request.
     **/
    virtual void handleReadRequestPacket(
      const UdpAddressType &from,
      const Packets::ReadRequestPacket &readRequestPacket) override final;

    /**
     * @copydoc PacketHandler::handleWriteRequestPacket
     *
     * The packet is decoded, and when valid the handler
     * NewRequestHandler::receviedWriteRequest() is called, which actually
     * handles the request.
     **/
    virtual void handleWriteRequestPacket(
      const UdpAddressType &from,
      const Packets::WriteRequestPacket &writeRequestPacket) override final;

    /**
     * @copydoc PacketHandler::handleDataPacket
     *
     * The TFTP server does not expect this packet.
     * This packet is responded with an TFTP Error Packet.
     **/
    virtual void handleDataPacket(
      const UdpAddressType &from,
      const Packets::DataPacket &dataPacket) override final;

    /**
     * @copydoc PacketHandler::handleAcknowledgementPacket
     *
     * The TFTP server does not expect this packet.
     * This packet is responded with an TFTP Error Packet.
     **/
    virtual void handleAcknowledgementPacket(
      const UdpAddressType &from,
      const Packets::AcknowledgementPacket &acknowledgementPacket) override final;

    /**
     * @copydoc PacketHandler::handleErrorPacket
     *
     * The TFTP server does not expect this packet.
     * This packet is responded with an TFTP Error Packet.
     **/
    virtual void handleErrorPacket(
      const UdpAddressType &from,
      const Packets::ErrorPacket &errorPacket) override final;

    /**
     * @copydoc PacketHandler::handleOptionsAcknowledgementPacket
     *
     * The TFTP server does not expect this packet. This packet is responded
     * with an TFTP Error Packet.
     **/
    virtual void handleOptionsAcknowledgementPacket(
      const UdpAddressType &from,
      const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket) override final;

    /**
     * @copydoc PacketHandler::handleInvalidPacket
     *
     * The TFTP server does not expect this packet.
     * This packet is ignored.
     **/
    virtual void handleInvalidPacket(
      const UdpAddressType &from,
      const RawTftpPacketType &rawPacket) override final;

  private:
    //! The registered handler
    ReceivedTftpRequestHandler handler;
    //! The TFTP configuration
    const TftpConfiguration configuration;
    //! The Server option list.
    const Options::OptionList options;
    //! the server address to listen on
    const UdpAddressType serverAddress;

    //! TFTP server ASIO service
    boost::asio::io_service ioService;
    //! TFTP server dummy work to prevent IO-Service from exiting.
    boost::asio::io_service::work work;
    //! TFTP well known socket
    boost::asio::ip::udp::socket socket;

    //! Buffer, which holds the received TFTP packet.
    RawTftpPacketType packet;
    //! The remote endpoint on receive.
    UdpAddressType remoteEndpoint;
};

}
}

#endif
