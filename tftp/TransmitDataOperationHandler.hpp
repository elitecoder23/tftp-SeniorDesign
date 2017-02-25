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
 * @brief Declaration of class Tftp::TransmitDataOperationHandler.
 **/

#ifndef TFTP_TRANSMITDATAOPERATIONHANDLER_HPP
#define TFTP_TRANSMITDATAOPERATIONHANDLER_HPP

#include <tftp/Tftp.hpp>
#include <tftp/OperationHandler.hpp>

namespace Tftp {

/**
 * @brief Class interface, which must be implemented by an class, which
 *   wants to transmit data for a TFTP operation (TFTP client WRQ or TFTP
 *   server RRQ)
 **/
class TransmitDataOperationHandler: public OperationHandler
{
  public:
    /**
     * @brief This call-back is executed, when the transfer size of the data
     *   to be transmitted is requested (TRANSFERSIZE option set).
     *
     * The call to this call-back is optional.
     *
     * @param[out] transferSize
     *   The transfer size.
     *
     * @return If the transfer size of the data can be provided.
     **/
    virtual bool requestedTransferSize( uint64_t &transferSize) = 0;

    /**
     * @brief Request for data, which will be transmitted.
     *
     * The operation must return a std::vector with data, which is transmitted
     * to the other side. The parameter maxSize defines the maximum data size,
     * which can be transmitted.
     *
     * The data buffer must not be greater then the max size as defined by the
     * parameter maxSize.
     *
     * If the dataBuffer is smaller than maxSize (also empty is allowed) this
     * will be the last packet (EOF).
     *
     * @param[in] maxSize
     *   The maximum size, which shall be returned.
     *
     * @return The data, which will be transmitted.
     **/
    virtual DataType sendData( size_t maxSize) noexcept = 0;
};

}

#endif
