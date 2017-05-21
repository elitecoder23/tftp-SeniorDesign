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
 * @brief Definition of class Tftp::Server::ErrorOperation.
 **/

#include "ErrorOperation.hpp"

#include <tftp/TftpLogger.hpp>
#include <tftp/TftpException.hpp>

namespace Tftp {
namespace Server {

ErrorOperation::ErrorOperation(
  boost::asio::io_service &ioService,
  const UdpAddressType &clientAddress,
  const UdpAddressType &from,
  const ErrorCode errorCode,
  const string &errorMessage,
  OperationCompletedHandler completionHandler)
try :
  completionHandler( completionHandler),
  socket( ioService),
  errorInfo( Packets::ErrorPacket( errorCode, errorMessage))
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

ErrorOperation::ErrorOperation(
  boost::asio::io_service &ioService,
  UdpAddressType &&clientAddress,
  UdpAddressType &&from,
  ErrorCode errorCode,
  string &&errorMessage,
  OperationCompletedHandler completionHandler)
try :
  completionHandler( completionHandler),
  socket( ioService),
  errorInfo( Packets::ErrorPacket( errorCode, errorMessage))
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

ErrorOperation::ErrorOperation(
  boost::asio::io_service &ioService,
  const UdpAddressType &clientAddress,
  const ErrorCode errorCode,
  const string &errorMessage,
  OperationCompletedHandler completionHandler)
try :
  completionHandler( completionHandler),
  socket( ioService),
  errorInfo( Packets::ErrorPacket( errorCode, errorMessage))
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

    BOOST_THROW_EXCEPTION( TftpException() << AdditionalInfo( err.what()));
  }
}
catch ( boost::system::system_error &err)
{
  BOOST_THROW_EXCEPTION(
    CommunicationException() << AdditionalInfo( err.what()));
}

ErrorOperation::ErrorOperation(
  boost::asio::io_service &ioService,
  UdpAddressType &&clientAddress,
  const ErrorCode errorCode,
  string &&errorMessage,
  OperationCompletedHandler completionHandler)
try :
  completionHandler( completionHandler),
  socket( ioService),
  errorInfo( Packets::ErrorPacket( errorCode, errorMessage))
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

    BOOST_THROW_EXCEPTION( TftpException() << AdditionalInfo( err.what()));
  }
}
catch ( boost::system::system_error &err)
{
  BOOST_THROW_EXCEPTION(
    CommunicationException() << AdditionalInfo( err.what()));
}


ErrorOperation::~ErrorOperation() noexcept
{
}

void ErrorOperation::start()
{
  assert( errorInfo);

  sendError( *errorInfo);
}

void ErrorOperation::gracefulAbort(
  const ErrorCode /* errorCode */,
  const string &/* errorMessage*/)
{
  // do nothing
}

void ErrorOperation::abort()
{
  // do nothing
}

const ErrorOperation::ErrorInfo& ErrorOperation::getErrorInfo() const
{
  return errorInfo;
}

void ErrorOperation::sendError( const Packets::ErrorPacket &error)
{
  try
  {
    socket.send( boost::asio::buffer( static_cast< RawTftpPacketType>( error)));

    if (completionHandler)
    {
      completionHandler( TransferStatus::Successful);
    }
  }
  catch ( boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) << err.what();

    if (completionHandler)
    {
      completionHandler( TransferStatus::CommunicationError);
    }
  }
}

}
}
