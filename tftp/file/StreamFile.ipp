/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of template class Tftp::File::StreamFile.
 **/

namespace Tftp {
namespace File {

template< typename StreamT>
StreamFile< StreamT>::StreamFile( StreamType &&stream) :
  stream( std::move( stream))
{
}

template< typename StreamT>
StreamFile< StreamT>::StreamFile( StreamType &&stream, const size_t size) :
  stream(  std::move( stream)),
  size( size)
{
}

template< typename StreamT>
StreamFile< StreamT>& StreamFile< StreamT>::operator=( StreamType &&stream)
{
  this->stream = stream;
  size.reset();

  return *this;
}

template< typename StreamT>
const typename StreamFile< StreamT>::StreamType&
StreamFile< StreamT>::getStream() const
{
  return stream;
}

template< typename StreamT>
typename StreamFile< StreamT>::StreamType& StreamFile< StreamT>::getStream()
{
  return stream;
}

template< typename StreamT>
void StreamFile< StreamT>::setSize( const size_t size) noexcept
{
  this->size = size;
}

template< typename StreamT>
void StreamFile< StreamT>::finished() noexcept
{
  stream.flush();
}

template< typename StreamT>
bool StreamFile< StreamT>::receivedTransferSize( const uint64_t transferSize)
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

template< typename StreamT>
void StreamFile< StreamT>::receviedData( const DataType &data) noexcept
{
  stream.write( reinterpret_cast< const char*>( &data[0]), data.size());
}

template< typename StreamT>
bool StreamFile< StreamT>::requestedTransferSize( uint64_t &transferSize)
{
  transferSize = size.value_or( 0);
  return size.is_initialized();
}

template< typename StreamT>
typename StreamFile< StreamT>::DataType StreamFile< StreamT>::sendData(
  const size_t maxSize) noexcept
{
  DataType data( maxSize);

  stream.read( reinterpret_cast< char*>(&data[0]), maxSize);

  data.resize( stream.gcount());

  return data;
}

}
}
