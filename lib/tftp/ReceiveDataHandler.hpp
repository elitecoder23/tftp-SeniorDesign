// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::ReceiveDataHandler.
 **/

#ifndef TFTP_RECEIVEDATAHANDLER_HPP
#define TFTP_RECEIVEDATAHANDLER_HPP

#include "tftp/Tftp.hpp"
#include "tftp/DataHandler.hpp"

namespace Tftp {

/**
 * @brief Receive Data Handler
 *
 * Class interface, which must be implemented by an class, which wants to
 * receive data from a TFTP operation (TFTP client RRQ or TFTP server WRQ)
 **/
class TFTP_EXPORT ReceiveDataHandler : public virtual DataHandler
{
  public:
    /**
     * @brief This call-back is executed, when the transfer size of the data
     *   to be transmitted is received.
     *
     * This call-back is called optional, when a transfer size option has been
     * received.
     *
     * @param[in] transferSize
     *   Received transfer size.
     *
     * @return If the handler can handle this amount of data
     * @retval true
     *   Received amount of data can be handled - operation should be continued.
     * @retval false
     *   Received amount of data cannot be handled - the operation should be
     *   aborted.
     **/
    [[nodiscard]] virtual bool receivedTransferSize(
      uint64_t transferSize ) = 0;

    /**
     * @brief Data has been received and must be processed.
     *
     * @param[in] data
     *   Received data.
     **/
    virtual void receivedData( DataSpan data ) noexcept = 0;
};

}

#endif
