/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * $Date$
 * $Revision$
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class TftpConfiguration.
 **/

#ifndef TFTP_TFTPCONFIGURATION_HPP
#define TFTP_TFTPCONFIGURATION_HPP

#include <tftp/Tftp.hpp>

#include <tftp/options/OptionList.hpp>

#include <boost/property_tree/ptree.hpp>

#include <cstdint>

namespace Tftp
{
	using Tftp::Options::OptionList;

	/**
	 * @brief Encapsulates common TFTP options, which can be loaded via a
	 *   boost::property_tree::ptree.
	 **/
	class TftpConfiguration
	{
		public:
			//! Loads the configuration with default values.
			TftpConfiguration( void);

			/**
			 * @brief Loads the configuration via a boost::property_tree::ptree.
			 *
			 * @param[in] properties
			 *   The stored properties.
			 **/
			TftpConfiguration( const boost::property_tree::ptree &properties);

			/**
			 * @brief Converts the configuration values to a
			 *   boost::property_tree::ptree.
			 *
			 * @return The boost::property_tree::ptree.
			 **/
			boost::property_tree::ptree toProperties( void) const;

			/**
			 * @brief Creates an option list (for TFTP clients) based on the actual
			 *   configuration and the supplied base options.
			 *
			 * @param[in] baseOptions
			 *   Base options, which shall be used for creation of this option list.
			 *
			 * @return Option list (for TFTP clients) based on the actual
			 *   configuration and the supplied base options.
			 **/
			OptionList getClientOptions(
				const OptionList &baseOptions = OptionList()) const;

			/**
			 * @brief Creates an option list (for TFTP servers) based on the actual
			 *   configuration and the supplied base options.
			 *
			 * @param[in] baseOptions
			 *   Base options, which shall be used for creation of this option list.
			 *
			 * @return Option list (for TFTP servers) based on the actual
			 *   configuration and the supplied base options.
			 **/
			OptionList getServerOptions(
				const OptionList &baseOptions = OptionList()) const;

			//! The TFTP timeout
			uint16_t tftpTimeout;
			//! Number of retries.
			uint16_t tftpRetries;

			//! The port used for TFTP communication.
			uint16_t tftpServerPort;

			//! If set, the client/ server shall handle the "Transfer Size" option
			bool handleTransferSizeOption;

			//! If set, the client/ server shall handle the "Block Size" option
			bool handleBlockSizeOption;
			//! If handleBlockSizeOption is set, this value is used for option negotiation
			uint16_t blockSizeOptionValue;

			//! If set, the client/ server shall handle the "Timeout" option
			bool handleTimeoutOption;
			//! If handleTimeoutOption is set, this value is used for option negotiation
			uint16_t timoutOptionValue;
	};
}
#endif
