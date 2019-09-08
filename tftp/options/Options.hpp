/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Tftp::Options.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Tftp::Options.
 **/

#ifndef TFTP_OPTIONS_OPTIONS_HPP
#define TFTP_OPTIONS_OPTIONS_HPP

#include <tftp/Tftp.hpp>

#include <memory>
#include <map>
#include <string>
#include <string_view>
#include <functional>

/**
 * @brief TFTP %Options (RFC 2348).
 **/
namespace Tftp::Options {

class OptionList;

//! TFTP Options (Maps Option Name to Option Value)
using Options = std::map< std::string, std::string, std::less<>>;

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
 * @retval std::string()
 *   If the option value is not acceptable.
 **/
using NegotiateOption = std::function< std::string(
  std::string_view optionValue)>;

//! TFTP Options Negotiation (Maps Option Name to Negotiation Function)
using OptionsNegotiation = std::map< std::string, NegotiateOption, std::less<>>;

}

#endif
