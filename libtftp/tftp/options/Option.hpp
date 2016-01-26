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
 * @brief Declaration of class Tftp::Options::Option.
 **/

#ifndef TFTP_OPTIONS_OPTION_HPP
#define TFTP_OPTIONS_OPTION_HPP

#include <tftp/options/Options.hpp>

#include <string>

namespace Tftp {
namespace Options {

using std::string;

/**
 * @brief Base class of a TFTP option.
 *
 * A TFTP option is a key - value association.
 **/
class Option
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
		static string getOptionName( const TftpOptions option) noexcept;

		/**
		 * @brief Generate TFTP option with the given name.
		 *
		 * @param[in] name
		 *   The option name.
		 **/
		Option( const string &name);

		//! Default destructor
		virtual ~Option( void) noexcept = default;

		/**
		 * @brief Returns the option name
		 *
		 * @return The option name.
		 **/
		string getName( void) const;

		/**
		 * @brief Set the option name
		 *
		 * @param[in] name
		 *   The new option name
		 **/
		void setName( const string &name);

		/**
		 * @brief Returns the option value as string.
		 *
		 * @return The option value as string.
		 **/
		virtual string getValueString( void) const = 0;

		/**
		 * @brief Option negotiation on server side.
		 *
		 * This function is called, when the TFTP server receives options within
		 * RRQ/ WRQ packets.
		 * The Server is allowed to modify the option value to a value, which is
		 * accepted by the client.
		 *
		 * @param[in] optionValue
		 *   The input option
		 *
		 * @return Option negotiation result
		 **/
		virtual OptionPointer negotiateServer( const string &optionValue) const = 0;

		/**
		 * @brief Option negotiation on client side.
		 *
		 * This function is called, when the TFTP client receives options within
		 * a OACK packet.
		 * The client will or will not accept the received option value.
		 *
		 * @param[in] optionValue
		 *   The input option
		 *
		 * @return Option negotiation result
		 * @retval OptionPointer()
		 *   Option negotiation failed on client side.
		 *   Error packet will be sent.
		 **/
		virtual OptionPointer negotiateClient( const string &optionValue) const = 0;

		/**
		 * @brief Returns a string, which describes the option.
		 *
		 * This operation is used for debugging and information purposes.
		 *
		 * @return Option list description.
		 **/
		virtual string toString( void) const;

	private:
		//! The option name
		string name;
};

}
}

#endif
