/*
 * $Date$
 * $Revision$
 */
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

#include <boost/optional.hpp>

#include <iostream>
#include <type_traits>

namespace Tftp {
namespace File {

/**
 * @brief File implementation, which uses an std::iostream for file I/O
 *   handling.
 **/
template< typename StreamT>
class StreamFile: public TftpFile
{
  public:
    static_assert( std::is_base_of< std::iostream, StreamT>::value, "StreamT must be a std::iostream");

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
    const StreamType& getStream() const;

    /**
     * @brief Returns the stream object.
     *
     * @return The stream object
     **/
    StreamType& getStream();

    /**
     * @brief updates the file size info.
     *
     * @param[in] size
     *   The new size information
     **/
    void setSize( size_t size) noexcept;

    /**
     * @copydoc TftpFile::finished()
     *
     * Flushes the stream.
     **/
    virtual void finished() noexcept override final;

    /**
     * @copydoc TftpFile::receivedTransferSize()
     **/
    virtual bool receivedTransferSize( uint64_t transferSize) override final;

    /**
     * @copydoc TftpFile::receviedData()
     **/
    virtual void receviedData( const DataType &data) noexcept override final;

    /**
     * @copydoc TftpFile::requestedTransferSize()
     **/
    virtual bool requestedTransferSize( uint64_t &transferSize) override final;

    /**
     * @copydoc TftpFile::sendData()
     **/
    virtual DataType sendData( size_t maxSize) noexcept override final;

  private:
    //! the data stream
    StreamType stream;
    //! file size
    boost::optional< size_t> size;
};

}
}

#include "StreamFile.ipp"

#endif
