// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Clients::OperationImpl.
 **/

#ifndef TFTP_CLIENTS_OPERATIONIMPL_HPP
#define TFTP_CLIENTS_OPERATIONIMPL_HPP

#include <tftp/clients/Clients.hpp>
#include <tftp/clients/Client.hpp>

#include <tftp/packets/ErrorPacket.hpp>
#include <tftp/packets/PacketHandler.hpp>
#include <tftp/packets/Packets.hpp>

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/system_timer.hpp>

#include <chrono>
#include <memory>
#include <string>

namespace Tftp::Clients {

/**
 * @brief TFTP %Client %Operation.
 *
 * This class is specialised for the two kinds of TFTP operations (Read Operation, Write Operation).
 **/
class OperationImpl : protected Packets::PacketHandler
{
  protected:
    /**
     * @brief Initialises the TFTP Client Operation.
     *
     * @param[in] ioContext
     *   I/O context used for communication.
     **/
    explicit OperationImpl( boost::asio::io_context &ioContext );

    /**
     * @brief Destructor.
     **/
    ~OperationImpl() override;

    /**
     * @brief Initialises the Operation
     *
     * Creates the socket.
     **/
    void initialise();

    /**
     * @brief Aborts the Operation Gracefully.
     *
     * Sends an error packet at next possible time point.
     *
     * @param[in] errorCode
     *   The TFTP error code.
     * @param[in] errorMessage
     *   An additional error message.
     **/
    void gracefulAbort(
      Packets::ErrorCode errorCode,
      std::string errorMessage );

    /**
     * @brief Immediately Cancels the Transfer.
     **/
    void abort();

    /**
     * @brief Returns the error information.
     *
     * @return The error information
     * @retval ErrorInfo()
     *   If no error occurred.
     **/
    [[nodiscard]] const Packets::ErrorInfo& errorInfo() const;

    /**
     * @brief Updates TFTP Timeout.
     *
     * TFTP Timeout, when no timeout option is negotiated in seconds.
     *
     * @param[in] timeout
     *   TFTP timeout.
     **/
    void tftpTimeout( std::chrono::seconds timeout );

    /**
     * @brief Updates the NUmber of TFTP Packet Retries.
     *
     * @param[in] retries
     *   Number of TFTP Packet Retries.
     **/
    void tftpRetries( uint16_t retries );

    /**
     * @brief Updates the remote (server address)
     *
     * @param[in] remote
     *   Where the connection should be established to.
     **/
    void remote( boost::asio::ip::udp::endpoint remote );

    /**
     * @brief Updates the local address to use as connection source.
     *
     * @param[in] local
     *   Parameter to define the communication source
     **/
    void local( boost::asio::ip::udp::endpoint local );

    /**
     * @brief Updates the Operation Completed Handler
     *
     * @param[in] handler
     *   Handler which is called on completion of the operation.
     **/
    void completionHandler( OperationCompletedHandler handler );

    /**
     * @brief Updates the Maximum Receive Packet Size.
     *
     * This operation should be called, if a block size option has been
     * negotiated.
     *
     * @param[in] maxReceivePacketSize
     *   New max receive packet size.
     **/
    void maxReceivePacketSize( uint16_t maxReceivePacketSize );

    /**
     * @brief Update the Receive Timeout Value.
     *
     * This operation should be called, if a timout option has been negotiated.
     *
     * @param[in] receiveTimeout
     *   New receive timeout.
     **/
    void receiveTimeout( std::chrono::seconds receiveTimeout ) noexcept;

    /**
     * @brief Sends the packet to the TFTP server identified by its default
     *   endpoint.
     *
     * @param[in] packet
     *   TFTP packet to send.
     **/
    void sendFirst( const Packets::Packet &packet );

    /**
     * @brief Sends the packet to the TFTP %Server.
     *
     * @param[in] packet
     *   Packet, which is sent to the server.
     **/
    void send( const Packets::Packet &packet );

    /**
     * @brief Waits for an incoming response from the server.
     *
     * @sa receiveFirstHandler
     * @sa timeoutFirstHandler
     **/
    void receiveFirst();

    /**
     * @brief Waits for an incoming response from the server.
     *
     * @sa receiveHandler
     * @sa timeoutHandler
     **/
    void receive();

    /**
     * @brief Final Wait for possible resend of last package, when final ACK was
     *   lost.
     *
     * Receive is handled as normal.
     * If timeout has occurred, operation is finished successfully.
     *
     * @sa receiveHandler
     * @sa timeoutDallyHandler
     **/
    void receiveDally();

    /**
     * @brief Sets the Finished flag.
     *
     * This operation is called, when the last packet has been received or
     * transmitted to stop the reception loop.
     *
     * @param[in] status
     *   If the operation was successful, or an error occurred.
     * @param[in] errorInfo
     *   In error case, this information is set accordingly.
     **/
    virtual void finished( TransferStatus status, Packets::ErrorInfo &&errorInfo = {} );

    /**
     * @copydoc Packets::PacketHandler::readRequestPacket()
     *
     * A read request packet is handled as failure.
     * An error packet is sent to the origin and the finished flag is set.
     *
     * This operation always throws an CommunicationException.
     **/
    void readRequestPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::ReadRequestPacket &readRequestPacket ) final;

    /**
     * @copydoc Packets::PacketHandler::writeRequestPacket()
     *
     * A write request packet is handled as failure.
     * An error packet is sent to the origin and the finished flag is set.
     **/
    void writeRequestPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::WriteRequestPacket &writeRequestPacket ) final;

    /**
     * @copydoc Packets::PacketHandler::errorPacket()
     *
     * Terminate connection.
     **/
    void errorPacket( const boost::asio::ip::udp::endpoint &remote, const Packets::ErrorPacket &errorPacket ) final;

    /**
     * @copydoc Packets::PacketHandler::invalidPacket()
     *
     * Send error packet and terminate connection.
     **/
    void invalidPacket(
      const boost::asio::ip::udp::endpoint &remote,
      Packets::ConstRawTftpPacketSpan rawPacket ) final;

  private:
    /**
     * @brief Called, when data is received the first time.
     *
     * In case the first time a TFTP packet has been received, the source is
     * checked and the socket is connected to the final endpoint.
     *
     * @param[in] errorCode
     *   error status of operation.
     * @param[in] bytesTransferred
     *   Number of bytes transferred.
     *
     * @throw CommunicationException
     *   On communication error.
     **/
    void receiveFirstHandler( const boost::system::error_code &errorCode, std::size_t bytesTransferred );

    /**
     * @brief Handler, which is called when new data has been received.
     *
     * @param[in] errorCode
     *   Receive error code
     * @param[in] bytesTransferred
     *   Number of bytes transferred.
     **/
    void receiveHandler( const boost::system::error_code &errorCode, std::size_t bytesTransferred );

    /**
     * @brief Called when no data is received for the first sent packet.
     *
     * If the retransmission counter has not exceeded, the last sent packet
     * is retransmitted.
     *
     * @param[in] errorCode
     *   error status of operation.
     **/
    void timeoutFirstHandler( const boost::system::error_code &errorCode );

    /**
     * @brief Called when no data is received for the sent packet.
     *
     * If the retransmission counter has not exceeded, the last sent packet
     * is retransmitted.
     *
     * @param[in] errorCode
     *   error status of operation.
     **/
    void timeoutHandler( const boost::system::error_code &errorCode );

    /**
     * @brief Called when no data is received for the last sent ACK.
     *
     * The operation is finished successfully.
     *
     * @param[in] errorCode
     *   error status of operation.
     **/
    void timeoutDallyHandler( const boost::system::error_code &errorCode );

    //! Receive timeout (can be updated by option negotiation)
    std::chrono::seconds receiveTimeoutV{ Tftp::DefaultTftpReceiveTimeout };
    //! TFTP Retries
    uint16_t tftpRetriesV{ Tftp::DefaultTftpRetries };

    //! Handler which is called on completion of the operation.
    OperationCompletedHandler completionHandlerV;
    //! Address of the remote endpoint (TFTP %Server).
    boost::asio::ip::udp::endpoint remoteV;
    //! Local address, where the client handles the request from.
    boost::asio::ip::udp::endpoint localV;

    //! TFTP UDP Socket
    boost::asio::ip::udp::socket socket;
    //! Receive timeout timer
    boost::asio::system_timer timer;

    //! Received Packet Data
    Packets::RawTftpPacket receivePacket;
    //! Remote Address (set, when server sends first answer)
    boost::asio::ip::udp::endpoint receiveEndpoint;
    //! Last transmitted Packet ( used for retries)
    Packets::RawTftpPacket transmitPacket;
    //! Re-transmission counter
    unsigned int transmitCounter{ 0U };
    //! Error info
    Packets::ErrorInfo errorInfoV;
};

}

#endif
