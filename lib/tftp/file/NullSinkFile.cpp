/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::File::NullSinkFile.
 **/

#include "NullSinkFile.hpp"

namespace Tftp::File {

NullSinkFile::NullSinkFile( std::optional< uint64_t> size ):
  size{ size }
{
}

void NullSinkFile::reset()
{
}

bool NullSinkFile::receivedTransferSize( const uint64_t transferSize )
{
  // If no size is provided
  if ( !size )
  {
    // Always accept file based on size
    return true;
  }

  // Accept file if size is matching the maximum allowed one.
  return ( transferSize <= size );
}

void NullSinkFile::receivedData(
  [[maybe_unused]] DataSpan data ) noexcept
{
  // Do nothing
}

}
