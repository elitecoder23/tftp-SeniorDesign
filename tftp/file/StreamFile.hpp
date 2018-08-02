/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of template class Tftp::File::StreamFile.
 **/

#ifndef TFTP_FILE_STREAMFILE_HPP
#define TFTP_FILE_STREAMFILE_HPP

#include <tftp/file/TftpFile.hpp>

#include <iostream>
#include <type_traits>

namespace Tftp::File {

/**
 * @brief File implementation, which uses an std::iostream for file I/O
 *   handling.
 **/
template< typename StreamT>
class StreamFile: public TftpFile
{
  public:
    static_assert(
      std::is_base_of< std::iostream, StreamT>::value,
      "StreamT must be a std::iostream");

    //! The stream type
    using StreamType = StreamT;

    StreamFile() = default;

    /**
     * @brief Creates the StreamFile with the given stream as in/ output.
     *
     * @param[in] stream
     *   The data stream - The stream is moved.
     **/
    explicit StreamFile( StreamType &&stream);

    /**
     * @brief Creates the StreamFile with the given stream as in/ output and
     *  the size information provided.
     *
     * @param[in] stream
     *   The data stream - The stream is moved.
     * @param[in] size
     *   The size of the stream (e.g. file stream)
     **/
    StreamFile( StreamType &&stream, size_t size);

    /**
     * @brief Assigns a new stream to the file.
     *
     * @param[in] stream
     *   The data stream - The stream is moved.
     *
     * @return *this
     **/
    StreamFile& operator=( StreamType &&stream);

    /**
     * @brief Returns the stream object.
     *
     * @return The stream object
     **/
    const StreamType& stream() const;

    /**
     * @brief Returns the stream object.
     *
     * @return The stream object
     **/
    StreamType& stream();

    /**
     * @brief updates the file size info.
     *
     * @param[in] size
     *   The new size information
     **/
    void size( size_t size) noexcept;

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
    //! the data stream
    StreamType streamValue;
    //! file size
    std::optional< size_t> sizeValue;
};

}

#include "StreamFile.ipp"

#endif
