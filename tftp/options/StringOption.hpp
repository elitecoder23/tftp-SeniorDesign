/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::Options::StringOption.
 **/

#ifndef TFTP_OPTIONS_STRINGOPTION_HPP
#define TFTP_OPTIONS_STRINGOPTION_HPP

#include <tftp/options/Option.hpp>

#include <string>

namespace Tftp::Options {

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
    StringOption( const std::string &name, const std::string &value);

    //! @copydoc StringOption(const std::string&,const std::string&)
    StringOption( std::string &&name, std::string &&value = {});

    /**
     * Returns the value directly.
     **/
    operator std::string() const final;

    /**
     * @brief Sets the option value.
     *
     * @param[in] value
     *   The option value.
     *
     * @return *this
     **/
    StringOption& operator=( const std::string &value);

    /**
     * @copydoc Option::negotiate()
     *
     * String options cannot be negotiated.
     * This operation always return an empty option pointer.
     *
     * @return Always an empty option pointer.
     **/
    OptionPtr negotiate( std::string_view optionValue) const noexcept final;

  private:
    //! The option value.
    std::string value;
};

}

#endif
