/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * $Date$
 * $Revision$
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class MemoryFile
 **/

#include "MemoryFile.hpp"

#include <helper/Logger.hpp>

using namespace Tftp::File;

MemoryFile::MemoryFile( void):
	dataPtr( data.begin())
{
}

MemoryFile::MemoryFile( const DataType &data):
	data( data),
	dataPtr( this->data.begin())
{
}

MemoryFile::MemoryFile( DataType &&data):
	data( data),
	dataPtr( this->data.begin())
{
}

const MemoryFile::DataType& MemoryFile::getData( void) const noexcept
{
	return data;
}

void MemoryFile::finishedOperation( void) noexcept
{
	dataPtr = data.begin();
}

bool MemoryFile::receivedTransferSize( const uint64_t transferSize)
{
	BOOST_LOG_TRIVIAL( info) << "Received transfer size: " << transferSize;
	return true;
}

void MemoryFile::receviedData( const DataType &data) noexcept
{
	this->data.insert(
		this->data.end(),
		data.begin(),
		data.end());

	dataPtr = this->data.begin();
}

bool MemoryFile::requestedTransferSize( uint64_t &transferSize)
{
	transferSize = data.size();
	return true;
}

MemoryFile::DataType MemoryFile::sendData( const unsigned int maxSize) noexcept
{
	DataType::const_iterator startPtr = dataPtr;
	DataType::const_iterator endPtr;

	if ( static_cast< unsigned int>(
		std::distance< std::vector< uint8_t>::const_iterator>(
			startPtr,
			data.end())) <= maxSize)
	{
		endPtr = data.end();
	}
	else
	{
		endPtr = dataPtr+maxSize;
	}

	dataPtr=endPtr;

	return DataType( startPtr, endPtr);
}
