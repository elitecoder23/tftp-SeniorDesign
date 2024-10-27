// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Files::StreamFile.
 **/

#include "StreamFile.hpp"

#include <tftp/Logger.hpp>
#include <tftp/TftpException.hpp>

#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

#include <utility>

namespace Tftp::Files {

StreamFile::StreamFile(
  const Operation operation,
  std::filesystem::path filename ) :
  operationV{ operation },
  filenameV{ std::move( filename ) }
{
}

StreamFile::StreamFile(
  const Operation operation,
  std::filesystem::path filename,
  const size_t size ) :
  operationV{ operation },
  filenameV{ std::move( filename ) },
  sizeV{ size }
{
}

void StreamFile::reset()
{
  BOOST_LOG_FUNCTION()

  switch ( operationV )
  {
    case File::Operation::Receive:
      streamV.open(
        filenameV,
        std::ios::out | std::ios::trunc | std::ios::binary );
      break;

    case File::Operation::Transmit:
      streamV.open(
        filenameV,
        std::ios::in | std::ios::binary );
      break;

    default:
      BOOST_THROW_EXCEPTION( TftpException()
        << Helper::AdditionalInfo{ "Invalid File Mode" }
        << boost::errinfo_file_name{ filenameV.string() } );
  }

  if ( !streamV )
  {
    BOOST_THROW_EXCEPTION( TftpException()
      << Helper::AdditionalInfo{ "Error opening file" }
      << boost::errinfo_file_name{ filenameV.string() } );
  }
}

void StreamFile::finished()
{
  streamV.flush();
  streamV.close();
}

bool StreamFile::receivedTransferSize( const uint64_t transferSize )
{
  // If no size is provided
  if ( !sizeV )
  {
    // Always accept file based on size
    return true;
  }

  // Accept file if size is matching the maximum allowed one.
  return ( transferSize <= sizeV );
}

void StreamFile::receivedData( DataSpan data )
{
  if ( !data.empty() )
  {
    streamV.write(
      reinterpret_cast< const char * >( std::data( data ) ),
      static_cast< std::streamsize >( data.size() ) );
  }
}

std::optional< uint64_t> StreamFile::requestedTransferSize()
{
  return sizeV;
}

StreamFile::Data StreamFile::sendData( const size_t maxSize )
{
  Data data( maxSize );

  streamV.read(
    reinterpret_cast< char*>( std::data( data ) ),
    static_cast< std::streamsize >( maxSize ) );

  data.resize( streamV.gcount() );

  return data;
}

}
