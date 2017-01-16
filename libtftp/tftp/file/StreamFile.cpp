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
 * @brief Definition of class Tftp::File::StreamFile.
 **/

#include "StreamFile.hpp"

#include <helper/Logger.hpp>

namespace Tftp {
namespace File {

StreamFile::StreamFile( std::iostream &stream) :
  stream( stream)
{
}

StreamFile::StreamFile( std::iostream &stream, const size_t size) :
  stream( stream),
  size( size)
{
}

void StreamFile::setSize( const size_t size) noexcept
{
  this->size = size;
}

void StreamFile::finishedOperation() noexcept
{
  stream.flush();
}

bool StreamFile::receivedTransferSize( const uint64_t transferSize)
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

void StreamFile::receviedData( const DataType &data) noexcept
{
  stream.write( reinterpret_cast< const char*>( &data[0]), data.size());
}

bool StreamFile::requestedTransferSize( uint64_t &transferSize)
{
  transferSize = size.value_or( 0);
  return size.is_initialized();
}

StreamFile::DataType StreamFile::sendData( const size_t maxSize) noexcept
{
  DataType data( maxSize);

  stream.read( reinterpret_cast< char*>(&data[0]), maxSize);

  data.resize( stream.gcount());

  return data;
}

}
}
