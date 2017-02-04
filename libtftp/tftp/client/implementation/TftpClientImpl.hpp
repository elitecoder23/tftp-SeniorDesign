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
 * @brief Declaration of class Tftp::Client::TftpClientImpl.
 **/

#ifndef TFTP_CLIENT_TFTPCLIENTIMPL_HPP
#define TFTP_CLIENT_TFTPCLIENTIMPL_HPP

#include <tftp/client/Client.hpp>
#include <tftp/client/implementation/TftpClientInternal.hpp>

#include <tftp/options/OptionList.hpp>

#include <tftp/TftpConfiguration.hpp>

namespace Tftp {
namespace Client {

/**
 * @brief Implementation of TFTP Client factory interface.
 *
 * This factory class creates on request the concrete client operations.
 **/
class TftpClientImpl : public TftpClientInternal
  {
  public:
    /**
     * @brief Creates the concrete TFTP client.
     *
     * @param[in] configuration
     *   The TFTP Configuration
     * @param[in] additionalOptions
     *   Additional Options, which shall be used as TFTP client option list.
     **/
    TftpClientImpl(
      const TftpConfiguration &configuration,
      const Options::OptionList& additionalOptions);

    //!@copydoc TftpClient::createReadRequestOperation(ReceiveDataOperationHandler &,const UdpAddressType &,const string &, TransferMode,const UdpAddressType &)
    virtual TftpClientOperationPtr createReadRequestOperation(
      ReceiveDataOperationHandler &handler,
      const UdpAddressType &serverAddress,
      const string &filename,
      TransferMode mode,
      const UdpAddressType &from) override final;

    //!@copydoc TftpClient::createReadRequestOperation(ReceiveDataOperationHandler &,const UdpAddressType &,const string &, TransferMode)
    virtual TftpClientOperationPtr createReadRequestOperation(
      ReceiveDataOperationHandler &handler,
      const UdpAddressType &serverAddress,
      const string &filename,
      TransferMode mode) override final;

    //!@copydoc TftpClient::createWriteRequestOperation(TransmitDataOperationHandler &,const UdpAddressType &,const string &, TransferMode,const UdpAddressType &)
    virtual TftpClientOperationPtr createWriteRequestOperation(
      TransmitDataOperationHandler &handler,
      const UdpAddressType &serverAddress,
      const string &filename,
      TransferMode mode,
      const UdpAddressType &from) override final;

    //!@copydoc TftpClient::createWriteRequestOperation(TransmitDataOperationHandler &,const UdpAddressType &,const string &, TransferMode)
    virtual TftpClientOperationPtr createWriteRequestOperation(
      TransmitDataOperationHandler &handler,
      const UdpAddressType &serverAddress,
      const string &filename,
      TransferMode mode) override final;

    //! @copydoc TftpClientInternal::getConfiguration
    virtual const TftpConfiguration& getConfiguration() const override final;

    //! @copydoc TftpClientInternal::getOptionList
    virtual const Options::OptionList& getOptionList() const override final;

  private:
    const TftpConfiguration configuration;
    const Options::OptionList options;
};

}
}

#endif
