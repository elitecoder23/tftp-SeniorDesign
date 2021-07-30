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

#include <tftp/TftpOptionsConfiguration.hpp>

#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/program_options/options_description.hpp>

#include <cstdint>

namespace Tftp {

/**
 * @brief Encapsulates common TFTP options, which can be loaded via a
 *   boost::property_tree::ptree.
 **/
class TftpConfiguration
{
  public:
    /**
     * @brief Initialises the Configuration with Default Values.
     *
     * @param[in] defaultTftpPort
     *   Default TFTP Port used for Configuration
     **/
    TftpConfiguration( uint16_t defaultTftpPort = DefaultTftpPort ) noexcept;

    /**
     * @brief Loads the configuration via a boost::property_tree::ptree.
     *
     * @param[in] config
     *   Stored configuration.
     * @param[in] defaultTftpPort
     *   Default TFTP Port used for Configuration
     **/
    explicit TftpConfiguration(
      const boost::property_tree::ptree &config,
      uint16_t defaultTftpPort = DefaultTftpPort );

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

    //! TFTP timeout - The standard when no timeout option is negotiated in seconds.
    uint8_t tftpTimeout;
    //! Number of Retries.
    uint16_t tftpRetries;

    //! UDP Port used for TFTP Communication.
    uint16_t tftpServerPort;

    //! Options Configuration
    TftpOptionsConfiguration tftpOptions;
};

}

#endif
