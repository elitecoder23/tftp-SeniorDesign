/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::Client::TftpClient.
 **/

#ifndef TFTP_CLIENT_TFTPCLIENT_HPP
#define TFTP_CLIENT_TFTPCLIENT_HPP

#include <tftp/TftpConfiguration.hpp>
#include <tftp/client/Client.hpp>
#include <tftp/options/Options.hpp>

#include <string>

namespace Tftp {
namespace Client {

/**
 * @brief The main entry for implementors of a TFTP client.
 *
 * This class acts as factory for creating client operations, like read
 * requests or write requests.
 **/
class TftpClient
{
  public:
    /**
     * @brief Creates an instance of TFTP client.
     *
     * With the instance you can create any client operation instances.
     *
     * @param[in] configuration
     *   The TFTP Configuration
     * @param[in] additionalOptions
     *   Additional Options, which shall be used as TFTP client option list.
     *
     * @return The created TFTP client instance.
     **/
    static TftpClientPtr createInstance(
      const TftpConfiguration &configuration = {},
      const Options::OptionList& additionalOptions = {});

    //! Default destructor
    virtual ~TftpClient() noexcept = default;

    /**
     * @brief Entry of the TFTP Client.
     *
     * This routines enters the IO Service loop.
     * The start routine will be leaved, when an FATAL error occurred or
     * the server has been stopped by calling stop().
     *
     * This entry can be called multiple time to allow parallel transfer handling
     **/
    virtual void entry() noexcept = 0;

    /**
     * @brief Starts the TFTP Client.
     *
     * This routine returns immediately.
     *
     * It can be called before entry() is called.
     **/
    virtual void stop() = 0;

    /**
     * @brief Initialises the operation.
     *
     * @param[in] dataHandler
     *   Handler for received data.
     * @param[in] completionHandler
     *   The handler which is called on completion of the operation.
     * @param[in] remote
     *   Where the connection should be established to.
     * @param[in] filename
     *   Which file shall be requested
     * @param[in] mode
     *   The transfer mode
     * @param[in] local
     *   Parameter to define the communication source
     *
     * @return The client operation instance.
     **/
    virtual OperationPtr createReadRequestOperation(
      ReceiveDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const UdpAddressType &remote,
      const std::string &filename,
      TransferMode mode,
      const UdpAddressType &local = UdpAddressType{}) = 0;

    /**
     * @brief Creates an write request operation (TFTP WRQ).
     *
     * @param[in] dataHandler
     *   Handler for data.
     * @param[in] completionHandler
     *   The handler which is called on completion of the operation.
     * @param[in] remote
     *   Where the connection should be established to.
     * @param[in] filename
     *   Which file shall be requested
     * @param[in] mode
     *   The transfer mode
     * @param[in] local
     *   Parameter to define the communication source
     *
     * @return The client operation instance.
     **/
    virtual OperationPtr createWriteRequestOperation(
      TransmitDataHandlerPtr dataHandler,
      OperationCompletedHandler completionHandler,
      const UdpAddressType &remote,
      const std::string &filename,
      TransferMode mode,
      const UdpAddressType &local = UdpAddressType{}) = 0;

  protected:
    //! Protected constructor.
    TftpClient() = default;
};

}
}

#endif
