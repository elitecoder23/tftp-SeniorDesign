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
 * @brief Declaration of class Tftp::Client::TftpClientOperationImpl.
 **/

#ifndef TFTP_CLIENT_TFTPCLIENTOPERATIONIMPL_HPP
#define TFTP_CLIENT_TFTPCLIENTOPERATIONIMPL_HPP

#include <tftp/TftpPacketHandler.hpp>
#include <tftp/client/Client.hpp>
#include <tftp/client/TftpClientOperation.hpp>
#include <tftp/packet/Packet.hpp>
#include <tftp/options/OptionList.hpp>

#include <boost/asio.hpp>

#include <string>

namespace Tftp {
namespace Client {


class TftpClientInternal;

/**
 * @brief base class for TFTP client operations.
 *
 * This class is specialised for the two kinds of TFTP operations
 * (Read Operation, Write Operation).
 **/
class TftpClientOperationImpl :
  public TftpClientOperation,
  protected TftpPacketHandler
{
  public:
    using string = std::string;

    /**
     * @brief Destructor.
     *
     * Closes the socket.
     **/
    virtual ~TftpClientOperationImpl() noexcept;

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
    virtual void operator()() override;

    //! @copydoc TftpClientOperation::gracefulAbort
    virtual void gracefulAbort(
      ErrorCode errorCode,
      const string &errorMessage = string()) override final;

    //! @copydoc TftpClientOperation::abort
    virtual void abort() override final;

    //! @copydoc TftpClientOperation::getRequestType
    virtual RequestType getRequestType() const override final;

    //! @copydoc TftpClientOperation::getServerAddress
    virtual const UdpAddressType& getServerAddress() const override final;

    //! @copydoc TftpClientOperation::getFilename
    virtual const string& getFilename() const override final;

    //! @copydoc TftpClientOperation::getMode
    virtual TransferMode getMode() const override final;

  protected:
    using OptionList = Options::OptionList;
    using TftpPacket = Packet::TftpPacket;

    /**
     * @brief Constructor of TftpClientOperation
     *
     * @param[in] requestType
     *   The TFTP request type.
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
    TftpClientOperationImpl(
      RequestType requestType,
      const TftpClientInternal &tftpClient,
      const UdpAddressType &serverAddress,
      const string &filename,
      TransferMode mode,
      const UdpAddressType &from);

    /**
     * @brief Constructor of TftpClientOperation
     *
     * @param[in] requestType
     *   The TFTP request type.
     * @param[in] tftpClient
     *   The TFTP client.
     * @param[in] serverAddress
     *   Where the connection should be established to.
     * @param[in] filename
     *   Which file shall be requested
     * @param[in] mode
     *   The transfer mode
     **/
    TftpClientOperationImpl(
      RequestType requestType,
      const TftpClientInternal &tftpClient,
      const UdpAddressType &serverAddress,
      const string &filename,
      TransferMode mode);

    /**
     * @brief Returns the TFTP option list.
     *
     * @return The TFTP option list.
     **/
    OptionList & getOptions();

    /**
     * @brief Sets the finished flag.
     **/
    void finished() noexcept;

    /**
     * @brief Sends the packet to the TFTP server identified by its default
     *   endpoint.
     *
     * @param[in] packet
     *   The TFTP packet to send.
     **/
    void sendFirst( const TftpPacket &packet);

    /**
     * @brief Sends the packet to the TFTp server.
     *
     * @param[in] packet
     *   The TFTP packet to send.
     **/
    void send( const TftpPacket &packet);

    /**
     * @brief Waits for an incoming response from the server.
     **/
    void receiveFirst();

    /**
     * @brief Waits for an incoming response from the server.
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
     * @copydoc TftpPacketHandler::handleReadRequestPacket()
     *
     * A read request packet is handled as failure. A error packet is sent
     * to the origin and the finished flag is set
     *
     * This operation always throws an CommunicationException.
     **/
    virtual void handleReadRequestPacket(
      const UdpAddressType &from,
      const ReadRequestPacket &readRequestPacket) override final;

    /**
     * @copydoc TftpPacketHandler::handleWriteRequestPacket()
     *
     * A write request packet is handled as failure. A error packet is sent
     * to the origin and the finished flag is set
     **/
    virtual void handleWriteRequestPacket(
      const UdpAddressType &from,
      const WriteRequestPacket &writeRequestPacket) override final;

    /**
     * @copydoc TftpPacketHandler::handleErrorPacket()
     **/
    virtual void handleErrorPacket(
      const UdpAddressType &from,
      const ErrorPacket &errorPacket) override final;

    /**
     * @copydoc TftpPacketHandler::handleInvalidPacket()
     **/
    virtual void handleInvalidPacket(
      const UdpAddressType &from,
      const RawTftpPacketType &rawPacket) override final;

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
    void timeoutFirstHandler( const boost::system::error_code& errorCode);

    /**
     * @brief Called when no data is received for the sent packet.
     *
     * If the retransmission counter has not exceeded, the last sent packet
     * is retransmitted.
     *
     * @param[in] errorCode
     *   error status of operation.
     **/
    void timeoutHandler( const boost::system::error_code& errorCode);

    //! The request type
    const RequestType requestType;
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

    //! The IO service, which handles the asynchronous receive operation
    boost::asio::io_service ioService;
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
};

}
}

#endif
