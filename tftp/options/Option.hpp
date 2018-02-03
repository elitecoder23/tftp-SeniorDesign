/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
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
    static std::string getOptionName( const KnownOptions option) noexcept;

    /**
     * @brief Generate TFTP option with the given name.
     *
     * @param[in] name
     *   The option name. Must be not empty.
     **/
    Option( const std::string &name);

    //! @copydoc Option(const std::string&)
    Option( std::string &&name);

    //! default copy constructor
    Option( const Option &other) = default;

    //! default move constructor
    Option( Option &&other) = default;

    //! default copy assignment operator
    Option& operator=( const Option &other) = default;

    //! default copy move operator
    Option& operator=( Option &&other) = default;

    //! Default destructor
    virtual ~Option() noexcept = default;

    /**
     * @brief Returns the option name.
     *
     * @return The option name.
     **/
    std::string name() const;

    /**
     * @brief Set the option name.
     *
     * @param[in] name
     *   The new option name. Must be not empty.
     **/
    void name( const std::string &name);

    /**
     * @brief Returns the option value as string.
     *
     * @return The option value as string.
     **/
    virtual operator std::string() const = 0;

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
    virtual OptionPtr negotiate( const std::string &optionValue) const noexcept = 0;

    /**
     * @brief Returns a string, which describes the option.
     *
     * This operation is used for debugging and information purposes.
     *
     * @return Option list description.
     **/
    virtual std::string toString() const;

  private:
    //! The option name
    std::string nameValue;
};

}
}

#endif
