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
 * @brief Declaration of class Tftp::TftpReadOperationHandler.
 **/

#ifndef TFTP_TFTPRECEIVEDATAOPERATIONHANDLER_HPP
#define TFTP_TFTPRECEIVEDATAOPERATIONHANDLER_HPP

#include <tftp/Tftp.hpp>
#include <tftp/TftpOperationHandler.hpp>

namespace Tftp {

/**
 * @brief Class interface, which must be implemented by an class, which
 *   wants to receive data from a TFTP operation (TFTP client RRQ or TFTP
 *   server WRQ)
 **/
class TftpReceiveDataOperationHandler: public TftpOperationHandler
{
  public:
    /**
     * @brief This call-back is executed, when the transfer size of the data
     *   to be transmitted is received.
     *
     * The call to this call-back is optional.
     *
     * @param[in] transferSize
     *   The received transfer size.
     *
     * @return If the handler can handle this amount of data
     * @retval true
     *   The received amount of data can be handled - operation should be
     *   continued.
     * @retval false
     *   The received amount of data cannot be handled - the operation should
     *   be aborted.
     **/
    virtual bool receivedTransferSize( uint64_t transferSize) = 0;

    /**
     * @brief Data has been received and must be processed.
     *
     * @param[in] data
     *   The received data.
     **/
    virtual void receviedData( const DataType &data) noexcept = 0;
};

}

#endif
