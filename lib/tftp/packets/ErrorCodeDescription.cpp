/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Packets::ErrorCodeDescription.
 **/

#include "ErrorCodeDescription.hpp"

#include <ostream>

namespace Tftp::Packets {

ErrorCodeDescription::ErrorCodeDescription():
  Description{
    { "Not defined",                     ErrorCode::NotDefined },
    { "File not found",                  ErrorCode::FileNotFound },
    { "Access violation",                ErrorCode::AccessViolation },
    { "Disk full or allocation exceeds", ErrorCode::DiskFullOrAllocationExceeds },
    { "Illegal TFTP operation",          ErrorCode::IllegalTftpOperation },
    { "Unknown transfer ID",             ErrorCode::UnknownTransferId },
    { "File already exists",             ErrorCode::FileAlreadyExists },
    { "No such user",                    ErrorCode::NoSuchUser },
    { "TFTP option refused",             ErrorCode::TftpOptionRefused },
  }
{
}

std::ostream& operator<<( std::ostream& stream, const ErrorCode errorCode )
{
  return (stream << ErrorCodeDescription::instance().name( errorCode ) );
}

}
