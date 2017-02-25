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
 * @brief Definition of class Tftp::Server::BaseErrorOperation.
 **/

#include "BaseErrorOperation.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>
#include <tftp/packets/BaseErrorPacket.hpp>

namespace Tftp {
namespace Server {

void BaseErrorOperation::gracefulAbort(
  const ErrorCode /* errorCode */,
  const string &/* errorMessage*/)
{
  // do nothing
}

void BaseErrorOperation::abort()
{
  // do nothing
}

BaseErrorOperation::~BaseErrorOperation() noexcept
{
  try
  {
    socket.close();
  }
  catch ( boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) << err.what();
  }
}


BaseErrorOperation::BaseErrorOperation(
  const UdpAddressType &clientAddress)
try :
  clientAddress( clientAddress),
  socket( ioService)
{
  try
  {
    socket.open( clientAddress.protocol());

    socket.connect( clientAddress);
  }
  catch ( boost::system::system_error &err)
  {
    if ( socket.is_open())
    {
      socket.close();
    }

    //! @throw CommunicationException on system_error
    BOOST_THROW_EXCEPTION(
      CommunicationException() << AdditionalInfo( err.what()));
  }
}
catch ( boost::system::system_error &err)
{
  //! @throw CommunicationException on system_error
  BOOST_THROW_EXCEPTION(
    CommunicationException() << AdditionalInfo( err.what()));
}

BaseErrorOperation::BaseErrorOperation(
  UdpAddressType &&clientAddress)
try :
  clientAddress( clientAddress),
  socket( ioService)
{
  try
  {
    socket.open( clientAddress.protocol());

    socket.connect( clientAddress);
  }
  catch ( boost::system::system_error &err)
  {
    if ( socket.is_open())
    {
      socket.close();
    }

    //! @throw CommunicationException on system_error
    BOOST_THROW_EXCEPTION(
      CommunicationException() << AdditionalInfo( err.what()));
  }
}
catch ( boost::system::system_error &err)
{
  //! @throw CommunicationException on system_error
  BOOST_THROW_EXCEPTION(
    CommunicationException() << AdditionalInfo( err.what()));
}

BaseErrorOperation::BaseErrorOperation(
  const UdpAddressType &clientAddress,
  const UdpAddressType &from)
try :
  clientAddress( clientAddress),
  socket( ioService)
{
  try
  {
    socket.open( clientAddress.protocol());

    socket.bind( from);

    socket.connect( clientAddress);
  }
  catch ( boost::system::system_error &err)
  {
    if ( socket.is_open())
    {
      socket.close();
    }

    BOOST_THROW_EXCEPTION( TftpException() << AdditionalInfo( err.what()));
  }
}
catch ( boost::system::system_error &err)
{
  BOOST_THROW_EXCEPTION(
    CommunicationException() << AdditionalInfo( err.what()));
}

BaseErrorOperation::BaseErrorOperation(
  UdpAddressType &&clientAddress,
  UdpAddressType &&from)
try :
  clientAddress( clientAddress),
  socket( ioService)
{
  try
  {
    socket.open( clientAddress.protocol());

    socket.bind( from);

    socket.connect( clientAddress);
  }
  catch ( boost::system::system_error &err)
  {
    if ( socket.is_open())
    {
      socket.close();
    }

    BOOST_THROW_EXCEPTION( TftpException() << AdditionalInfo( err.what()));
  }
}
catch ( boost::system::system_error &err)
{
  BOOST_THROW_EXCEPTION(
    CommunicationException() << AdditionalInfo( err.what()));
}

void BaseErrorOperation::sendError(
  const Packets::BaseErrorPacket &error)
{
  try
  {
    socket.send( boost::asio::buffer( error.encode()));
  }
  catch ( boost::system::system_error &err)
  {
    BOOST_THROW_EXCEPTION(
      CommunicationException() << AdditionalInfo( err.what()));
  }
}

}
}
