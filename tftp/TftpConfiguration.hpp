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

#include <boost/property_tree/ptree_fwd.hpp>

#include <boost/program_options/options_description.hpp>

#include <cstdint>
#include <chrono>

namespace Tftp {

/**
 * @brief TFTP Configuration Parameters.
 *
 * Configuration can be load/ stored as Property Tree and Command Line
 * Parameters.
 **/
class TftpConfiguration
{
  public:
    /**
     * @brief Initialises the Configuration with Default Values.
     *
     * @param[in] defaultTftpPort
     *   Default TFTP Port used for Configuration.
     **/
    explicit TftpConfiguration(
      uint16_t defaultTftpPort = DefaultTftpPort ) noexcept;

    /**
     * @brief Loads the configuration via a Property Tree.
     *
     * @param[in] properties
     *   Stored Configuration.
     * @param[in] defaultTftpPort
     *   Default TFTP Port used for Configuration.
     **/
    explicit TftpConfiguration(
      const boost::property_tree::ptree &properties,
      uint16_t defaultTftpPort = DefaultTftpPort );

    /**
     * @brief Load Configuration from given Property Tree.
     *
     * @param[in] properties
     *   Configuration as Property Tree
     **/
    void fromProperties( const boost::property_tree::ptree &properties );

    /**
     * @brief Converts the configuration values to a Property Tree.
     *
     * @return Configuration as Property Tree.
     **/
    [[nodiscard]] boost::property_tree::ptree toProperties() const;

    /**
     * @brief Returns an option description, which can be used to parse a
     *   command line.
     *
     * @return TFTP Configuration Options Description.
     **/
    [[nodiscard]] boost::program_options::options_description options();

    //! TFTP timeout - standard when no timeout option is negotiated in seconds.
    std::chrono::seconds tftpTimeout{ DefaultTftpReceiveTimeout };
    //! Number of Retries.
    uint16_t tftpRetries{ DefaultTftpRetries };

    //! UDP Port used for TFTP Communication.
    uint16_t tftpServerPort;

    //! Dally Option
    bool dally{ false };

  private:
    //! Default TFTP Port ( can be overridden by configuration)
    //! This value is used for loading and storing to determine the default
    //! value.
    const uint16_t defaultTftpPort;
};

}

#endif
