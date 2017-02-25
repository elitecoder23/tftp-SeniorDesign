/*
 * $Date$
 * $Revision$
 */
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

namespace Tftp {
namespace Options {

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
    StringOption( const string &name, const string &value = string());

    /**
     * Returns the value directly.
     **/
    virtual operator string() const final;

    /**
     * @brief Sets the option value.
     *
     * @param[in] value
     *   The option value.
     **/
    StringOption& operator=( const string &value);

    /**
     * @copydoc Option::negotiate()
     *
     * String options cannot be negotiated.
     * This operation always return an empty option pointer.
     *
     * @return Always an empty option pointer.
     **/
    virtual OptionPtr negotiate(
      const string &optionValue) const noexcept final;

  private:
    //! The option value.
    string value;
};
}
}

#endif
