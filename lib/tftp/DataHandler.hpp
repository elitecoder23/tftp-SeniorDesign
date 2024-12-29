// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::DataHandler.
 **/

#ifndef TFTP_DATAHANDLER_HPP
#define TFTP_DATAHANDLER_HPP

#include <tftp/Tftp.hpp>

#include <cstddef>
#include <span>
#include <vector>

namespace Tftp {

/**
 * @brief TFTP Data Handler.
 **/
class TFTP_EXPORT DataHandler
{
  public:
    //! Data Type.
    using Data = std::vector< std::byte >;
    //! Data Span.
    using ConstDataSpan = std::span< const std::byte >;

    /**
     * @brief Destructor.
     **/
    virtual ~DataHandler() noexcept = default;

    /**
     * @brief Data Handler Reset Request.
     *
     * Is called once before any data is requested or provided.
     * Should reset the data handler.
     **/
    virtual void reset() = 0;

    /**
     * @brief Called, when the operation has been finished.
     **/
    virtual void finished() = 0;
};

}

#endif
