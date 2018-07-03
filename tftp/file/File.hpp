/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of namespace Tftp::File
 **/

#ifndef TFTP_FILE_FILE_HPP
#define TFTP_FILE_FILE_HPP

#include <tftp/Tftp.hpp>
#include <memory>

/**
 * @brief TFTP file implementation.
 *
 * This namespace provides common handler of data which shall be received from
 * or transmitted to an other TFTP instance.
 *
 * Currently there are two implementations:
 * @li MemoryFile, which handles the data within a local std::vector, and
 * @li StreamFile, which handles the data through a std::iostream
 * @li NullSinkFile, which drops every received data.
 *
 * The two implementations do not handle the data in the manner of encoding
 * handling, i.e. only the TFTP OCTET transfer mode is supported.
 **/
namespace Tftp::File {

class TftpFile;
class MemoryFile;
class StreamFile;
class NullSinkFile;

//! Memory file pointer
using MemoryFilePtr = std::shared_ptr< MemoryFile>;

}

#endif
