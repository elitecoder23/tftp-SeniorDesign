// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Files::MemoryFile.
 **/

#ifndef TFTP_FILES_MEMORYFILE_HPP
#define TFTP_FILES_MEMORYFILE_HPP

#include <tftp/files/Files.hpp>
#include <tftp/files/File.hpp>

namespace Tftp::Files {

/**
 * @brief Memory %File.
 *
 * %File implementation, which holds all data in memory.
 **/
class TFTP_EXPORT MemoryFile final : public File
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
     *   Initial data.
     **/
    explicit MemoryFile( Helper::ConstRawDataSpan data );

    /**
     * @brief Move constructor.
     *
     * Moves the given data to the file.
     *
     * @param[in] data
     *   Data of memory file.
     **/
    explicit MemoryFile( Helper::RawData data );

    /**
     * @copydoc File::reset
     *
     * If the file is received for receiving data, the content is cleared.
     * For write operation, the read pointer is reset to the beginning of the file.
     **/
    void reset() override;

    /**
     * @brief Returns a reference to the locally stored data.
     *
     * @return The locally stored data
     **/
    [[nodiscard]] Helper::ConstRawDataSpan data() const noexcept;

    /**
     * @copydoc File::finished()
     *
     * Reset current position.
     **/
    void finished() noexcept override;

    /**
     * @copydoc File::receivedTransferSize()
     *
     * Value supplied is ignored.
     * @return Always true.
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
    //! Operation Type
    const Operation operationV;
    //! Data
    Helper::RawData dataV;
    //! Current Read Position
    Helper::RawData::const_iterator dataPtr;
};

}

#endif
