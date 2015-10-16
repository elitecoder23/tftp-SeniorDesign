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
 * @brief Definition of class StreamFile
 **/

#include "StreamFile.hpp"

#include <helper/Logger.hpp>

using namespace Tftp::File;

StreamFile::StreamFile( std::iostream &stream):
	stream( stream),
	hasSize( false),
	size( 0)
{
}

StreamFile::StreamFile( std::iostream &stream, size_t size):
	stream( stream),
	hasSize( true),
	size( size)
{
}

void StreamFile::finishedOperation( void) noexcept
{
	stream.flush();
}

bool StreamFile::receivedTransferSize( const uint64_t transferSize)
{
	//! If no size is provided
	if (!hasSize)
	{
		//! Always accept file based on size
		return true;
	}

	//! Accept file if size is matching the maximum allowed one.
	return (transferSize <= size);
}

void StreamFile::receviedData( const std::vector< uint8_t> &data) noexcept
{
	stream.write( reinterpret_cast< const char*>( &data[0]), data.size());
}

bool StreamFile::requestedTransferSize( uint64_t &transferSize)
{
	transferSize = size;
	return hasSize;
}

StreamFile::DataType StreamFile::sendData( const unsigned int maxSize) noexcept
{
	DataType data( maxSize);

	stream.read( reinterpret_cast< char*>(&data[0]), maxSize);

	data.resize( stream.gcount());

	return data;
}
