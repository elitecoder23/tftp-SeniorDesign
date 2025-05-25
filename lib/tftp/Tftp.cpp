// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace Tftp.
 **/

#include "Tftp.hpp"

#include <tftp/Version.hpp>

#include <helper/VersionInformation.hpp>

namespace Tftp {

/**
 * @brief Provide Version Information as Helper::VersionsInformation::value_type.
 *
 * @return Version Information.
 **/
Helper::VersionsInformation::value_type tftpVersion()
{
  return {
    Version::Key,
    {
      Version::Name,
      Version::VersionInformation,
      Version::License,
      Version::Url
    }
  };
}

}
