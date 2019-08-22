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

/**
 * @brief TFTP Protocol Options Implementation (RFC 2348).
 **/
namespace Tftp::Options {

class Option;
class OptionList;

//! Option Pointer
using OptionPtr = std::shared_ptr< Option>;

}

#endif
