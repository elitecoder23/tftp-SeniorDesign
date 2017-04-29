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
 * @brief Declaration of class Tftp::Server::OperationImpl.
 **/

#ifndef TFTP_SERVER_OPERATIONIMPL_HPP
#define TFTP_SERVER_OPERATIONIMPL_HPP

#include <tftp/server/Server.hpp>
#include <tftp/server/Operation.hpp>

#include <tftp/packets/Packets.hpp>
#include <tftp/packets/ErrorPacket.hpp>

#include <tftp/options/OptionList.hpp>

#include <tftp/PacketHandler.hpp>

#include <boost/asio.hpp>

#include <string>
#include <memory>

namespace Tftp {
namespace Server {

class TftpServerInternal;

/**
 * @brief Base class for TFTP server operations.
 *
 * This class is specialised for the two kinds of TFTP operations
 * (Read Operation, Write Operation).
 **/
class OperationImpl:
  public std::enable_shared_from_this< OperationImpl>,
  public Operation,
  protected PacketHandler
{
  public:
    //! @copydoc Operation::start()
    virtual void start() override;

    //! @copydoc Operation::gracefulAbort
    virtual void gracefulAbort(
      ErrorCode errorCode,
      const string &errorMessage) override;

    //! @copydoc Operation::abort
    virtual void abort() override;

    //! @copydoc Operation::getErrorInfo
    virtual const ErrorInfo& getErrorInfo() const override final;

  protected:
    /**
     * @brief Initialises the TFTP server operation.
     *
     * @param[in] ioService
     *   The IO service used for communication.
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
     * @param[in] tftpServerInternal
     *   The TFTP internal server.
     * @param[in] clientAddress
     *   Address of the remote endpoint (TFTP client).
     * @param[in] clientOptions
     *   Received option list from client.
     * @param[in] serverAddress
     *   local endpoint, where the server handles the request from.
     **/
    OperationImpl(
      boost::asio::io_service &ioService,
      OperationCompletedHandler completionHandler,
      const TftpServerInternal &tftpServerInternal,
      const UdpAddressType &clientAddress,
      const Options::OptionList &clientOptions,
      const UdpAddressType &serverAddress);

    /**
     * @brief Constructor of operation
     *
     * @param[in] ioService
     *   The IO service used for communication.
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
     * @param[in] tftpServerInternal
     *   The TFTP internal server.
     * @param[in] clientAddress
     *   Address of the remote endpoint (TFTP client).
     * @param[in] clientOptions
     *   Received option list from client.
     **/
    OperationImpl(
      boost::asio::io_service &ioService,
      OperationCompletedHandler completionHandler,
      const TftpServerInternal &tftpServerInternal,
      const UdpAddressType &clientAddress,
      const Options::OptionList &clientOptions);

    /**
     * @brief default destructor.
     **/
    virtual ~OperationImpl() noexcept;

    /**
     * @brief Sets the Finished flag.
     *
     * This operation is called, when the last packet has been received or
     * transmitted to stop the receive loop.
     **/
    virtual void finished(
      TransferStatus status,
      ErrorInfo &&errorInfo = {}) noexcept;

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
     * @brief Returns the stored TFTP option list.
     *
     * @return The stored TFTP option list.
     **/
    Options::OptionList& getOptions();

    /**
     * @brief Updates the limit of maximum packet size for the receive
     *   operation.
     *
     * This operation must be used to vary the receive buffer, when the
     * option negotiation results in a modified packet size.
     *
     * The set maximum packet size is not forced when a packet is sent to
     * the client.
     *
     * @param[in] maxReceivePacketSize
     *   The new maximum packet size for receive operation.
     **/
    void setMaxReceivePacketSize( uint16_t maxReceivePacketSize);

    /**
     * @brief Update the receiveTimeout value.
     *
     * @param[in] receiveTimeout
     *   The new receive timeout.
     **/
    void setReceiveTimeout( uint8_t receiveTimeout);

    /**
     * @copydoc PacketHandler::handleReadRequestPacket
     *
     * A RRQ packet is not expected - therefore send an error packet an
     * terminate connection.
     **/
    virtual void handleReadRequestPacket(
      const UdpAddressType &from,
      const Packets::ReadRequestPacket &readRequestPacket) final;

    /**
     * @copydoc PacketHandler::handleWriteRequestPacket
     *
     * A WRQ packet is not expected - therefore send an error packet an
     * terminate connection.
     **/
    virtual void handleWriteRequestPacket(
      const UdpAddressType &from,
      const Packets::WriteRequestPacket &writeRequestPacket) final;

    /**
     * @copydoc PacketHandler::handleErrorPacket()
     *
     * Terminate connection.
     **/
    virtual void handleErrorPacket(
      const UdpAddressType &from,
      const Packets::ErrorPacket &errorPacket) final;

    /**
     * @copydoc PacketHandler::handleOptionsAcknowledgementPacket()
     *
     * A OACK packet is not expected - therefore send an error packet an
     * terminate connection.
     **/
    virtual void handleOptionsAcknowledgementPacket(
      const UdpAddressType &from,
      const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket)
        final;

    /**
     * @copydoc PacketHandler::handleInvalidPacket()
     *
     * Send error packet and terminate connection.
     **/
    virtual void handleInvalidPacket(
      const UdpAddressType &from,
      const RawTftpPacketType &rawPacket) final;

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
    void timeoutHandler( const boost::system::error_code& errorCode);

    //!
    OperationCompletedHandler completionHandler;
    //! The internal TFTP server
    const TftpServerInternal &tftpServerInternal;

    //! The stored negotiated options
    Options::OptionList options;
    //! The maximum packet size, which can be received
    uint16_t maxReceivePacketSize;
    //! The receive timeout - is initialised to TFTP_DEFAULT_TIMEOUT
    uint8_t receiveTimeout;

    //! The TFTP UDP socket
    boost::asio::ip::udp::socket socket;
    //! The timeout timer
    boost::asio::deadline_timer timer;

    //! stores the received packets
    RawTftpPacketType receivePacket;
    //! the transmitted packet is stored for retries
    RawTftpPacketType transmitPacket;
    //! Packet Type of last transmitted packet
    PacketType transmitPacketType;
    //! counter to store how often the same packet has been transmitted (retries)
    unsigned int transmitCounter;
    //! Error info
    ErrorInfo errorInfo;
};

}
}

#endif
