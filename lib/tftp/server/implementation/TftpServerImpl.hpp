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
 * @brief Declaration of Class Tftp::Server::TftpServerImpl.
 **/

#ifndef TFTP_SERVER_TFTPSERVERIMPL_HPP
#define TFTP_SERVER_TFTPSERVERIMPL_HPP

#include <tftp/server/Server.hpp>
#include <tftp/server/TftpServer.hpp>
#include <tftp/server/ServerConfiguration.hpp>

#include <tftp/packets/PacketHandler.hpp>

#include <boost/asio.hpp>

#include <string>
#include <map>

namespace Tftp::Server {

/**
 * @brief TFTP %Server.
 *
 * Waits on the specified port for a valid TFTP request and calls the
 * appropriate call-back, which has to handle the request.
 *
 * If not expected packets or invalid packets are received a error is send
 * back to the sender.
 *
 * Valid requests are TFTP Read Request (RRQ) and TFTP Write Request (WRQ)
 **/
class TftpServerImpl final :
  public TftpServer,
  private Packets::PacketHandler
{
  public:
    /**
     * @brief Creates an instance of the TFTP Server.
     *
     * @param[in] ioContext
     *   I/O context used for Communication.
     * @param[in] configuration
     *   TFTP Server Configuration
     *
     * @throw CommunicationException
     *   When an error occurs during socket initialisation.
     **/
    TftpServerImpl(
      boost::asio::io_context &ioContext,
      ServerConfiguration configuration );

    /**
     * @brief Destructor
     **/
    ~TftpServerImpl() noexcept override;

    //! @copydoc TftpServer::localEndpoint()
    [[nodiscard]] boost::asio::ip::udp::endpoint localEndpoint() const override;

    //! @copydoc TftpServer::start()
    void start() override;

    //! @copydoc TftpServer::stop()
    void stop() override;

    //! @copydoc TftpServer::readOperation()
    [[nodiscard]] OperationPtr readOperation(
      ReadOperationConfiguration configuration ) override;

    //! @copydoc TftpServer::writeOperation()
    [[nodiscard]] OperationPtr writeOperation(
      WriteOperationConfiguration configuration ) override;

    //! @copydoc TftpServer::errorOperation(const boost::asio::ip::udp::endpoint&,Packets::ErrorCode,std::string)
    void errorOperation(
      const boost::asio::ip::udp::endpoint &remote,
      Packets::ErrorCode errorCode,
      std::string errorMessage = {} ) override;

    //! @copydoc TftpServer::errorOperation(const boost::asio::ip::udp::endpoint&,const boost::asio::ip::udp::endpoint&,Packets::ErrorCode,std::string)
    void errorOperation(
      const boost::asio::ip::udp::endpoint &remote,
      const boost::asio::ip::udp::endpoint &local,
      Packets::ErrorCode errorCode,
      std::string errorMessage = {} ) override;

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
     *   Number of bytes transferred.
     *
     * @throw CommunicationException
     *   On communication failure.
     **/
    void receiveHandler(
      const boost::system::error_code& errorCode,
      std::size_t bytesTransferred );

    /**
     * @copydoc Packets::PacketHandler::readRequestPacket
     *
     * The packet is decoded, and when valid the handler
     * NewRequestHandler::receivedReadRequest() is called, which actually
     * handles the request.
     **/
    void readRequestPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::ReadRequestPacket &readRequestPacket ) override;

    /**
     * @copydoc Packets::PacketHandler::writeRequestPacket
     *
     * The packet is decoded, and when valid the handler
     * NewRequestHandler::receivedWriteRequest() is called, which actually
     * handles the request.
     **/
    void writeRequestPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::WriteRequestPacket &writeRequestPacket ) override;

    /**
     * @copydoc Packets::PacketHandler::dataPacket
     *
     * The TFTP server does not expect this packet.
     * This packet is responded with an TFTP Error Packet.
     **/
    void dataPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::DataPacket &dataPacket ) override;

    /**
     * @copydoc Packets::PacketHandler::acknowledgementPacket
     *
     * The TFTP server does not expect this packet.
     * This packet is responded with an TFTP Error Packet.
     **/
    void acknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::AcknowledgementPacket &acknowledgementPacket ) override;

    /**
     * @copydoc Packets::PacketHandler::errorPacket
     *
     * The TFTP server does not expect this packet.
     * This packet is responded with an TFTP Error Packet.
     **/
    void errorPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::ErrorPacket &errorPacket ) override;

    /**
     * @copydoc Packets::PacketHandler::optionsAcknowledgementPacket
     *
     * The TFTP server does not expect this packet. This packet is responded
     * with an TFTP Error Packet.
     **/
    void optionsAcknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket ) override;

    /**
     * @copydoc Packets::PacketHandler::invalidPacket
     *
     * The TFTP server does not expect this packet.
     * This packet is ignored.
     **/
    void invalidPacket(
      const boost::asio::ip::udp::endpoint &remote,
      Packets::ConstRawTftpPacketSpan rawPacket ) override;

    /**
     * @brief Decodes the TFTP Options.
     *
     * @param[in,out] clientOptions
     *   Received TFTP Options.
     *   Will remove all options decoded by this operation.
     *
     * @return Decoded TFTP Options
     **/
    Packets::TftpOptions tftpOptions( Packets::Options &clientOptions ) const;

    //! TFTP Server I/O context
    boost::asio::io_context &ioContext;
    //! TFTP well known socket
    boost::asio::ip::udp::socket socket;

    //! TFTP Client Configuration
    ServerConfiguration configurationV;

    //! Buffer, which holds the received TFTP packet.
    Packets::RawTftpPacket receivePacket;
    //! Remote endpoint on receive.
    boost::asio::ip::udp::endpoint remoteEndpoint;
};

}

#endif
