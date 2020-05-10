/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::TftpConfiguration.
 **/

#ifndef TFTP_TFTPCONFIGURATION_HPP
#define TFTP_TFTPCONFIGURATION_HPP

#include <tftp/Tftp.hpp>

#include <tftp/options/OptionList.hpp>

#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/optional.hpp>

#include <cstdint>

namespace Tftp {

/**
 * @brief Encapsulates common TFTP options, which can be loaded via a
 *   boost::property_tree::ptree.
 **/
class TftpConfiguration
{
  public:
    //! Initialises the configuration with default values.
    TftpConfiguration() noexcept;

    /**
     * @brief Loads the configuration via a boost::property_tree::ptree.
     *
     * @param[in] config
     *   The stored configuration.
     **/
    explicit TftpConfiguration( const boost::property_tree::ptree &config);

    /**
     * @brief Converts the configuration values to a
     *   boost::property_tree::ptree.
     *
     * @return The boost::property_tree::ptree.
     **/
    [[nodiscard]] boost::property_tree::ptree toProperties() const;

    /**
     * @brief Returns an option description, which can be used to parse a
     *   command line.
     *
     * @return
     **/
    [[nodiscard]] boost::program_options::options_description options();

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
     * @param[in] baseOptions
     *   Base options, which shall be used for creation of this option list.
     *
     * @return Option list (for TFTP clients) based on the actual
     *   configuration and the supplied base options.
     **/
    [[nodiscard]] Options::OptionList clientOptions(
      const Options::OptionList &baseOptions = {}) const;

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
    [[nodiscard]] Options::OptionList serverOptions(
      const Options::OptionList &baseOptions = {}) const;

    //! TFTP timeout - The standard when no timeout option is negotiated in seconds.
    uint8_t tftpTimeout;
    //! Number of Retries.
    uint16_t tftpRetries;

    //! UDP Port used for TFTP Communication.
    uint16_t tftpServerPort;

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
