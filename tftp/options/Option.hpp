/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Options::Option.
 **/

#ifndef TFTP_OPTIONS_OPTION_HPP
#define TFTP_OPTIONS_OPTION_HPP

#include <tftp/options/Options.hpp>

#include <string>
#include <string_view>

namespace Tftp::Options {

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
    static std::string_view optionName( KnownOptions option) noexcept;

    /**
     * @brief Generate TFTP Option.
     **/
    Option() = default;

    /**
     * @brief Copy Constructor
     *
     * @param[in] other
     *   Object to copy.
     **/
    Option( const Option &other) = default;

    /**
     * @brief Move Constructor
     *
     * @param[in] other
     *   Object to move.
     **/
    Option( Option &&other) = default;

    /**
     * @brief Copy Assignment Operator
     *
     * @param[in] other
     *   Object to copy.
     * @return *this.
     **/
    Option& operator=( const Option &other) = default;

    /**
     * @brief Move Assignment Operator
     *
     * @param[in] other
     *   Object to move.
     * @return *this.
     **/
    Option& operator=( Option &&other) = default;

    //! Destructor
    virtual ~Option() noexcept = default;

    /**
     * @brief Returns the option value as string.
     *
     * @return The option value as string.
     **/
    virtual explicit operator std::string() const = 0;

    /**
     * @brief %Option negotiation.
     *
     * This function is called, when the TFTP server receives options within
     * RRQ/ WRQ packets.
     * The Server is allowed to modify the option value to a value, which is
     * accepted by the client.
     *
     * This function is called, when the TFTP client receives options within
     * a OACK packet.
     * The client will or will not accept the received option value.
     *
     * @param[in] optionValue
     *   The input option
     *
     * @return Option negotiation result
     * @retval OptionPtr()
     *   If the option value is not acceptable.
     **/
    [[nodiscard]] virtual OptionPtr negotiate(
      std::string_view optionValue) const noexcept = 0;
};

}

#endif
