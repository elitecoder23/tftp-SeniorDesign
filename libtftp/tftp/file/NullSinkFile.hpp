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
 * @brief Declaration of class Tftp::File::NullSinkFile.
 **/

#ifndef TFTP_FILE_NULLSINKFILE_HPP
#define TFTP_FILE_NULLSINKFILE_HPP

#include <tftp/file/File.hpp>

#include <boost/optional.hpp>

namespace Tftp {
namespace File {

class NullSinkFile : public TftpReceiveDataOperationHandler
{
  public:
    NullSinkFile();

    NullSinkFile( uint64_t transferSize);

    virtual bool receivedTransferSize( const uint64_t transferSize) override final;

    virtual void receviedData( const DataType &data) noexcept override final;

  private:
    boost::optional< uint64_t> transferSize;
};

}
}

#endif
