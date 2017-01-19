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
 * @brief Definition of class Tftp::File::NullSinkFile.
 **/

#include "NullSinkFile.hpp"

namespace Tftp {
namespace File {

NullSinkFile::NullSinkFile()
{
}

NullSinkFile::NullSinkFile( uint64_t transferSize):
  transferSize( transferSize)
{
}

bool NullSinkFile::receivedTransferSize( const uint64_t transferSize)
{
  if (this->transferSize.is_initialized())
  {
    return (this->transferSize >= transferSize);
  }

  return true;
}

void NullSinkFile::receviedData( const DataType &data) noexcept
{

}

}
}
