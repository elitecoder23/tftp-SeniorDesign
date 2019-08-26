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
  dataPtr{ dataValue.begin()}
{
}

MemoryFile::MemoryFile( const DataType &data):
  dataValue{ data},
  dataPtr{ dataValue.begin()}
{
}

MemoryFile::MemoryFile( DataType &&data):
  dataValue{ data},
  dataPtr{ dataValue.begin()}
{
}

const MemoryFile::DataType& MemoryFile::data() const noexcept
{
  return dataValue;
}

void MemoryFile::finished() noexcept
{
  dataPtr = dataValue.begin();
}

bool MemoryFile::receivedTransferSize( const uint64_t transferSize)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
    << "Received transfer size: " << transferSize;

  return true;
}

void MemoryFile::receivedData( const DataType &data) noexcept
{
  dataValue.insert(
    dataValue.end(),
    data.begin(),
    data.end());

  dataPtr = dataValue.begin();
}

std::optional< uint64_t> MemoryFile::requestedTransferSize()
{
  return dataValue.size();
}

MemoryFile::DataType MemoryFile::sendData( const size_t maxSize) noexcept
{
  DataType::const_iterator startPtr = dataPtr;
  DataType::const_iterator endPtr;

  if ( static_cast< unsigned int>(
    std::distance< DataType::const_iterator>(
      startPtr,
      dataValue.end())) <= maxSize)
  {
    endPtr = dataValue.end();
  }
  else
  {
    endPtr = dataPtr+maxSize;
  }

  dataPtr=endPtr;

  return DataType( startPtr, endPtr);
}

}
