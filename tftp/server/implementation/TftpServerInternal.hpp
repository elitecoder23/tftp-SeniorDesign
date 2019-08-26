/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of interface class Tftp::Server::TftpServerInternal.
 **/

#ifndef TFTP_SERVER_TFTPSERVERINTERNAL_HPP
#define TFTP_SERVER_TFTPSERVERINTERNAL_HPP

#include <tftp/server/Server.hpp>
#include <tftp/server/TftpServer.hpp>
#include <tftp/options/Options.hpp>

namespace Tftp::Server {

/**
 * @brief This interface class defines the methods used internally for TFTP
 *   Server interrogation.
 **/
class TftpServerInternal : public TftpServer
{
  public:
    //! Destructor
    ~TftpServerInternal() override = default;

    /**
     * @brief Returns the TFTP configuration.
     *
     * @return The TFTP configuration.
     **/
    [[nodiscard]] virtual const TftpConfiguration& configuration() const = 0;

    /**
     * @brief Returns the configured TFTP Options.
     *
     * @return The configured TFTP Options.
     **/
    [[nodiscard]] virtual const Options::OptionList& options() const = 0;
};

}

#endif
