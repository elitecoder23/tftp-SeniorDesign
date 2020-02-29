/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::File::StreamFile.
 **/

#include "StreamFile.hpp"

#include <tftp/TftpException.hpp>

namespace Tftp::File {

StreamFile::StreamFile(
  const Operation operation,
  const std::filesystem::path &filename) :
  operationV{ operation},
  filenameV{ filename}
{
}

StreamFile::StreamFile(
  const Operation operation,
  const std::filesystem::path &filename,
  const size_t size) :
  operationV{ operation},
  filenameV{ filename},
  sizeV{ size}
{
}

void StreamFile::reset()
{
  switch ( operationV)
  {
    case TftpFile::Operation::Receive:
      streamV.open( filenameV, std::ios::out | std::ios::trunc | std::ios::binary);
      break;

    case TftpFile::Operation::Transmit:
      streamV.open(
        filenameV,
        std::ios::in | std::ios::binary);
      break;

    default:
      BOOST_THROW_EXCEPTION( TftpException()
        << Helper::AdditionalInfo( "Invalid File Mode"));
  }

  if (!streamV)
  {
    BOOST_THROW_EXCEPTION( TftpException()
      << Helper::AdditionalInfo( "Error opening file"));
  }
}

void StreamFile::finished() noexcept
{
  streamV.flush();
  streamV.close();
}

bool StreamFile::receivedTransferSize( const uint64_t transferSize)
{
  // If no size is provided
  if ( !sizeV)
  {
    // Always accept file based on size
    return true;
  }

  // Accept file if size is matching the maximum allowed one.
  return ( transferSize <= sizeV);
}

void StreamFile::receivedData( const DataType &data) noexcept
{
  streamV.write( reinterpret_cast< const char*>( &data[0]), data.size());
}

std::optional< uint64_t> StreamFile::requestedTransferSize()
{
  return sizeV;
}

StreamFile::DataType StreamFile::sendData( const size_t maxSize) noexcept
{
  DataType data( maxSize);

  streamV.read( reinterpret_cast< char*>(&data[0]), maxSize);

  data.resize( streamV.gcount());

  return data;
}

}
