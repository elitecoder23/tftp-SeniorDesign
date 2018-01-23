/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of interface class Tftp::Client::TftpClientInternal.
 **/

#ifndef TFTP_CLIENT_TFTPCLIENTINTERNAL_HPP
#define TFTP_CLIENT_TFTPCLIENTINTERNAL_HPP

#include <tftp/server/Server.hpp>
#include <tftp/client/TftpClient.hpp>
#include <tftp/options/Options.hpp>

namespace Tftp {
namespace Client {

/**
 * @brief This interface class defines the methods used internally for TFTP
 *   client interrogation.
 **/
class TftpClientInternal : public TftpClient
{
  public:
    //! Default destructor
    virtual ~TftpClientInternal() = default;

    /**
     * @brief Returns the used TFTP configuration.
     *
     * @return The TFTP configuration
     **/
    virtual const TftpConfiguration& configuration() const = 0;

    /**
     * @brief Returns the own options list.
     *
     * @return The options list.
     **/
    virtual const Options::OptionList& options() const = 0;
};

}
}

#endif
