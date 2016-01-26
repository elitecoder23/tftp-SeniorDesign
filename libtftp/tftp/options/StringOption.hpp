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
 * @brief Declaration of class StringOption.
 **/

#ifndef TFTP_OPTIONS_STRINGOPTION_HPP
#define TFTP_OPTIONS_STRINGOPTION_HPP

#include <tftp/options/Option.hpp>

#include <string>

namespace Tftp {
namespace Options {

using std::string;

/**
 * @brief TFTP option, which is interpreted as string.
 **/
class StringOption: public Option
{
	public:
		/**
		 * @brief Generates an option with the given name an value.
		 *
		 * @param[in] name
		 *   The option name.
		 * @param[in] value
		 *   The option value.
		 **/
		StringOption( const string &name, const string &value);

		//! @copybrief Option::getValueString()
		virtual string getValueString( void) const override;

		/**
		 * @brief Sets the option value.
		 *
		 * @param[in] value
		 *   The option value.
		 **/
		void setValue( const string &value);

		/**
		 * @copydoc Option::negotiateServer()
		 *
		 * String options cannot be negotiated.
		 * This operation always return an empty option pointer.
		 *
		 * @return Always an empty option pointer.
		 **/
		virtual OptionPointer negotiateServer(
			const string &optionValue) const override;

		/**
		 * @copydoc Option::negotiateClient()
		 *
		 * String options cannot be negotiated.
		 * This operation always return an empty option pointer.
		 *
		 * @return Always an empty option pointer.
		 **/
		virtual OptionPointer negotiateClient(
			const string &optionValue) const override;

	private:
		//! The option value.
		string value;
};
}
}

#endif
