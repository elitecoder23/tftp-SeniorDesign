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
 * @brief Declaration of class Tftp::Server::TftpServerBaseErrorOperation.
 **/

#ifndef TFTP_SERVER_TFTPSERVERBASEERROROPERATION_HPP
#define TFTP_SERVER_TFTPSERVERBASEERROROPERATION_HPP

#include <tftp/server/Server.hpp>
#include <tftp/packet/Packet.hpp>

#include <boost/asio.hpp>

namespace Tftp {
namespace Server {

// Forward declaration of BaseErrorPacket
using Tftp::Packet::BaseErrorPacket;

/**
 * @brief Base class of TFTP error operation.
 **/
class TftpServerBaseErrorOperation
{
	public:
		typedef boost::asio::ip::udp::endpoint AddressType;

	protected:
		/**
		 * @brief Constructor of error operation
		 *
		 * @param[in] clientAddress
		 *   Where the error packet shall be transmitted to.
		 *
		 * @throw CommunicationException
		 **/
		TftpServerBaseErrorOperation( const AddressType &clientAddress);

		/**
		 * @brief Constructor of error operation
		 *
		 * @param[in] clientAddress
		 *   Where the error packet shall be transmitted to.
		 * @param[in] from
		 *   The communication source.
		 *
		 * @throw CommunicationException
		 **/
		TftpServerBaseErrorOperation(
			const AddressType &clientAddress,
			const AddressType &from);

		/**
		 * @brief Default destructor.
		 **/
		virtual ~TftpServerBaseErrorOperation( void) noexcept;

		/**
		 * @brief Sends the given error packet.
		 *
		 * @param[in] error
		 *   The error packet.
		 *
		 * @throw CommunicationException
		 **/
		void sendError( const BaseErrorPacket &error);

	private:
		const AddressType clientAddress;
		boost::asio::io_service ioService;
		boost::asio::ip::udp::socket socket;
};

}
}

#endif
