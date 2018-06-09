/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class Tftp::ErrorCodeDescription.
 **/

#include "ErrorCodeDescription.hpp"

#include <boost/program_options.hpp>

#include <ostream>

namespace Tftp {

ErrorCodeDescription::ErrorCodeDescription():
  Description{
    { "Not defined",                    ErrorCode::NotDefined},
    { "File not found",                 ErrorCode::FileNotFound},
    { "Access violation",               ErrorCode::AccessViolation},
    { "Disk full or allocation exceeds",ErrorCode::DiskFullOrAllocationExceeds},
    { "Illegal TFTP operation",         ErrorCode::IllegalTftpOperation},
    { "Unknown transfer ID",            ErrorCode::UnknownTransferId},
    { "File already exists",            ErrorCode::FileAllreadyExists},
    { "No such user",                   ErrorCode::NoSuchUser},
    { "TFTP option refused",            ErrorCode::TftpOptionRefused},
  }
{
}

std::ostream& operator<<( std::ostream& stream, const ErrorCode errorCode)
{
  return (stream << ErrorCodeDescription::instance().name( errorCode));
}

}


