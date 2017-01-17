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
 * @brief Declaration of class Tftp::TftpOperationHandler.
 **/

#ifndef TFTP_TFTPOPERATIONHANDLER_HPP
#define TFTP_TFTPOPERATIONHANDLER_HPP

#include <tftp/Tftp.hpp>

#include <vector>
#include <cstdint>

namespace Tftp {

/**
 * @brief Base class for TFTP operation handlers.
 **/
class TftpOperationHandler
{
  public:
    //! The data type is used for the inherited classes.
    using DataType = std::vector< uint8_t>;

    /**
     * @brief Default destructor.
     **/
    virtual ~TftpOperationHandler() noexcept = default;

    /**
     * @brief Called, when the operation has been finished
     **/
    virtual void finishedOperation() noexcept = 0;
};

}

#endif
