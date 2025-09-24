// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::TransmitDataHandler.
 **/

#ifndef TFTP_TRANSMITDATAHANDLER_HPP
#define TFTP_TRANSMITDATAHANDLER_HPP

#include <tftp/Tftp.hpp>
#include <tftp/DataHandler.hpp>

#include <optional>

namespace Tftp {

/**
 * @brief Transmit Data Handler
 *
 * Class interface, which must be implemented by a class, which wants to transmit data for a TFTP operation.
 * These operations are:
 * - TFTP client WRQ, or
 * - TFTP server RRQ.
 **/
class TFTP_EXPORT TransmitDataHandler : public virtual DataHandler
{
  public:
    /**
     * @brief This call-back is executed when the transfer size of the data to be transmitted is requested and the
     *   transfer size option is set.
     *
     * The call to this call-back is optional.
     *
     * @return The transfer size, if it can be provided
     * @retval {}
     *   If the transfer size of the data cannot be provided.
     **/
    [[nodiscard]] virtual std::optional< uint64_t > requestedTransferSize() = 0;

    /**
     * @brief Request for data, which will be transmitted.
     *
     * The operation must return a @p std::vector with data, which is transmitted to the other side.
     * The parameter @p maxSize defines the maximum data size, which can be transmitted.
     *
     * The data buffer must not be greater than the max size as defined by the parameter maxSize.
     *
     * If the data buffer is smaller than @p maxSize (also empty is allowed) this will be the last packet (EOF).
     *
     * @param[in] maxSize
     *   The maximum size, which shall be returned.
     *
     * @return The data, which will be transmitted.
     **/
    [[nodiscard]] virtual Helper::RawData sendData( std::size_t maxSize ) = 0;
};

}

#endif
