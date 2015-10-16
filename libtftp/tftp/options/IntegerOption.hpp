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
 * @brief Declaration/ definition of template class IntegerOption
 **/

#ifndef TFTP_OPTIONS_INTEGEROPTION_HPP
#define TFTP_OPTIONS_INTEGEROPTION_HPP

#include <tftp/options/Option.hpp>

#include <boost/lexical_cast.hpp>

namespace Tftp
{
	namespace Options
	{
		using std::string;

		/**
		 * @brief TFTP option, which is interpreted as integer.
		 *
		 * @note
		 * Do not specialise this class to uint8_t (unsigned char) or int8_t (char)!
		 * This would fail because C++ internally handles strings as this type.
		 **/
		template< typename IntT>
		class IntegerOption : public Option
		{
			public:
				//! The used integer type
				typedef IntT IntegerType;

				/**
				 * @brief Generates a option with the given parameters.
				 *
				 * @param[in] name
				 *   Option name
				 * @param[in] minValue
				 *   minimum allowed value
				 * @param[in] maxValue
				 *   maximum allowed value
				 * @param[in] value
				 *   Current value
				 **/
				IntegerOption(
					const string &name,
					const IntegerType minValue,
					const IntegerType maxValue,
					const IntegerType value);

				/**
				 * @brief returns the current value.
				 *
				 * @return The current option value
				 **/
				IntegerType getValue( void) const;

				/**
				 * @brief Returns the option value as string.
				 *
				 * @return The option value as string.
				 **/
				virtual string getValueString( void) const override;

				/**
				 * @brief Sets the value of the option.
				 *
				 * @param[in] value
				 *   Option value
				 **/
				void setValue( const IntegerType value);

				/**
				 * @brief Interprets the string as option value and sets them to it.
				 *
				 * @param[in] value
				 *   Value string
				 **/
				void setValue( const string &value);

				/**
				 * @copybrief Option::negotiateServer()
				 *
				 * @param[in] optionValue
				 *   The option value received from the client.
				 *
				 * @return The negotiated Option.
				 * @retval OptionPointer()
				 *   If option negotiation failed.
				 **/
				virtual OptionPointer negotiateServer(
					const string &optionValue) const override;

				/**
				 * @copybrief Option::negotiateClient()
				 *
				 * @param[in] optionValue
				 *   The option value received from the server.
				 *
				 * @return The negotiated Option.
				 * @retval OptionPointer()
				 *   if option negotiation failed.
				 **/
				virtual OptionPointer negotiateClient(
					const string &optionValue) const override;

			private:
				/**
				 * @brief Converts the given value to a string.
				 *
				 * @param[in] value
				 *   The value.
				 *
				 * @return The value converted to a string.
				 **/
				static string toString( const IntegerType value);

				/**
				 * @brief Converts the given string to an integer.
				 *
				 * @param[in] value
				 *   Value as string
				 *
				 * @return The converted value
				 **/
				static IntegerType toInt( const string &value);

				//! minimum value
				const IntegerType minValue;
				//! maximum value
				const IntegerType maxValue;
				//! The value
				IntegerType value;
		};

		template< typename IntT>
		IntegerOption< IntT>::IntegerOption(
			const string &name,
			const IntegerType minValue,
			const IntegerType maxValue,
			const IntegerType value):
			Option( name),
			minValue( minValue),
			maxValue( maxValue),
			value( value)
		{
		}

		template< typename IntT>
		IntT IntegerOption< IntT>::getValue( void) const
		{
			return value;
		}

		template< typename IntT>
		string IntegerOption< IntT>::getValueString( void) const
		{
			return toString( value);
		}

		template< typename IntT>
		void IntegerOption< IntT>::setValue( const IntegerType value)
		{
			this->value = value;
		}

		template< typename IntT>
		void IntegerOption< IntT>::setValue( const std::string &value)
		{
			this->value = toInt( value);
		}

		template< typename IntT>
		OptionPointer IntegerOption< IntT>::negotiateServer(
			const string &optionValue) const
		{
			IntT value = toInt( optionValue);

			if (value < minValue)
				return OptionPointer();

			if (value > maxValue)
				value = maxValue;

			return std::make_shared< IntegerOption< IntT> >(
				getName(),
				minValue,
				maxValue,
				value);
		}

		template< typename IntT>
		OptionPointer IntegerOption< IntT>::negotiateClient(
			const string &optionValue) const
		{
			IntegerType value = toInt( optionValue);

			if (value < minValue)
				return OptionPointer();

			if (value > maxValue)
				return OptionPointer();

			return std::make_shared< IntegerOption< IntT> >(
				getName(),
				minValue,
				maxValue,
				value);
		}

		template< typename IntT>
		string IntegerOption< IntT>::toString( const IntegerType value)
		{
			return boost::lexical_cast< string>( value);
		}

		template< typename IntT>
		IntT IntegerOption< IntT>::toInt( const string &value)
		{
			return boost::lexical_cast< IntT>( value);
		}
	}
}

#endif
