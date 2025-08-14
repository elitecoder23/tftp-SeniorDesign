// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Tftp::Files.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Tftp::Files.
 **/

#ifndef TFTP_FILES_FILES_HPP
#define TFTP_FILES_FILES_HPP

#include <tftp/Tftp.hpp>

#include <memory>

/**
 * @brief TFTP %File Handling.
 *
 * This namespace provides common handlers of data which shall be received from or transmitted to another TFTP instance.
 *
 * Currently, there are two implementations:
 * - @ref MemoryFile, which handles the data within a local std::vector, and
 * - @ref StreamFile, which handles the data through a std::iostream
 * - @ref NullSinkFile, which drops every received data.
 *
 * The two implementations do not handle the data in the manner of encoding handling, i.e. only the TFTP OCTET transfer
 * mode is supported.
 **/
namespace Tftp::Files {

class File;
class MemoryFile;
class StreamFile;
class NullSinkFile;

//! Memory %File Pointer
using MemoryFilePtr = std::shared_ptr< MemoryFile>;

//! Stream %File Pointer
using StreamFilePtr = std::shared_ptr< StreamFile >;

}

#endif
