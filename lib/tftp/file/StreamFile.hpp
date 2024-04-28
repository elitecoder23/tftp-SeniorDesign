// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::File::StreamFile.
 **/

#ifndef TFTP_FILE_STREAMFILE_HPP
#define TFTP_FILE_STREAMFILE_HPP

#include "tftp/file/File.hpp"
#include "tftp/file/TftpFile.hpp"

#include <fstream>
#include <filesystem>

namespace Tftp::File {

/**
 * @brief Stream %File.
 *
 * File implementation, which uses a std::fstream for file I/O handling.
 **/
class TFTP_EXPORT StreamFile final : public TftpFile
{
  public:
    /**
     * @brief Creates the StreamFile with the given stream as in/ output.
     *
     * @param[in] operation
     *   Receive or Transmit Operation.
     * @param[in] filename
     *   Filename to open/ create
     **/
    explicit StreamFile(
      Operation operation,
      std::filesystem::path filename );

    /**
     * @brief Creates the StreamFile with the given stream as in/ output and
     *  the size information provided.
     *
     * @param[in] operation
     *   Receive or Transmit Operation.
     * @param[in] filename
     *   Filename to open/ create
     * @param[in] size
     *   Size of the file.
     *   In Receive Operation, the transfer is reject if size is to big.
     *   On Transmit Operation this size is provided.
     **/
    StreamFile(
      Operation operation,
      std::filesystem::path filename,
      size_t size );

    /**
     * @copydoc TftpFile::reset
     *
     * Reopens the file depending on @p operationV.
     **/
    void reset() override;

    /**
     * @copydoc TftpFile::finished()
     *
     * Flushes the stream.
     **/
    void finished() noexcept override;

    /**
     * @copydoc TftpFile::receivedTransferSize()
     **/
    [[nodiscard]] bool receivedTransferSize( uint64_t transferSize ) override;

    /**
     * @copydoc TftpFile::receivedData()
     **/
    void receivedData( DataSpan data ) noexcept override;

    /**
     * @copydoc TftpFile::requestedTransferSize()
     **/
    [[nodiscard]] std::optional< uint64_t> requestedTransferSize() override;

    /**
     * @copydoc TftpFile::sendData()
     **/
    [[nodiscard]] Data sendData( size_t maxSize ) noexcept override;

  private:
    //! Actual Operation
    const Operation operationV;
    //! Filename
    const std::filesystem::path filenameV;
    //! Data Stream
    std::fstream streamV;
    //! File Size
    std::optional< size_t> sizeV;
};

}

#endif
