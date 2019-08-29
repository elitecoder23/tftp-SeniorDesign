/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Server::ErrorOperation.
 **/

#include "ErrorOperation.hpp"

#include <tftp/TftpLogger.hpp>
#include <tftp/TftpException.hpp>

namespace Tftp::Server {

ErrorOperation::ErrorOperation(
  boost::asio::io_context &ioContext,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  const boost::asio::ip::udp::endpoint &local,
  const ErrorCode errorCode,
  std::string_view errorMessage)
try :
  completionHandler{ completionHandler},
  socket{ ioContext},
  errorInfoV{ Packets::ErrorPacket{ errorCode, errorMessage}}
{
  try
  {
    socket.open( remote.protocol());

    socket.bind( local);

    socket.connect( remote);
  }
  catch ( boost::system::system_error &err)
  {
    if ( socket.is_open())
    {
      socket.close();
    }

    BOOST_THROW_EXCEPTION(
      CommunicationException() << AdditionalInfo( err.what()));
  }
}
catch ( boost::system::system_error &err)
{
  BOOST_THROW_EXCEPTION(
    CommunicationException() << AdditionalInfo( err.what()));
}

void ErrorOperation::start()
{
  assert( errorInfoV);

  sendError( *errorInfoV);
}

void ErrorOperation::gracefulAbort(
  const ErrorCode errorCode [[maybe_unused]],
  std::string_view errorMessage [[maybe_unused]])
{
  // do nothing
}

void ErrorOperation::abort()
{
  // do nothing
}

const ErrorOperation::ErrorInfo& ErrorOperation::errorInfo() const
{
  return errorInfoV;
}

void ErrorOperation::sendError( const Packets::ErrorPacket &error)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
    << "TX: " << static_cast< std::string>( error);

  try
  {
    socket.send( boost::asio::buffer( static_cast< RawTftpPacket>( error)));

    if (completionHandler)
    {
      completionHandler( TransferStatus::Successful);
    }
  }
  catch ( boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
      << err.what();

    if (completionHandler)
    {
      completionHandler( TransferStatus::CommunicationError);
    }
  }
}

}
