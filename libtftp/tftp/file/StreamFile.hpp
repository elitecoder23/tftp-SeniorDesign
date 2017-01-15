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
 * @brief Declaration of class Tftp::File::StreamFile.
 **/

#ifndef TFTP_FILE_STREAMFILE_HPP
#define TFTP_FILE_STREAMFILE_HPP

#include <tftp/file/TftpFile.hpp>
#include <iostream>

namespace Tftp {
namespace File {
/**
 * @brief File implementation, which uses an std::iostream for file I/O
 *   handling.
 **/
class StreamFile: public TftpFile
{
	public:
		/**
		 * @brief Creates the StreamFile with the given stream as in/ output.
		 *
		 * @param[in] stream
		 *   The data stream - A reference to this stream is stored. The stream
		 *   must be valid until this instance has been destroyed.
		 **/
		StreamFile( std::iostream &stream);

		/**
		 * @brief Creates the StreamFile with the given stream as in/ output and
		 *  the size information provided.
		 *
		 * @param[in] stream
		 *   The data stream - A reference to this stream is stored. The stream
		 *   must be valid until this instance has been destroyed.
		 * @param[in] size
		 *   The size of the stream (e.g. file stream)
		 **/
		StreamFile( std::iostream &stream, size_t size);

		/**
		 * @copydoc TftpFile::finishedOperation()
		 **/
		virtual void finishedOperation() noexcept override final;

		/**
		 * @copydoc TftpFile::receivedTransferSize()
		 **/
		virtual bool receivedTransferSize( uint64_t transferSize) override final;

		/**
		 * @copydoc TftpFile::receviedData()
		 **/
		virtual void receviedData( const DataType &data) noexcept override final;

		/**
		 * @copydoc TftpFile::requestedTransferSize()
		 **/
		virtual bool requestedTransferSize( uint64_t &transferSize) override final;

		/**
		 * @copydoc TftpFile::sendData()
		 **/
		virtual DataType sendData( unsigned int maxSize) noexcept override final;

	private:
		//! the data stream
		std::iostream &stream;
		bool hasSize;
		size_t size;
};

}
}

#endif
