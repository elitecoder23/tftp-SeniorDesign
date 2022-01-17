/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::File::MemoryFile.
 **/

#include "MemoryFile.hpp"

#include <tftp/TftpLogger.hpp>

namespace Tftp::File {

MemoryFile::MemoryFile():
  operationV{ Operation::Receive },
  dataPtr{ dataV.begin() }
{
}

MemoryFile::MemoryFile( const DataType &data ) :
  operationV{ Operation::Transmit },
  dataV{ data },
  dataPtr{ dataV.begin() }
{
}

MemoryFile::MemoryFile( DataType &&data ):
  operationV{ Operation::Transmit },
  dataV{ std::move( data ) },
  dataPtr{ dataV.begin() }
{
}

void MemoryFile::reset()
{
  if ( Operation::Receive == operationV )
  {
    dataV.clear();
  }

  dataPtr = dataV.begin();
}

const MemoryFile::DataType& MemoryFile::data() const noexcept
{
  return dataV;
}

void MemoryFile::finished() noexcept
{
  dataPtr = dataV.begin();
}

bool MemoryFile::receivedTransferSize( const uint64_t transferSize )
{
  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "Received transfer size: " << transferSize;

  return true;
}

void MemoryFile::receivedData( const DataType &data ) noexcept
{
  if ( !data.empty() )
  {
    dataV.insert( dataV.end(), data.begin(), data.end() );

    dataPtr = dataV.begin();
  }
}

std::optional< uint64_t> MemoryFile::requestedTransferSize()
{
  return dataV.size();
}

MemoryFile::DataType MemoryFile::sendData( const size_t maxSize ) noexcept
{
  DataType::const_iterator startPtr{ dataPtr };
  DataType::const_iterator endPtr;

  if ( static_cast< size_t >(
    std::distance< DataType::const_iterator>(
      startPtr,
      dataV.end() ) ) <= maxSize )
  {
    endPtr = dataV.end();
  }
  else
  {
    endPtr = dataPtr + static_cast< ptrdiff_t >( maxSize );
  }

  dataPtr=endPtr;

  return DataType( startPtr, endPtr );
}

}
