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

namespace Tftp::File {

template< typename StreamT>
StreamFile< StreamT>::StreamFile( StreamType &&stream) :
  streamValue( std::move( stream))
{
}

template< typename StreamT>
StreamFile< StreamT>::StreamFile( StreamType &&stream, const size_t size) :
  streamValue( std::move( stream)),
  sizeValue( size)
{
}

template< typename StreamT>
StreamFile< StreamT>& StreamFile< StreamT>::operator=( StreamType &&stream)
{
  streamValue = stream;
  sizeValue.reset();

  return *this;
}

template< typename StreamT>
const typename StreamFile< StreamT>::StreamType&
StreamFile< StreamT>::stream() const
{
  return streamValue;
}

template< typename StreamT>
typename StreamFile< StreamT>::StreamType& StreamFile< StreamT>::stream()
{
  return streamValue;
}

template< typename StreamT>
void StreamFile< StreamT>::size( const size_t size) noexcept
{
  sizeValue = size;
}

template< typename StreamT>
void StreamFile< StreamT>::finished() noexcept
{
  streamValue.flush();
}

template< typename StreamT>
bool StreamFile< StreamT>::receivedTransferSize( const uint64_t transferSize)
{
  // If no size is provided
  if ( !sizeValue)
  {
    // Always accept file based on size
    return true;
  }

  // Accept file if size is matching the maximum allowed one.
  return (transferSize <= sizeValue);
}

template< typename StreamT>
void StreamFile< StreamT>::receivedData( const DataType &data) noexcept
{
  streamValue.write( reinterpret_cast< const char*>( &data[0]), data.size());
}

template< typename StreamT>
std::optional< uint64_t> StreamFile< StreamT>::requestedTransferSize()
{
  return sizeValue;
}

template< typename StreamT>
typename StreamFile< StreamT>::DataType StreamFile< StreamT>::sendData(
  const size_t maxSize) noexcept
{
  DataType data( maxSize);

  streamValue.read( reinterpret_cast< char*>(&data[0]), maxSize);

  data.resize( streamValue.gcount());

  return data;
}

}
