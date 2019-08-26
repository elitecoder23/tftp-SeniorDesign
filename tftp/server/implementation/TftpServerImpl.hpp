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
#include <tftp/server/implementation/TftpServerInternal.hpp>

#include <tftp/options/OptionList.hpp>

#include <tftp/PacketHandler.hpp>

#include <boost/asio.hpp>

#include <string>
#include <map>

namespace Tftp::Server {

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

    //! @copydoc TftpServer::readRequestOperation
    OperationPtr readRequestOperation(
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const Options::OptionList &clientOptions,
      const boost::asio::ip::udp::endpoint &local) final;

    //! @copydoc TftpServer::writeRequestOperation
    OperationPtr writeRequestOperation(
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const Options::OptionList &clientOptions,
      const boost::asio::ip::udp::endpoint &local) final;

    //! @copydoc TftpServer::errorOperation(OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,const boost::asio::ip::udp::endpoint&,ErrorCode,std::string_view)
    OperationPtr errorOperation(
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const boost::asio::ip::udp::endpoint &local,
      ErrorCode errorCode,
      std::string_view errorMessage) final;

    //! @copydoc TftpServer::errorOperation(OperationCompletedHandler,const boost::asio::ip::udp::endpoint&,const boost::asio::ip::udp::endpoint&,ErrorCode,std::string&&)
    OperationPtr errorOperation(
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const boost::asio::ip::udp::endpoint &local,
      ErrorCode errorCode,
      std::string &&errorMessage = {}) final;

    //! @copydoc TftpServerInternal::configuration
    [[nodiscard]] const TftpConfiguration& configuration() const final;

    //! @copydoc TftpServerInternal::options
    [[nodiscard]] const Options::OptionList& options() const final;

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
    //! The registered handler
    ReceivedTftpRequestHandler handler;
    //! The TFTP configuration
    const TftpConfiguration configurationV;
    //! The Server option list.
    const Options::OptionList optionsV;
    //! the server address to listen on
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
