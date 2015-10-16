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
 * @brief Declaration of class TftpClient.
 **/

#ifndef TFTP_CLIENT_TFTPCLIENT_HPP
#define TFTP_CLIENT_TFTPCLIENT_HPP

#include <tftp/TftpConfiguration.hpp>
#include <tftp/client/Client.hpp>
#include <tftp/options/Options.hpp>

#include <string>

namespace Tftp
{
	namespace Client
	{
		using Tftp::Options::OptionList;

		using std::string;

		/**
		 * @brief The main entry for implementors of a TFTP client.
		 *
		 * This class acts as factory for creating client operations, like read
		 * requests or write requests.
		 **/
		class TftpClient
		{
			public:
				/**
				 * @brief Creates an instance of TFTP client.
				 *
				 * With the instance you can create any client operation instances.
				 *
				 * @param[in] configuration
				 *   The TFTP Configuration
				 * @param[in] additionalOptions
				 *   Additional Options, which shall be used as TFTP client option list.
				 *
				 * @return The created TFTP client instance.
				 **/
				static TftpClientPtr createInstance(
					const TftpConfiguration &configuration = TftpConfiguration(),
					const OptionList& additionalOptions = OptionList());

				//! Default destructor
				virtual ~TftpClient( void) noexcept = default;

				/**
				 * @brief Creates an read request operation (TFTP RRQ).
				 *
				 * @param[in] handler
				 *   Handler for received data.
				 * @param[in] serverAddress
				 *   Where the connection should be established to.
				 * @param[in] filename
				 *   Which file shall be requested
				 * @param[in] mode
				 *   The transfer mode
				 * @param[in] from
				 *   Optional parameter to define the communication source
				 *
				 * @return The client operation instance.
				 **/
				virtual TftpClientOperation createReadRequestOperation(
					TftpReceiveDataOperationHandler &handler,
					const UdpAddressType &serverAddress,
					const string &filename,
					const TransferMode mode,
					const UdpAddressType &from) = 0;

				/**
				 * @brief Creates an read request operation (TFTP RRQ).
				 *
				 * @param[in] handler
				 *   Handler for received data.
				 * @param[in] serverAddress
				 *   Where the connection should be established to.
				 * @param[in] filename
				 *   Which file shall be requested
				 * @param[in] mode
				 *   The transfer mode
				 *
				 * @return The client operation instance.
				 **/
				virtual TftpClientOperation createReadRequestOperation(
					TftpReceiveDataOperationHandler &handler,
					const UdpAddressType &serverAddress,
					const string &filename,
					const TransferMode mode) = 0;

				/**
				 * @brief Creates an write request operation (TFTP WRQ).
				 *
				 * @param[in] handler
				 *   Handler for data.
				 * @param[in] serverAddress
				 *   Where the connection should be established to.
				 * @param[in] filename
				 *   Which file shall be requested
				 * @param[in] mode
				 *   The transfer mode
				 * @param[in] from
				 *   Optional parameter to define the communication source
				 *
				 * @return The client operation instance.
				 **/
				virtual TftpClientOperation createWriteRequestOperation(
					TftpTransmitDataOperationHandler &handler,
					const UdpAddressType &serverAddress,
					const string &filename,
					const TransferMode mode,
					const UdpAddressType &from) = 0;

				/**
				 * @brief Creates an write request operation (TFTP WRQ).
				 *
				 * @param[in] handler
				 *   Handler for data.
				 * @param[in] serverAddress
				 *   Where the connection should be established to.
				 * @param[in] filename
				 *   Which file shall be requested
				 * @param[in] mode
				 *   The transfer mode
				 *
				 * @return The client operation instance.
				 **/
				virtual TftpClientOperation createWriteRequestOperation(
					TftpTransmitDataOperationHandler &handler,
					const UdpAddressType &serverAddress,
					const string &filename,
					const TransferMode mode) = 0;

			protected:
				//! Protected constructor.
				TftpClient( void) = default;
		};
	}
}

#endif
