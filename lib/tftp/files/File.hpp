// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Files::File.
 **/

#ifndef TFTP_FILES_FILE_HPP
#define TFTP_FILES_FILE_HPP

#include <tftp/files/Files.hpp>

#include <tftp/ReceiveDataHandler.hpp>
#include <tftp/TransmitDataHandler.hpp>

namespace Tftp::Files {

/**
 * @brief TFTP %File.
 *
 * Base class for a TFTP file operation handler, which can be used to transfer files.
 **/
class TFTP_EXPORT File : public ReceiveDataHandler, public TransmitDataHandler
{
  public:
    //! Type of Operation
    enum class Operation
    {
      //! Receive Operation
      Receive,
      //! Transmit Operation
      Transmit
    };
};

}

#endif
