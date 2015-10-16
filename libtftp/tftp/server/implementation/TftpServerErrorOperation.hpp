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
 * @brief Declaration of class TftpServerErrorOperation.
 **/

#ifndef TFTP_SERVER_TFTPSERVERERROROPERATION_HPP
#define TFTP_SERVER_TFTPSERVERERROROPERATION_HPP

#include <tftp/Tftp.hpp>
#include <tftp/server/implementation/TftpServerBaseErrorOperation.hpp>

#include <string>

namespace Tftp
{
	namespace Server
	{
		using std::string;

		/**
		 * @brief This operation can be used to transfer an error message back to
		 *  the initiator of an TFTP request.
		 **/
		class TftpServerErrorOperation: public TftpServerBaseErrorOperation
		{
			public:
				/**
				 * @brief Initialises the error operation.
				 *
				 * @param[in] clientAddress
				 *   Where the error packet shall be transmitted to.
				 * @param[in] errorCode
				 *   The error code of the error packet.
				 * @param[in] errorMessage
				 *   The error message of the packet.
				 * @param[in] from
				 *   Optional parameter to define the communication source
				 *
				 * @throw CommunicationException
				 **/
				TftpServerErrorOperation(
					const AddressType &clientAddress,
					const AddressType &from,
					const ErrorCode errorCode,
					const string &errorMessage);

				/**
				 * @brief Initialises the error operation.
				 *
				 * @param[in] clientAddress
				 *   Where the error packet shall be transmitted to.
				 * @param[in] errorCode
				 *   The error code of the error packet.
				 * @param[in] errorMessage
				 *   The error message of the packet.
				 *
				 * @throw CommunicationException
				 **/
				TftpServerErrorOperation(
					const AddressType &clientAddress,
					const ErrorCode errorCode,
					const string &errorMessage);

				/**
				 *
				 **/
				void operator ()( void);

			private:
				//! The error code
				const ErrorCode errorCode;
				//! The error message
				const string errorMessage;
		};
	}
}

#endif
