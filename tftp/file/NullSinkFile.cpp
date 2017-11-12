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

NullSinkFile::NullSinkFile( uint64_t size):
  size( size)
{
}

bool NullSinkFile::receivedTransferSize( const uint64_t transferSize)
{
  // If no size is provided
  if ( !size)
  {
    // Always accept file based on size
    return true;
  }

  // Accept file if size is matching the maximum allowed one.
  return (transferSize <= size);
}

void NullSinkFile::receviedData( const DataType &/*data*/) noexcept
{
  // Do nothing
}

}
}
