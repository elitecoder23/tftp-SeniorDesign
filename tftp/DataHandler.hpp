/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::DataHandler.
 **/

#ifndef TFTP_DATAHANDLER_HPP
#define TFTP_DATAHANDLER_HPP

#include <tftp/Tftp.hpp>

#include <vector>
#include <cstdint>

namespace Tftp {

/**
 * @brief TFTP Data Handler.
 **/
class DataHandler
{
  public:
    //! Data Type.
    using DataType = std::vector< uint8_t>;

    /**
     * @brief Destructor.
     **/
    virtual ~DataHandler() noexcept = default;

    /**
     * @brief Data Handler Reset Request.
     *
     * Is called once before any data is requested or provided.
     * Should reset the fata handler.
     **/
    virtual void reset() = 0;

    /**
     * @brief Called, when the operation has been finished.
     **/
    virtual void finished() noexcept = 0;
};

}

#endif
