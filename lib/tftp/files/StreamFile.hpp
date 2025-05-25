// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Files::StreamFile.
 **/

#ifndef TFTP_FILES_STREAMFILE_HPP
#define TFTP_FILES_STREAMFILE_HPP

#include <tftp/files/Files.hpp>
#include <tftp/files/File.hpp>

#include <fstream>
#include <filesystem>

namespace Tftp::Files {

/**
 * @brief Stream %File.
 *
 * %File implementation, which uses a std::fstream for file I/O handling.
 **/
class TFTP_EXPORT StreamFile final : public File
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
    explicit StreamFile( Operation operation, std::filesystem::path filename );

    /**
     * @brief Creates the StreamFile with the given stream as in/ output and the size information provided.
     *
     * @param[in] operation
     *   Receive or Transmit Operation.
     * @param[in] filename
     *   Filename to open/ create
     * @param[in] size
     *   Size of the file.
     *   In Receive Operation, the transfer is rejected if @p size is too big.
     *   On Transmit Operation this size is provided.
     **/
    StreamFile( Operation operation, std::filesystem::path filename, size_t size );

    /**
     * @copydoc File::reset
     *
     * Reopens the file depending on @p operationV.
     **/
    void reset() override;

    /**
     * @copydoc File::finished()
     *
     * Flushes the stream.
     **/
    void finished() override;

    /**
     * @copydoc File::receivedTransferSize()
     **/
    [[nodiscard]] bool receivedTransferSize( uint64_t transferSize ) override;

    /**
     * @copydoc File::receivedData()
     **/
    void receivedData( Helper::ConstRawDataSpan data ) override;

    /**
     * @copydoc File::requestedTransferSize()
     **/
    [[nodiscard]] std::optional< uint64_t> requestedTransferSize() override;

    /**
     * @copydoc File::sendData()
     **/
    [[nodiscard]] Helper::RawData sendData( size_t maxSize ) override;

  private:
    //! Actual Operation
    const Operation operationV;
    //! Filename
    const std::filesystem::path filenameV;
    //! Data Stream
    std::fstream streamV;
    //! File Size
    std::optional< size_t > sizeV;
};

}

#endif
