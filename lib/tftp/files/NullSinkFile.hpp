// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Tftp::Files::NullSinkFile.
 **/

#ifndef TFTP_FILES_NULLSINKFILE_HPP
#define TFTP_FILES_NULLSINKFILE_HPP

#include <tftp/files/Files.hpp>
#include <tftp/ReceiveDataHandler.hpp>

#include <optional>

namespace Tftp::Files {

/**
 * @brief NULL Sink %File.
 *
 * This class provides a receive data handler, which drops every received data package.
 * This handler can be used for testing purposes.
 *
 * If a size is given, this size is checked against the value given when calling receivedTransferSize().
 **/
class TFTP_EXPORT NullSinkFile final : public ReceiveDataHandler
{
  public:
    //! Constructor
    NullSinkFile() = default;

    /**
     * @brief Constructs file with maximum file size.
     *
     * @param[in] size
     *   The maximum allowed size.
     **/
    explicit NullSinkFile( uint64_t size );

    /**
     * @copydoc ReceiveDataHandler::reset
     *
     **/
    void reset() override;

    /**
     * @copydoc ReceiveDataHandler::finished
     **/
    void finished() noexcept override;

    /**
     * @copydoc ReceiveDataHandler::receivedTransferSize
     *
     * If size is given on constructing this file, handle it.
     * Otherwise, return true.
     **/
    [[nodiscard]] bool receivedTransferSize( uint64_t transferSize ) override;

    /**
     * @copydoc ReceiveDataHandler::receivedData
     *
     * Drops the data immediately.
     **/
    void receivedData( ConstDataSpan data ) override;

  private:
    //! Optional size (used for options negotiation)
    std::optional< uint64_t > size;
};

}

#endif
