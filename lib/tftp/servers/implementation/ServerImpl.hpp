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
 * @brief Declaration of Class Tftp::Servers::ServerImpl.
 **/

#ifndef TFTP_SERVERS_SERVERIMPL_HPP
#define TFTP_SERVERS_SERVERIMPL_HPP

#include <tftp/servers/Servers.hpp>
#include <tftp/servers/Server.hpp>

#include <tftp/packets/PacketHandler.hpp>

#include <tftp/TftpOptionsConfiguration.hpp>

#include <boost/asio.hpp>

#include <map>
#include <string>

namespace Tftp::Servers {

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
class ServerImpl final :
  public Server,
  private Packets::PacketHandler
{
  public:
    /**
     * @brief Creates an Instance of the TFTP %Server.
     *
     * @param[in] ioContext
     *   I/O context used for Communication.
     *
     * @throw CommunicationException
     *   When an error occurs during socket initialisation.
     **/
    explicit ServerImpl( boost::asio::io_context &ioContext );

    /**
     * @brief Destructor
     **/
    ~ServerImpl() override;

    //! @copydoc Server::requestHandler()
    Server& requestHandler( ReceivedTftpRequestHandler handler ) override;

    //! @copydoc Server::serverAddress()
    Server& serverAddress(
      boost::asio::ip::udp::endpoint serverAddress ) override;

    //! @copydoc Server::localEndpoint()
    [[nodiscard]] boost::asio::ip::udp::endpoint localEndpoint() const override;

    //! @copydoc Server::tftpTimeoutDefault()
    Server& tftpTimeoutDefault( std::chrono::seconds timeout ) override;

    //! @copydoc Server::tftpRetriesDefault()
    Server& tftpRetriesDefault( uint16_t retries ) override;

    //! @copydoc Server::dallyDefault()
    Server& dallyDefault( bool dally ) override;

    //! @copydoc Server::optionsConfigurationDefault()
    Server& optionsConfigurationDefault(
      TftpOptionsConfiguration optionsConfiguration ) override;

    //! @copydoc Server::localDefault()
    Server& localDefault( boost::asio::ip::address local ) override;

    //! @copydoc Server::start()
    void start() override;

    //! @copydoc Server::stop()
    void stop() override;

    //! @copydoc Server::readOperation()
    [[nodiscard]] ReadOperationPtr readOperation() override;

    //! @copydoc Server::writeOperation()
    [[nodiscard]] WriteOperationPtr writeOperation() override;

    //! @copydoc Server::errorOperation(const boost::asio::ip::udp::endpoint&,Packets::ErrorCode,std::string)
    void errorOperation(
      const boost::asio::ip::udp::endpoint &remote,
      Packets::ErrorCode errorCode,
      std::string errorMessage = {} ) override;

    //! @copydoc Server::errorOperation(const boost::asio::ip::udp::endpoint&,const boost::asio::ip::udp::endpoint&,Packets::ErrorCode,std::string)
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

    //! TFTP Request Received Handler
    ReceivedTftpRequestHandler requestHandlerV;
    //! Address where the TFTP server should listen on.
    boost::asio::ip::udp::endpoint serverAddressV{ DefaultLocalEndpoint };

    //! TFTP Server I/O context
    boost::asio::io_context &ioContextV;
    //! TFTP well known socket
    boost::asio::ip::udp::socket socketV;

    //! Default timeout for TFTP operations
    std::optional< std::chrono::seconds > tftpTimeoutDefaultV;
    //! Default number of retries for TFTP operations
    std::optional< uint16_t > tftpRetriesDefaultV;
    //! Default value for the DALLY option
    std::optional< bool > dallyDefaultV;
    //! Default value for the options configuration
    std::optional< TftpOptionsConfiguration > optionsConfigurationDefaultV;
    //! Additional options
    Packets::Options additionalOptionsV;
    //! Default local IP address
    boost::asio::ip::address localV;

    //! Buffer, which holds the received TFTP packet.
    Packets::RawTftpPacket receivePacketV;
    //! Remote endpoint on receive.
    boost::asio::ip::udp::endpoint remoteEndpointV;
};

}

#endif
