/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::File::MemoryFile.
 **/

#ifndef TFTP_FILE_MEMORYFILE_HPP
#define TFTP_FILE_MEMORYFILE_HPP

#include <tftp/file/TftpFile.hpp>

namespace Tftp::File {

/**
 * @brief File implementation, which holds all data in memory.
 **/
class MemoryFile: public TftpFile
{
  public:
    /**
     * @brief Creates a memory file with no current data.
     *
     * This constructor is useful for receiving data.
     **/
    MemoryFile();

    /**
     * @brief Creates a memory file with the given data.
     *
     * The data is copied to an internal data structure.
     *
     * @param[in] data
     *   The initial data.
     **/
    explicit MemoryFile( const DataType &data);

    /**
     * @brief Move constructor.
     *
     * Moves the given data to the file.
     *
     * @param[in] data
     *   Data of memory file.
     **/
    explicit MemoryFile( DataType &&data);

    void reset() final;

    /**
     * @brief Returns a reference to the locally stored data.
     *
     * @return The locally stored data
     **/
    [[nodiscard]] const DataType& data() const noexcept;

    /**
     * @copydoc TftpFile::finished()
     *
     * Reset current position.
     **/
    void finished() noexcept final;

    /**
     * @copydoc TftpFile::receivedTransferSize()
     *
     * Value supplied is ignored.
     * @return Always true.
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
    //! Operation Type
    const Operation operationV;
    //! Data
    DataType dataV;
    //! Current Read Position
    DataType::const_iterator dataPtr;
};

}

#endif
