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
 * @brief Declaration of class Tftp::Server::TftpServer.
 *
 * The class TftpServer is the main entry for a TFTP server application.
 **/

#ifndef TFTP_SERVER_TFTPSERVER_HPP
#define TFTP_SERVER_TFTPSERVER_HPP

#include <tftp/server/Server.hpp>
#include <tftp/TftpConfiguration.hpp>
#include <tftp/options/Options.hpp>

#include <string>

namespace Tftp {
namespace Server {

/**
 * @brief Public interface of TFTP servers.
 *
 * Waits on the specified port for a valid TFTP request and calls the
 * approbate call-back, which has to handle the request.
 *
 * If not expected packets or invalid packets are received a error is send
 * back to the sender.
 *
 * Valid requests are TFTP Read Request (RRQ) and TFTP Write Request (WRQ)
 **/
class TftpServer
{
  public:
    using string = std::string;

    /**
     * @brief The default address, where the server listens.
     *
     * The default local end-point where the TFTP server is listening is the
     * default TFTP Port any IP address.
     **/
    static const UdpAddressType DefaultLocalEndpoint;

    /**
     * @brief Creates an instance of the TFTP server.
     *
     * @param[in] handler
     *   The request handler.
     * @param[in] configuration
     *   The TFTP Configuration
     * @param[in] additionalOptions
     *   Additional Options, which shall be used as TFTP server option list.
     * @param[in] serverAddress
     *   Address where the FTP server should listen on.
     **/
    static TftpServerPtr createInstance(
      ReceivedTftpRequestHandler handler,
      const TftpConfiguration &configuration = TftpConfiguration(),
      const Options::OptionList& additionalOptions = Options::OptionList(),
      const UdpAddressType &serverAddress = DefaultLocalEndpoint);

    //! Default destructor
    virtual ~TftpServer() noexcept = default;

    /**
     * @brief Starts the TFTP Server.
     *
     * This routines starts the server loop, which waits for incoming
     * requests and calls the handler to handles them.
     *
     * The start routine will be leaved, when an FATAL error occurred or
     * the server has been stopped by calling stop().
     **/
    virtual void operator()() = 0;

    /**
     * @brief Stops the TFTP Server.
     **/
    virtual void stop() = 0;

    /**
     * @brief Creates a TFTP write operation (TFTP RRQ)
     *
     * @param[in] handler
     *   Handler, which will be called on various events.
     * @param[in] clientAddress
     *   Address of the remote endpoint (TFTP client).
     * @param[in] clientOptions
     *   Received option list from client.
     * @param[in] serverAddress
     *   local endpoint, where the server handles the request from.
     *
     * @return The TFTP server write operation.
     **/
    virtual TftpServerOperation createReadRequestOperation(
      TransmitDataOperationHandler &handler,
      const UdpAddressType &clientAddress,
      const Options::OptionList &clientOptions,
      const UdpAddressType &serverAddress) = 0;

    /**
     * @brief Creates a TFTP write operation (TFTP RRQ)
     *
     * @param[in] handler
     *   Handler, which will be called on various events.
     * @param[in] clientAddress
     *   Address of the remote endpoint (TFTP client).
     * @param[in] clientOptions
     *   Received option list from client.
     *
     * @return The TFTP server write operation.
     **/
    virtual TftpServerOperation createReadRequestOperation(
      TransmitDataOperationHandler &handler,
      const UdpAddressType &clientAddress,
      const Options::OptionList &clientOptions) = 0;

    /**
     * @brief Creates a TFTP read operation (TFTP WRQ)
     *
     * @param[in] handler
     *   Handler, which will be called on various events.
     * @param[in] clientAddress
     *   Address of the remote endpoint (TFTP client).
     * @param[in] clientOptions
     *   Received option list from client.
     * @param[in] serverAddress
     *   local endpoint, where the server handles the request from.
     *
     * @return The TFTP server read operation.
     **/
    virtual TftpServerOperation createWriteRequestOperation(
      ReceiveDataOperationHandler &handler,
      const UdpAddressType &clientAddress,
      const Options::OptionList &clientOptions,
      const UdpAddressType &serverAddress) = 0;

    /**
     * @brief Creates a TFTP read operation (TFTP WRQ)
     *
     * @param[in] handler
     *   Handler, which will be called on various events.
     * @param[in] clientAddress
     *   Address of the remote endpoint (TFTP client).
     * @param[in] clientOptions
     *   Received option list from client.
     *
     * @return The TFTP server read operation.
     **/
    virtual TftpServerOperation createWriteRequestOperation(
      ReceiveDataOperationHandler &handler,
      const UdpAddressType &clientAddress,
      const Options::OptionList &clientOptions) = 0;

    /**
     * @brief Creates a TFTP error operation.
     *
     * @param[in] clientAddress
     *   Where the error packet shall be transmitted to.
     * @param[in] from
     *   Optional parameter to define the communication source
     * @param[in] errorCode
     *   The error code of the error packet.
     * @param[in] errorMessage
     *   The error message of the packet.
     **/
    virtual TftpServerOperation createErrorOperation(
      const UdpAddressType &clientAddress,
      const UdpAddressType &from,
      ErrorCode errorCode,
      const string &errorMessage = string()) = 0;

    /**
     * @brief Creates a TFTP error operation.
     *
     * @param[in] clientAddress
     *   Where the error packet shall be transmitted to.
     * @param[in] errorCode
     *   The error code of the error packet.
     * @param[in] errorMessage
     *   The error message of the packet.
     **/
    virtual TftpServerOperation createErrorOperation(
      const UdpAddressType &clientAddress,
      ErrorCode errorCode,
      const string &errorMessage = string()) = 0;

  protected:
    //! Protected constructor
    TftpServer() = default;
};

}
}

#endif
