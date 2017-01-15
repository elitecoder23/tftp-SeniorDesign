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
 * @brief Declaration of namespace Tftp::Client.
 **/

#ifndef TFTP_CLIENT_CLIENT_HPP
#define TFTP_CLIENT_CLIENT_HPP

#include <tftp/Tftp.hpp>

#include <memory>
#include <functional>

namespace Tftp {

/**
 * @brief Implementation of TFTP client functionality.
 *
 * The main entry point for users of this part of the TFTP library is the
 * class TftpClient. With an instance of this class the user can create
 * TFTP client read and write operations.
 **/
namespace Client {

class TftpClient;

//! Declaration of TFTP client instance pointer
using TftpClientPtr = std::shared_ptr< TftpClient>;

//! Declaration of TFTP client operation
using TftpClientOperation = std::function< void()>;

}
}

#endif
