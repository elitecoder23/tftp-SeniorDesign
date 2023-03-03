/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Version.
 **/

#ifndef TFTP_VERSION_HPP
#define TFTP_VERSION_HPP

#include <tftp/Tftp.hpp>

#include <cstdint>
#include <string>
#include <string_view>

namespace Tftp {

//! TFTP C++ Library %Version Information.
class TFTP_EXPORT Version
{
  public:
    // delete constructors
    Version() = delete;

    //! Major Version
    static const uint32_t Major;
    //! Minor Version
    static const uint32_t Minor;
    //! Patch Version
    static const uint32_t Patch;

    //! Additional Version Information
    static const std::string_view Addition;

    //! Version Information as String.
    static const std::string_view VersionInformation;
};

}

#endif
