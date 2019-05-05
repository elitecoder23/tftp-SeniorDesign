/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::File::TftpFile.
 **/

#ifndef TFTP_FILE_TFTPFILE_HPP
#define TFTP_FILE_TFTPFILE_HPP

#include <tftp/ReceiveDataHandler.hpp>
#include <tftp/TransmitDataHandler.hpp>

namespace Tftp::File {

/**
 * @brief Base class for a TFTP file operation handler, which can be used to
 * transfer files
 **/
class TftpFile:
  public ReceiveDataHandler,
  public TransmitDataHandler
{
};

}

#endif
