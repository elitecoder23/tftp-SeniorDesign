/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::ErrorCodeDescription.
 **/

#include "ErrorCodeDescription.hpp"

#include <ostream>

namespace Tftp {

using namespace std::literals;

ErrorCodeDescription::ErrorCodeDescription():
  Description{
    { "Not defined"sv,                     ErrorCode::NotDefined },
    { "File not found"sv,                  ErrorCode::FileNotFound },
    { "Access violation"sv,                ErrorCode::AccessViolation },
    { "Disk full or allocation exceeds"sv, ErrorCode::DiskFullOrAllocationExceeds },
    { "Illegal TFTP operation"sv,          ErrorCode::IllegalTftpOperation },
    { "Unknown transfer ID"sv,             ErrorCode::UnknownTransferId },
    { "File already exists"sv,             ErrorCode::FileAlreadyExists },
    { "No such user"sv,                    ErrorCode::NoSuchUser },
    { "TFTP option refused"sv,             ErrorCode::TftpOptionRefused },
  }
{
}

std::ostream& operator<<( std::ostream& stream, const ErrorCode errorCode)
{
  return (stream << ErrorCodeDescription::instance().name( errorCode ) );
}

}
