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
 * @brief Declaration of class MemoryFile.
 **/

#ifndef TFTP_FILE_MEMORYFILE_HPP
#define TFTP_FILE_MEMORYFILE_HPP

#include <tftp/file/TftpFile.hpp>

namespace Tftp
{
	namespace File
	{
		/**
		 * @brief File implementation, which holds all data in memory.
		 **/
		class MemoryFile : public TftpFile
		{
			public:
				/**
				 * @brief Creates a memory file with no current data.
				 *
				 * This constructor is useful for receiving data.
				 **/
				MemoryFile( void);

				/**
				 * @brief Creates a memory file with the given data.
				 *
				 * The data is copied to an internal data structure.
				 *
				 * @param[in] data
				 *   The initial data.
				 **/
				MemoryFile( const DataType &data);

				MemoryFile( DataType &&data);

				/**
				 * @brief Returns a reference to the locally stored data.
				 *
				 * @return The locally stored data
				 **/
				const DataType& getData( void) const noexcept;

				/**
				 * @copydoc TftpFile::finishedOperation()
				 **/
				virtual void finishedOperation( void) noexcept override;

				/**
				 * @copydoc TftpFile::receivedTransferSize()
				 **/
				virtual bool receivedTransferSize( const uint64_t transferSize) override;
				
				/**
				 * @copydoc TftpFile::receviedData()
				 **/
				virtual void receviedData(
					const std::vector<uint8_t> &data) noexcept override;

				/**
				 * @copydoc TftpFile::requestedTransferSize()
				 **/
				virtual bool requestedTransferSize( uint64_t &transferSize) override;
				
				/**
				 * @copydoc TftpFile::sendData()
				 **/
				virtual DataType sendData( const unsigned int maxSize) noexcept override;

			private:
				//! the data
				DataType data;
				//! the current read position
				DataType::const_iterator dataPtr;
		};
	}
}

#endif
