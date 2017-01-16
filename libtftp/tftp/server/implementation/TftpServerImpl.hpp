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
 * @brief Declaration of class Tftp::Server::TftpServerImpl.
 **/

#ifndef TFTP_SERVER_TFTPSERVERIMPL_HPP
#define TFTP_SERVER_TFTPSERVERIMPL_HPP

#include <tftp/server/Server.hpp>
#include <tftp/server/TftpServer.hpp>
#include <tftp/server/implementation/TftpServerInternal.hpp>

#include <tftp/options/OptionList.hpp>

#include <tftp/TftpPacketHandler.hpp>

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
  public TftpServer,
  public TftpServerInternal,
  private TftpPacketHandler
{
  public:
  using TftpServer::OptionList;
    /**
     * @brief Creates an instance of the TFTP server.
     *
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
      const TftpConfiguration &configuration,
      const OptionList& additionalOptions,
      const UdpAddressType &serverAddress);

    /**
     * @brief Destructor
     **/
    virtual ~TftpServerImpl() noexcept;

    virtual void registerRequestHandler(
      ReceivedTftpRequestHandler handler) override final;

    /**
     * @brief Starts the TFTP Server.
     *
     * This routines starts the server loop, which waits for incoming
     * requests and handles them.
     *
     * The start routine will be leaved, when an FATAL error occurred or
     * the server has been stopped by calling stop().
     **/
    virtual void start() override;

    /**
     * @brief Stops the TFTP Server.
     **/
    virtual void stop() override;

    //! @copydoc TftpServer::createReadRequestOperation(TftpTransmitDataOperationHandler&,const UdpAddressType&,const OptionList&,const UdpAddressType&)
    virtual TftpServerOperation createReadRequestOperation(
      TftpTransmitDataOperationHandler &handler,
      const UdpAddressType &clientAddress,
      const OptionList &clientOptions,
      const UdpAddressType &serverAddress) override final;

    //! @copydoc TftpServer::createReadRequestOperation(TftpTransmitDataOperationHandler&,const UdpAddressType&,const OptionList&)
    virtual TftpServerOperation createReadRequestOperation(
      TftpTransmitDataOperationHandler &handler,
      const UdpAddressType &clientAddress,
      const OptionList &clientOptions) override final;

    //! @copydoc TftpServer::createWriteRequestOperation(TftpReceiveDataOperationHandler&,const UdpAddressType&,const OptionList&,const UdpAddressType&)
    virtual TftpServerOperation createWriteRequestOperation(
      TftpReceiveDataOperationHandler &handler,
      const UdpAddressType &clientAddress,
      const OptionList &clientOptions,
      const UdpAddressType &serverAddress) override final;

    //! @copydoc TftpServer::createWriteRequestOperation(TftpReceiveDataOperationHandler&,const UdpAddressType&,const OptionList&)
    virtual TftpServerOperation createWriteRequestOperation(
      TftpReceiveDataOperationHandler &handler,
      const UdpAddressType &clientAddress,
      const OptionList &clientOptions) override final;

    //! @copydoc TftpServer::createErrorOperation(const UdpAddressType&,const UdpAddressType&,const ErrorCode,const string&)
    virtual TftpServerOperation createErrorOperation(
      const UdpAddressType &clientAddress,
      const UdpAddressType &from,
      const ErrorCode errorCode,
      const string &errorMessage = string()) override final;

    //! @copydoc TftpServer::createErrorOperation(const UdpAddressType&,const ErrorCode,const string&)
    virtual TftpServerOperation createErrorOperation(
      const UdpAddressType &clientAddress,
      const ErrorCode errorCode,
      const string &errorMessage = string()) override final;

    //! @copydoc TftpServerInternal::getConfiguration
    virtual const TftpConfiguration& getConfiguration() const override final;

    //! @copydoc TftpServerInternal::getOptionList
    virtual const OptionList& getOptionList() const override final;

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
     * @copydoc TftpPacketHandler::handleReadRequestPacket
     *
     * The packet is decoded, and when valid the handler
     * NewRequestHandler::receviedReadRequest() is called, which actually
     * handles the request.
     **/
    virtual void handleReadRequestPacket(
      const UdpAddressType &from,
      const ReadRequestPacket &readRequestPacket) override final;

    /**
     * @copydoc TftpPacketHandler::handleWriteRequestPacket
     *
     * The packet is decoded, and when valid the handler
     * NewRequestHandler::receviedWriteRequest() is called, which actually
     * handles the request.
     **/
    virtual void handleWriteRequestPacket(
      const UdpAddressType &from,
      const WriteRequestPacket &writeRequestPacket) override final;

    /**
     * @copydoc TftpPacketHandler::handleDataPacket
     *
     * The TFTP server does not expect this packet.
     * This packet is responded with an TFTP Error Packet.
     **/
    virtual void handleDataPacket(
      const UdpAddressType &from,
      const DataPacket &dataPacket) override final;

    /**
     * @copydoc TftpPacketHandler::handleAcknowledgementPacket
     *
     * The TFTP server does not expect this packet.
     * This packet is responded with an TFTP Error Packet.
     **/
    virtual void handleAcknowledgementPacket(
      const UdpAddressType &from,
      const AcknowledgementPacket &acknowledgementPacket) override final;

    /**
     * @copydoc TftpPacketHandler::handleErrorPacket
     *
     * The TFTP server does not expect this packet.
     * This packet is responded with an TFTP Error Packet.
     **/
    virtual void handleErrorPacket(
      const UdpAddressType &from,
      const ErrorPacket &errorPacket) override final;

    /**
     * @copydoc TftpPacketHandler::handleOptionsAcknowledgementPacket
     *
     * The TFTP server does not expect this packet. This packet is responded
     * with an TFTP Error Packet.
     **/
    virtual void handleOptionsAcknowledgementPacket(
      const UdpAddressType &from,
      const OptionsAcknowledgementPacket &optionsAcknowledgementPacket) override final;

    /**
     * @copydoc TftpPacketHandler::handleInvalidPacket
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
    const OptionList options;
    //! the server address to listen on
    const UdpAddressType serverAddress;

    boost::asio::io_service ioService;
    boost::asio::ip::udp::socket socket;

    RawTftpPacketType packet;
    UdpAddressType remoteEndpoint;
};

}
}

#endif
