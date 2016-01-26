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
 * @brief Declaration of namespace Tftp::Options.
 **/

#ifndef TFTP_OPTIONS_OPTIONS_HPP
#define TFTP_OPTIONS_OPTIONS_HPP

#include <tftp/Tftp.hpp>

#include <memory>

namespace Tftp {

/**
 * @brief Namespace defining classes for accessing TFTP options according to
 *   RFC 2348.
 **/
namespace Options {

class Option;
class OptionList;

//! Shortened form of option pointer
typedef std::shared_ptr< Option> OptionPointer;

}
}

#endif
