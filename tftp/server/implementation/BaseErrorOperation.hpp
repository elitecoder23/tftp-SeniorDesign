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
 * @brief Declaration of class Tftp::Server::BaseErrorOperation.
 **/

#ifndef TFTP_SERVER_BASEERROROPERATION_HPP
#define TFTP_SERVER_BASEERROROPERATION_HPP

#include <tftp/server/Server.hpp>
#include <tftp/server/Operation.hpp>
#include <tftp/packets/Packets.hpp>

#include <boost/asio.hpp>

namespace Tftp {
namespace Server {

/**
 * @brief Base class of TFTP error operation.
 **/
class BaseErrorOperation : public Operation
{
  public:
    //! @copydoc Operation::gracefulAbort
    virtual void gracefulAbort(
      ErrorCode errorCode,
      const string &errorMessage = string()) override final;

    //! @copydoc Operation::abort
    virtual void abort() override final;

    /**
     * @brief Default destructor.
     **/
    virtual ~BaseErrorOperation() noexcept;

  protected:
    /**
     * @brief Constructor of error operation
     *
     * @param[in] clientAddress
     *   Where the error packet shall be transmitted to.
     **/
    BaseErrorOperation( const UdpAddressType &clientAddress);

    BaseErrorOperation( UdpAddressType &&clientAddress);

    /**
     * @brief Constructor of error operation
     *
     * @param[in] clientAddress
     *   Where the error packet shall be transmitted to.
     * @param[in] from
     *   The communication source.
     **/
    BaseErrorOperation(
      const UdpAddressType &clientAddress,
      const UdpAddressType &from);

    BaseErrorOperation(
      UdpAddressType &&clientAddress,
      UdpAddressType &&from);

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
