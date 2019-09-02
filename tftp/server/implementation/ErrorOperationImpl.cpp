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

#include "ErrorOperationImpl.hpp"

#include <tftp/TftpLogger.hpp>
#include <tftp/TftpException.hpp>

namespace Tftp::Server {

ErrorOperationImpl::ErrorOperationImpl(
  boost::asio::io_context &ioContext,
  const boost::asio::ip::udp::endpoint &remote,
  const ErrorCode errorCode,
  std::string_view errorMessage)
try :
  socket{ ioContext},
  errorCode{ errorCode},
  errorMessage{ errorMessage}
{
  try
  {
    socket.open( remote.protocol());

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

ErrorOperationImpl::ErrorOperationImpl(
  boost::asio::io_context &ioContext,
  const boost::asio::ip::udp::endpoint &remote,
  const boost::asio::ip::udp::endpoint &local,
  const ErrorCode errorCode,
  std::string_view errorMessage)
try :
  socket{ ioContext},
  errorCode{ errorCode},
  errorMessage{ errorMessage}
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

void ErrorOperationImpl::operator()()
{
  BOOST_LOG_FUNCTION()

  Packets::ErrorPacket errorPacket{ errorCode, errorMessage};

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
    << "TX: " << static_cast< std::string>( errorPacket);

  try
  {
    socket.send( boost::asio::buffer( static_cast< RawTftpPacket>( errorPacket)));
  }
  catch ( boost::system::system_error &err)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
      << err.what();
  }
}


}
