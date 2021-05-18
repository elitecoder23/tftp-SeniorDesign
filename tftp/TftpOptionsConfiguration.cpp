/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::TftpOptionsConfiguration.
 **/

#include "TftpOptionsConfiguration.hpp"

namespace Tftp {

std::string_view TftpOptionsConfiguration::optionName(
  const KnownOptions option ) noexcept
{
  switch ( option )
  {
    case KnownOptions::BlockSize:
      return "blksize";

    case KnownOptions::Timeout:
      return "timeout";

    case KnownOptions::TransferSize:
      return "tsize";

    default:
      return {};
  }
}

}
