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

#include <tftp/file/TftpFile.hpp>

#include <fstream>
#include <filesystem>

namespace Tftp::File {

/**
 * @brief File implementation, which uses an std::fstream for file I/O handling.
 **/
class StreamFile: public TftpFile
{
  public:
    /**
     * @brief Creates the StreamFile with the given stream as in/ output.
     *
     * @param[in] stream
     *   The data stream - The stream is moved.
     **/
    explicit StreamFile(
      Operation operation,
      const std::filesystem::path &filename);

    /**
     * @brief Creates the StreamFile with the given stream as in/ output and
     *  the size information provided.
     *
     * @param[in] stream
     *   The data stream - The stream is moved.
     * @param[in] size
     *   The size of the stream (e.g. file stream)
     **/
    StreamFile(
      Operation operation,
      const std::filesystem::path &fileName,
      size_t size);

    /**
     * @copydoc TftpFile::reset
     *
     **/
    void reset() final;

    /**
     * @copydoc TftpFile::finished()
     *
     * Flushes the stream.
     **/
    void finished() noexcept final;

    /**
     * @copydoc TftpFile::receivedTransferSize()
     **/
    bool receivedTransferSize( uint64_t transferSize) final;

    /**
     * @copydoc TftpFile::receivedData()
     **/
    void receivedData( const DataType &data) noexcept final;

    /**
     * @copydoc TftpFile::requestedTransferSize()
     **/
    std::optional< uint64_t> requestedTransferSize() final;

    /**
     * @copydoc TftpFile::sendData()
     **/
    DataType sendData( size_t maxSize) noexcept final;

  private:
    const Operation operationV;
    const std::filesystem::path filenameV;
    //! Data Stream
    std::fstream streamV;
    //! File Size
    std::optional< size_t> sizeV;
};

}

#endif
