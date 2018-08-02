/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::File::NullSinkFile.
 **/

#ifndef TFTP_FILE_NULLSINKFILE_HPP
#define TFTP_FILE_NULLSINKFILE_HPP

#include <tftp/file/File.hpp>
#include <tftp/ReceiveDataHandler.hpp>

#include <optional>

namespace Tftp::File {

/**
 * @brief NULL sink file.
 *
 * This class provides an receive data handler, which drops every received
 * data package.
 * This handler can be used for testing purposes.
 *
 * If a size is given, this size is checked against the value given when calling
 * receivedTransferSize().
 **/
class NullSinkFile : public ReceiveDataHandler
{
  public:
    //! default constructor
    NullSinkFile() = default;

    /**
     * @brief Constructs file with maximum file size.
     *
     * @param[in] size
     *   The maximum allowed size.
     **/
    explicit NullSinkFile( uint64_t size);

    /**
     * @copydoc ReceiveDataHandler::receivedTransferSize
     *
     * If size is given on constructing this file, handle it.
     * Otherwise return true.
     **/
    bool receivedTransferSize( uint64_t transferSize) final;

    /**
     * @copydoc ReceiveDataHandler::receivedData
     *
     * Drops the data immediately.
     **/
    void receivedData( const DataType &data) noexcept final;

  private:
    //! Optional size (used for options negotiation)
    std::optional< uint64_t> size;
};

}

#endif
