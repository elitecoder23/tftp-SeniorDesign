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
 * @brief Declaration of interface class Tftp::Server::TftpServerInternal.
 **/

#ifndef TFTP_SERVER_TFTPSERVERINTERNAL_HPP
#define TFTP_SERVER_TFTPSERVERINTERNAL_HPP

#include <tftp/server/Server.hpp>
#include <tftp/options/Options.hpp>

namespace Tftp {
namespace Server {

/**
 * @brief This interface class defines the methods used internally for TFTP
 *   Server interrogation.
 **/
class TftpServerInternal
{
  public:
    using OptionList = Options::OptionList;

    //! Default destructor
    virtual ~TftpServerInternal() = default;

    virtual const TftpConfiguration& getConfiguration() const = 0;

    virtual const OptionList& getOptionList() const = 0;
};

}
}

#endif
