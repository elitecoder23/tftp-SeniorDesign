/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Template Class Tftp::File::StreamFile.
 **/

namespace Tftp::File {

template< typename StreamT>
StreamFile< StreamT>::StreamFile( StreamType &&stream) :
  streamV( std::move( stream))
{
}

template< typename StreamT>
StreamFile< StreamT>::StreamFile( StreamType &&stream, const size_t size) :
  streamV( std::move( stream)),
  sizeV( size)
{
}

template< typename StreamT>
StreamFile< StreamT>& StreamFile< StreamT>::operator=( StreamType &&stream)
{
  streamV = stream;
  sizeV.reset();

  return *this;
}

template< typename StreamT>
const typename StreamFile< StreamT>::StreamType&
StreamFile< StreamT>::stream() const
{
  return streamV;
}

template< typename StreamT>
typename StreamFile< StreamT>::StreamType& StreamFile< StreamT>::stream()
{
  return streamV;
}

template< typename StreamT>
void StreamFile< StreamT>::size( const size_t size) noexcept
{
  sizeV = size;
}

template< typename StreamT>
void StreamFile< StreamT>::finished() noexcept
{
  streamV.flush();
}

template< typename StreamT>
bool StreamFile< StreamT>::receivedTransferSize( const uint64_t transferSize)
{
  // If no size is provided
  if ( !sizeV)
  {
    // Always accept file based on size
    return true;
  }

  // Accept file if size is matching the maximum allowed one.
  return (transferSize <= sizeV);
}

template< typename StreamT>
void StreamFile< StreamT>::receivedData( const DataType &data) noexcept
{
  streamV.write( reinterpret_cast< const char*>( &data[0]), data.size());
}

template< typename StreamT>
std::optional< uint64_t> StreamFile< StreamT>::requestedTransferSize()
{
  return sizeV;
}

template< typename StreamT>
typename StreamFile< StreamT>::DataType StreamFile< StreamT>::sendData(
  const size_t maxSize) noexcept
{
  DataType data( maxSize);

  streamV.read( reinterpret_cast< char*>(&data[0]), maxSize);

  data.resize( streamV.gcount());

  return data;
}

}
