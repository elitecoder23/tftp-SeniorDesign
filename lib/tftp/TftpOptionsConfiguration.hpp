// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::TftpOptionsConfiguration.
 **/

#ifndef TFTP_TFTPOOPTIONSCONFIGURATION_HPP
#define TFTP_TFTPOOPTIONSCONFIGURATION_HPP

#include <tftp/Tftp.hpp>

#include <boost/optional.hpp>

#include <boost/property_tree/ptree_fwd.hpp>

#include <boost/program_options/options_description.hpp>

#include <chrono>
#include <optional>

namespace Tftp {

/**
 * @brief TFTP Options Configuration.
 *
 * A TFTP option is a name - value pair.
 *
 * The option list also contains handler of common TFTP options like:
 * - block size option (RFC 2348)
 * - timeout option (RFC 2349)
 * - transfer size option (RFC 2349)
 *
 * @sa TftpConfiguration
 **/
class TFTP_EXPORT TftpOptionsConfiguration
{
  public:
    /**
     * @brief Initialises the Configuration with Default Values.
     **/
    TftpOptionsConfiguration() noexcept = default;

    /**
     * @brief Loads the Configuration via a Property Tree.
     *
     * @param[in] properties
     *   Stored TFTP Option Configuration.
     **/
    explicit TftpOptionsConfiguration( const boost::property_tree::ptree &properties );

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
     * @param[in] full
     *   If set to true, all options are added to the property tree, even if defaulted.
     *
     * @return Configuration represented as Property Tree.
     **/
    [[nodiscard]] boost::property_tree::ptree toProperties( bool full = false ) const;

    /**
     * @brief Returns an option description, which can be used to parse a command line.
     *
     * @return TFTP Options Configuration
     **/
    [[nodiscard]] boost::program_options::options_description options();

    //! If set, the client/ server shall handle the "Transfer Size" option
    bool handleTransferSizeOption{ false };

    //! If set, this value is used for option negotiation
    boost::optional< uint16_t > blockSizeOption;

    //! If set, this value is used for option negotiation
    boost::optional< std::chrono::seconds > timeoutOption;
};

}

#endif
