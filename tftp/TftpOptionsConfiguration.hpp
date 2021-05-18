/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::TftpOptionsConfiguration.
 **/

#ifndef TFTP_TFTPOOPTIONSCONFIGURATION_HPP
#define TFTP_TFTPOOPTIONSCONFIGURATION_HPP

#include <tftp/Tftp.hpp>

#include <boost/optional.hpp>

namespace Tftp {

/**
 * @brief TFTP Option List.
 *
 * A TFTP option is a name - value pair.
 *
 * The option list also contains handler of common TFTP options like:
 * - blocksize option (RFC 2348)
 * - timeout option (RFC 2349)
 * - transfer size option (RFC 2349)
 **/
class TftpOptionsConfiguration
{
  public:
    /**
     * @brief Returns the option string for the given option.
     *
     * @param[in] option
     *   The TFTP option.
     *
     * @return Returns the option name.
     **/
    static std::string_view optionName( KnownOptions option) noexcept;

    /**
     * @brief Creates an option list (for TFTP clients) based on the actual
     *   configuration and the supplied base options.
     *
     * This operation handles:
     * - block size option,
     * - timeout option, and
     * - transfer size option.
     *
     * The transfer size option must is handled in a special way.
     * It is set to '0' to be checked by RRQ/ WRQ operations.
     *
     * @return Option list (for TFTP clients) based on the actual
     *   configuration and the supplied base options.
     **/
    [[nodiscard]] Options clientOptions() const;

    /**
     * @brief Creates an option list (for TFTP servers) based on the actual
     *   configuration and the supplied base options.
     *
     * This operation handles:
     * - block size option,
     * - timeout option, and
     * - transfer size option.
     *
     * The transfer size option must is handled in a special way.
     * It is set to '0' to be checked by RRQ/ WRQ operations.
     *
     * @param[in] baseOptions
     *   Base options, which shall be used for creation of this option list.
     *
     * @return Option list (for TFTP servers) based on the actual
     *   configuration and the supplied base options.
     **/
    [[nodiscard]] Options serverOptions() const;

    //! If set, the client/ server shall handle the "Transfer Size" option
    bool handleTransferSizeOption;

    //! If handleBlockSizeOption is set, this value is used for option negotiation
    boost::optional< uint16_t> blockSizeOption;

    /** If handleTimeoutOption is set, this value is used for option negotiation
     * @note Even if timeout is only uint8_t we make it 16bit for parsing.
     **/
    boost::optional< uint16_t> timeoutOption;

};

}

#endif
