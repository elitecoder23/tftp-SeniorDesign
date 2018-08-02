/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::TftpConfiguration.
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
    TftpConfiguration();

    /**
     * @brief Loads the configuration via a boost::property_tree::ptree.
     *
     * @param[in] config
     *   The stored configuration.
     **/
    TftpConfiguration( const boost::property_tree::ptree &config);

    /**
     * @brief Converts the configuration values to a
     *   boost::property_tree::ptree.
     *
     * @return The boost::property_tree::ptree.
     **/
    boost::property_tree::ptree toProperties() const;

    /**
     * @brief Returns an option description, which can be used to parse a
     *   command line.
     *
     * @return
     **/
    boost::program_options::options_description options();

    /**
     * @brief Creates an option list (for TFTP clients) based on the actual
     *   configuration and the supplied base options.
     *
     * This operation handles:
     * * blocksize option and
     * * timeout option.
     *
     * The transfer size option must be handled manually, due to the different
     * handling on RRG/ WRQ.
     *
     * @param[in] baseOptions
     *   Base options, which shall be used for creation of this option list.
     *
     * @return Option list (for TFTP clients) based on the actual
     *   configuration and the supplied base options.
     **/
    Options::OptionList clientOptions(
      const Options::OptionList &baseOptions = {}) const;

    /**
     * @brief Creates an option list (for TFTP servers) based on the actual
     *   configuration and the supplied base options.
     *
     * This operation handles:
     * * blocksize option and
     * * timeout option.
     *
     * The transfersize option must be handled manually, due to the different
     * handling on RRG/ WRQ.
     *
     * @param[in] baseOptions
     *   Base options, which shall be used for creation of this option list.
     *
     * @return Option list (for TFTP servers) based on the actual
     *   configuration and the supplied base options.
     **/
    Options::OptionList serverOptions(
      const Options::OptionList &baseOptions = {}) const;

    //! The TFTP timeout - The standard when no timeout option is negotiated in seconds.
    uint8_t tftpTimeout;
    //! Number of retries.
    uint16_t tftpRetries;

    //! The port used for TFTP communication.
    uint16_t tftpServerPort;

    //! If set, the client/ server shall handle the "Transfer Size" option
    bool handleTransferSizeOption;

    //! If handleBlockSizeOption is set, this value is used for option negotiation
    boost::optional< uint16_t> blockSizeOption;

    //! If handleTimeoutOption is set, this value is used for option negotiation
    boost::optional< uint8_t> timeoutOption;
};

}

#endif
