/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::Client::OperationImpl.
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

namespace Tftp {
namespace Client {


class TftpClientInternal;

/**
 * @brief base class for TFTP client operations.
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
    virtual ~OperationImpl() noexcept;

    /**
     * @brief Operation execution
     *
     * This operation executes the TFTP operation.
     *
     * The implementation on this level starts the receive operation.
     *
     * A child class inheriting from this class must override this operation,
     * sending the request package and then calling this method.
     **/
    virtual void start() override;

    //! @copydoc Operation::gracefulAbort
    void gracefulAbort(
      ErrorCode errorCode,
      const string &errorMessage = string()) final;

    //! @copydoc Operation::abort
    void abort() override final;

    //! @copydoc Operation::getErrorInfo
    const ErrorInfo& getErrorInfo() const final;

  protected:
    //! TFTP options (list) @todo rename to Options
    using OptionList = Options::OptionList;

    /**
     * @brief Initialises the operation.
     *
     * @param[in] ioService
     *   The IO service used for communication.
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
     * @param[in] tftpClient
     *   The TFTP client.
     * @param[in] serverAddress
     *   Where the connection should be established to.
     * @param[in] filename
     *   Which file shall be requested
     * @param[in] mode
     *   The transfer mode
     * @param[in] from
     *   Optional parameter to define the communication source
     **/
    OperationImpl(
      boost::asio::io_service &ioService,
      OperationCompletedHandler completionHandler,
      const TftpClientInternal &tftpClient,
      const UdpAddressType &serverAddress,
      const string &filename,
      TransferMode mode,
      const UdpAddressType &from);

    /**
     * @brief Initialises the operation.
     *
     * @param[in] ioService
     *   The IO service used for communication.
     * @param[in] completionHandler
     *   The handler which is called on completion of this operation.
     * @param[in] tftpClient
     *   The TFTP client.
     * @param[in] serverAddress
     *   Where the connection should be established to.
     * @param[in] filename
     *   Which file shall be requested
     * @param[in] mode
     *   The transfer mode
     **/
    OperationImpl(
      boost::asio::io_service &ioService,
      OperationCompletedHandler completionHandler,
      const TftpClientInternal &tftpClient,
      const UdpAddressType &serverAddress,
      const string &filename,
      TransferMode mode);

    /**
     * @brief Returns the request filename.
     *
     * @return The request filename.
     **/
    const string& getFilename() const;

    /**
     * @brief Returns the transfer mode.
     *
     * @return The transfer mode.
     **/
    TransferMode getMode() const;

    /**
     * @brief Returns the TFTP option list.
     *
     * @return The TFTP option list.
     **/
    OptionList& getOptions();

    /**
     * @brief Sends the packet to the TFTP server identified by its default
     *   endpoint.
     *
     * @param[in] packet
     *   The TFTP packet to send.
     **/
    void sendFirst( const Packets::Packet &packet);

    /**
     * @brief Sends the packet to the TFTP server.
     *
     * @param[in] packet
     *   The TFTP packet to send.
     **/
    void send( const Packets::Packet &packet);

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
     * @brief Updates the maxReceivePacketSize value.
     *
     * This value is used to resize the packet buffer before starting a
     * receive operation. This value could be modified, e.g. during option
     * negotiation.
     *
     * @param[in] maxReceivePacketSize
     *   The new maxReceivePacketSize value.
     **/
    void setMaxReceivePacketSize( uint16_t maxReceivePacketSize) noexcept;

    /**
     * @brief Update the receiveTimeout value.
     *
     * @param[in] receiveTimeout
     *   The new receive timeout.
     **/
    void setReceiveTimeout( uint8_t receiveTimeout) noexcept;

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
      ErrorInfo &&errorInfo = {}) noexcept;

    /**
     * @copydoc PacketHandler::handleReadRequestPacket()
     *
     * A read request packet is handled as failure. A error packet is sent
     * to the origin and the finished flag is set
     *
     * This operation always throws an CommunicationException.
     **/
    void handleReadRequestPacket(
      const UdpAddressType &from,
      const Packets::ReadRequestPacket &readRequestPacket) final;

    /**
     * @copydoc PacketHandler::handleWriteRequestPacket()
     *
     * A write request packet is handled as failure. A error packet is sent
     * to the origin and the finished flag is set
     **/
    void handleWriteRequestPacket(
      const UdpAddressType &from,
      const Packets::WriteRequestPacket &writeRequestPacket) final;

    /**
     * @copydoc PacketHandler::handleErrorPacket()
     **/
    void handleErrorPacket(
      const UdpAddressType &from,
      const Packets::ErrorPacket &errorPacket) final;

    /**
     * @copydoc PacketHandler::handleInvalidPacket()
     **/
    void handleInvalidPacket(
      const UdpAddressType &from,
      const RawTftpPacketType &rawPacket) final;

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
     *   Number of bytes transfered.
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
    //! The internal TFTP client
    const TftpClientInternal &tftpClient;
    //! The TFTP server endpoint
    UdpAddressType remoteEndpoint;
    //! The filename of the transfer
    const string filename;
    //! The transfer mode (OCTETT/ NETASCII/ MAIL/ ...)
    const TransferMode mode;
    //! options for the transfer
    OptionList options;
    /**
     * The maximum size of a received TFTP packet. Defaults to
     * TftpPacket::DEFAULT_MAX_PACKET_SIZE.
     *
     * This value can be modified by calling setMaxReceivePacketSize(), e.g.
     * during option negotiation.
     **/
    uint16_t maxReceivePacketSize;
    //! The receive timeout - is initialised to TFTP_DEFAULT_TIMEOUT
    uint8_t receiveTimeout;

    //! The TFTP socket
    boost::asio::ip::udp::socket socket;
    //! The receive timeout timer
    boost::asio::deadline_timer timer;

    //! The received packet data
    RawTftpPacketType receivePacket;
    //! The remote address
    UdpAddressType receiveEndpoint;
    //! The last transmitted packet
    RawTftpPacketType transmitPacket;
    //! Packet Type of last transmitted packet
    PacketType transmitPacketType;
    //! the retransmission counter
    unsigned int transmitCounter;
    //! Error info
    ErrorInfo errorInfo;
};

}
}

#endif
