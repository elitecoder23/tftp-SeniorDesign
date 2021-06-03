/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Server::OperationImpl.
 **/

#ifndef TFTP_SERVER_OPERATIONIMPL_HPP
#define TFTP_SERVER_OPERATIONIMPL_HPP

#include <tftp/server/Server.hpp>
#include <tftp/server/Operation.hpp>

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/ErrorPacket.hpp>
#include <tftp/packets/PacketHandler.hpp>

#include <boost/asio.hpp>

#include <string>
#include <memory>

namespace Tftp::Server {

class TftpServerInternal;

/**
 * @brief TFTP %Server %Operation.
 *
 * This class is specialised for the two kinds of TFTP operations
 * (Read Operation, Write Operation).
 **/
class OperationImpl:
  public std::enable_shared_from_this< OperationImpl>,
  public Operation,
  protected Packets::PacketHandler
{
  public:
    //! @copydoc Operation::gracefulAbort
    void gracefulAbort(
      ErrorCode errorCode,
      std::string_view errorMessage ) final;

    //! @copydoc Operation::abort
    void abort() final;

    //! @copydoc Operation::errorInfo
    const ErrorInfo& errorInfo() const final;

  protected:
    /**
     * @brief Initialises the TFTP server operation.
     *
     * @param[in] ioContext
     *   I/O context used for communication.
     * @param[in] tftpTimeout
     *   TFTP Timeout, when no timeout option is negotiated in seconds.
     * @param[in] tftpRetries
     *   Number of retries.
     * @param[in] maxReceivePacketSize
     *   Maximum packet size for receive operation.
     * @param[in] completionHandler
     *   Handler which is called on completion of this operation.
     * @param[in] remote
     *   Address of the remote endpoint (TFTP client).
     **/
    OperationImpl(
      boost::asio::io_context &ioContext,
      uint8_t tftpTimeout,
      uint16_t tftpRetries,
      uint16_t maxReceivePacketSize,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote );

    /**
     * @copydoc OperationImpl(boost::asio::io_context&,uint8_t,uint16_t,OperationCompletedHandler,const boost::asio::ip::udp::endpoint&)
     *
     * @param[in] local
     *   local endpoint, where the server handles the request from.
     **/
    OperationImpl(
      boost::asio::io_context &ioContext,
      uint8_t tftpTimeout,
      uint16_t tftpRetries,
      uint16_t maxReceivePacketSize,
      OperationCompletedHandler completionHandler,
      const boost::asio::ip::udp::endpoint &remote,
      const boost::asio::ip::udp::endpoint &local );

    /**
     * @brief Destructor.
     **/
    ~OperationImpl() noexcept override;

    /**
     * @brief Sets the Finished flag.
     *
     * This operation is called, when the last packet has been received or
     * transmitted to stop the receive loop.
     *
     * @param[in] status
     *   Transfer status.
     * @param[in] errorInfo
     *   Optional error information
     **/
    virtual void finished(
      TransferStatus status,
      ErrorInfo &&errorInfo = {} );

    /**
     * @brief Sends the given packet to the client.
     *
     * @param[in] packet
     *   The packet, which is sent to the client.
     *
     * @throw CommunicationException
     **/
    void send( const Packets::Packet &packet);

    /**
     * @brief receives a packet and calls the packet handlers
     **/
    void receive();

    /**
     * @brief Update the receiveTimeout value.
     *
     * @param[in] receiveTimeout
     *   The new receive timeout.
     **/
    void receiveTimeout( uint8_t receiveTimeout ) noexcept;

    /**
     * @copydoc PacketHandler::readRequestPacket
     *
     * A RRQ packet is not expected - therefore send an error packet an
     * terminate connection.
     **/
    void readRequestPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::ReadRequestPacket &readRequestPacket ) final;

    /**
     * @copydoc PacketHandler::writeRequestPacket
     *
     * A WRQ packet is not expected - therefore send an error packet an
     * terminate connection.
     **/
    void writeRequestPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::WriteRequestPacket &writeRequestPacket ) final;

    /**
     * @copydoc PacketHandler::errorPacket()
     *
     * Terminate connection.
     **/
    void errorPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::ErrorPacket &errorPacket) final;

    /**
     * @copydoc PacketHandler::optionsAcknowledgementPacket()
     *
     * A OACK packet is not expected - therefore send an error packet an
     * terminate connection.
     **/
     void optionsAcknowledgementPacket(
      const boost::asio::ip::udp::endpoint &remote,
      const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket ) final;

    /**
     * @copydoc PacketHandler::invalidPacket()
     *
     * Send error packet and terminate connection.
     **/
    void invalidPacket(
      const boost::asio::ip::udp::endpoint &remote,
      Packets::ConstRawTftpPacketSpan rawPacket ) final;

  private:
    /**
     * @brief Handler, which is called when new data has been received.
     *
     * @param[in] errorCode
     *   Receive error code
     * @param bytesTransferred
     *   Number of transferred bytes.
     **/
    void receiveHandler(
      const boost::system::error_code& errorCode,
      std::size_t bytesTransferred);

    /**
     * @brief Handler for receive timeouts
     *
     * @param[in] errorCode
     *   When handler is called with error, errorCode is set
     **/
    void timeoutHandler( const boost::system::error_code& errorCode );

    //! Operation Completed Handler
    OperationCompletedHandler completionHandler;

    //! Receive timeout - is initialised to Tftp::DefaultTftpReceiveTimeout
    uint8_t receiveTimeoutV;
    //! TFTP Retries
    const uint16_t tftpRetries;

    //! TFTP UDP Socket
    boost::asio::ip::udp::socket socket;
    //! Timeout timer
    boost::asio::deadline_timer timer;

    //! stores the received packets
    Packets::RawTftpPacket receivePacket;
    //! transmitted packet is stored for retries
    Packets::RawTftpPacket transmitPacket;
    //! counter to store how often the same packet has been transmitted (retries)
    unsigned int transmitCounter;
    //! Error info
    ErrorInfo errorInfoV;
};

}

#endif
