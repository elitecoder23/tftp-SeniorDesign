/*
 * $Date$
 * $Revision$
 */
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Declaration of class Tftp::Server::TftpServerBaseErrorOperation.
 **/

#ifndef TFTP_SERVER_TFTPSERVERBASEERROROPERATION_HPP
#define TFTP_SERVER_TFTPSERVERBASEERROROPERATION_HPP

#include <tftp/server/Server.hpp>
#include <tftp/packets/Packets.hpp>

#include <boost/asio.hpp>

namespace Tftp {
namespace Server {

/**
 * @brief Base class of TFTP error operation.
 **/
class TftpServerBaseErrorOperation
{
  protected:
    /**
     * @brief Constructor of error operation
     *
     * @param[in] clientAddress
     *   Where the error packet shall be transmitted to.
     **/
    TftpServerBaseErrorOperation( const UdpAddressType &clientAddress);

    /**
     * @brief Constructor of error operation
     *
     * @param[in] clientAddress
     *   Where the error packet shall be transmitted to.
     * @param[in] from
     *   The communication source.
     **/
    TftpServerBaseErrorOperation(
      const UdpAddressType &clientAddress,
      const UdpAddressType &from);

    /**
     * @brief Default destructor.
     **/
    virtual ~TftpServerBaseErrorOperation() noexcept;

    /**
     * @brief Sends the given error packet.
     *
     * @param[in] error
     *   The error packet.
     **/
    void sendError( const Packets::BaseErrorPacket &error);

  private:
    const UdpAddressType clientAddress;
    boost::asio::io_service ioService;
    boost::asio::ip::udp::socket socket;
};

}
}

#endif
