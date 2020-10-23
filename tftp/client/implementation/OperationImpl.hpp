/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Client::OperationImpl.
 **/

#ifndef TFTP_CLIENT_OPERATIONIMPL_HPP
#define TFTP_CLIENT_OPERATIONIMPL_HPP

#include <tftp/PacketHandler.hpp>

#include <tftp/client/Client.hpp>
#include <tftp/client/Operation.hpp>
#include <tftp/client/TftpClient.hpp>

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/ErrorPacket.hpp>

#include <tftp/options/OptionList.hpp>

#include <boost/asio.hpp>

#include <string>
#include <memory>

namespace Tftp::Client {

// Forward declaration
class TftpClientInternal;

/**
 * @brief TFTP %Client %Operation.
 *
 * This class is specialised for the two kinds of TFTP operations
 * (Read Operation, Write Operation).
 **/
class OperationImpl :
  public std::enable_shared_from_this< OperationImpl>,
  public Operation,
  protected PacketHandler
{
  public:
    /**
     * @brief Destructor.
     *
     * Closes the socket.
     **/
    ~OperationImpl() noexcept override;

    //! @copydoc Operation::gracefulAbort
    void gracefulAbort(
      ErrorCode errorCode,
      std::string_view errorMessage = {}) final;

    //! @copydoc Operation::abort
    void abort() final;

    //! @copydoc Operation::errorInfo
    const ErrorInfo& errorInfo() const final;

  protected:
    /**
     * @brief Initialises the operation.
     *
     * @param[in] ioContext
     *   The I/O context used for communication.
     * @param[in] tftpTimeout
     *   TFTP Timeout, when no timeout option is negotiated in seconds.
     * @param[in] tftpRetries
     *   Number of retries.
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
     * @param[in] remote
     *   Where the connection should be established to.
     **/
    OperationImpl(
      boost::asio::io_context &ioContext,
      uint8_t tftpTimeout,
      uint16_t tftpRetries,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote );

    /**
     * @copydoc OperationImpl(boost::asio::io_context&,uint8_t,uint16_t,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&)
     *
     * @param[in] local
     *   Parameter to define the communication source
     **/
    OperationImpl(
      boost::asio::io_context &ioContext,
      uint8_t tftpTimeout,
      uint16_t tftpRetries,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const boost::asio::ip::udp::endpoint &local );

    /**
     * @brief Sends the packet to the TFTP server identified by its default
     *   endpoint.
     *
     * @param[in] packet
     *   TFTP packet to send.
     **/
    void sendFirst( const Packets::Packet &packet );

    /**
     * @brief Sends the packet to the TFTP server.
     *
     * @param[in] packet
     *   TFTP packet to send.
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
     * @brief Updates the @p maxReceivePacketSize value.
     *
     * This value is used to resize the packet buffer before starting a
     * receive operation. This value could be modified, e.g. during option
     * negotiation.
     *
     * @param[in] maxReceivePacketSize
     *   New maxReceivePacketSize value.
     **/
    void maxReceivePacketSize( uint16_t maxReceivePacketSize ) noexcept;

    /**
     * @brief Update the receiveTimeout value.
     *
     * @param[in] receiveTimeout
     *   New receive timeout.
     **/
    void receiveTimeout( uint8_t receiveTimeout ) noexcept;

    /**
     * @brief Sets the finished flag.
     *
     * @param[in] status
     *   If the operation was successful or an error occurred.
     * @param[in] errorInfo
     *   In error case, this information is set accordingly.
     **/
    virtual void finished(
      TransferStatus status,
      ErrorInfo &&errorInfo = {} ) noexcept;

    /**
     * @copydoc PacketHandler::readRequestPacket()
     *
     * A read request packet is handled as failure.
     * A error packet is sent to the origin and the finished flag is set.
     *
     * This operation always throws an CommunicationException.
     **/
    void readRequestPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::ReadRequestPacket &readRequestPacket ) final;

    /**
     * @copydoc PacketHandler::writeRequestPacket()
     *
     * A write request packet is handled as failure.
     * A error packet is sent to the origin and the finished flag is set.
     **/
    void writeRequestPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::WriteRequestPacket &writeRequestPacket ) final;

    /**
     * @copydoc PacketHandler::errorPacket()
     **/
    void errorPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::ErrorPacket &errorPacket ) final;

    /**
     * @copydoc PacketHandler::invalidPacket()
     **/
    void invalidPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const RawTftpPacket &rawPacket ) final;

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
    void receiveFirstHandler(
      const boost::system::error_code& errorCode,
      std::size_t bytesTransferred);

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
     * @brief Called when no data is received for the first sent packet.
     *
     * If the retransmission counter has not exceeded, the last sent packet
     * is retransmitted.
     *
     * @param[in] errorCode
     *   error status of operation.
     **/
    void timeoutFirstHandler( const boost::system::error_code &errorCode);

    /**
     * @brief Called when no data is received for the sent packet.
     *
     * If the retransmission counter has not exceeded, the last sent packet
     * is retransmitted.
     *
     * @param[in] errorCode
     *   error status of operation.
     **/
    void timeoutHandler( const boost::system::error_code &errorCode);

    //! Completion Handler
    OperationCompletedHandler completionHandler;
    //! TFTP Server Endpoint
    boost::asio::ip::udp::endpoint remoteEndpoint;

    /**
     * The maximum size of a received TFTP packet.
     * Defaults to Tftp::DefaultMaxPacketSize.
     *
     * This value can be modified by calling setMaxReceivePacketSize(), e.g.
     * during option negotiation.
     **/
    uint16_t maxReceivePacketSizeV;
    //! Receive timeout - is initialised to Tftp::DefaultTftpReceiveTimeout
    uint8_t receiveTimeoutV;
    //! TFTP Retries
    const uint16_t tftpRetries;

    //! TFTP socket
    boost::asio::ip::udp::socket socket;
    //! Receive timeout timer
    boost::asio::deadline_timer timer;

    //! Received packet data
    RawTftpPacket receivePacket;
    //! Remote address
    boost::asio::ip::udp::endpoint receiveEndpoint;
    //! Last transmitted Packet ( used for retries)
    RawTftpPacket transmitPacket;
    //! Re-transmission counter
    unsigned int transmitCounter;
    //! Error info
    ErrorInfo errorInfoV;
};

}

#endif
