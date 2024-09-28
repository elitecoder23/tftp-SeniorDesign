// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Client::TftpClientImpl.
 **/

#include "TftpClientImpl.hpp"

#include "tftp/client/implementation/ReadOperationImpl.hpp"
#include "tftp/client/implementation/WriteOperationImpl.hpp"

namespace Tftp::Client {

TftpClientImpl::TftpClientImpl( boost::asio::io_context &ioContext ) :
  ioContext{ ioContext }
{
}

TftpClientImpl::~TftpClientImpl() = default;

TftpClient& TftpClientImpl::tftpTimeoutDefault(
  const std::chrono::seconds timeout )
{
  tftpTimeoutDefaultV = timeout;
  return *this;
}

TftpClient& TftpClientImpl::tftpRetriesDefault( const uint16_t retries )
{
  tftpRetriesDefaultV = retries;
  return *this;
}

TftpClient& TftpClientImpl::dallyDefault( const bool dally )
{
  dallyDefaultV = dally;
  return *this;
}

TftpClient& TftpClientImpl::optionsConfigurationDefault(
  TftpOptionsConfiguration optionsConfiguration )
{
  optionsConfigurationDefaultV = std::move( optionsConfiguration );
  return *this;
}

TftpClient& TftpClientImpl::additionalOptions(
  Packets::Options additionalOptions )
{
  additionalOptionsV = std::move( additionalOptions );
  return *this;
}

TftpClient& TftpClientImpl::localDefault( const boost::asio::ip::address local )
{
  localV = local;
  return *this;
}

ReadOperationPtr TftpClientImpl::readOperation()
{
  auto operation{ std::make_shared< ReadOperationImpl >( ioContext ) };

  if ( tftpTimeoutDefaultV )
  {
    operation->tftpTimeout( *tftpTimeoutDefaultV );
  }

  if ( tftpRetriesDefaultV )
  {
    operation->tftpRetries( *tftpRetriesDefaultV );
  }

  if ( dallyDefaultV )
  {
    operation->dally( *dallyDefaultV );
  }

  if ( optionsConfigurationDefaultV )
  {
    operation->optionsConfiguration( *optionsConfigurationDefaultV );
  }

  if ( !additionalOptionsV.empty() )
  {
    operation->additionalOptions( additionalOptionsV );
  }

  if ( !localV.is_unspecified() )
  {
    operation->local( { localV, 0 } );
  }

  return operation;
}

WriteOperationPtr TftpClientImpl::writeOperation()
{
  auto operation{ std::make_shared< WriteOperationImpl >( ioContext ) };

  if ( tftpTimeoutDefaultV )
  {
    operation->tftpTimeout( *tftpTimeoutDefaultV );
  }

  if ( tftpRetriesDefaultV )
  {
    operation->tftpRetries( *tftpRetriesDefaultV );
  }

  if ( optionsConfigurationDefaultV )
  {
    operation->optionsConfiguration( *optionsConfigurationDefaultV );
  }

  if ( !additionalOptionsV.empty() )
  {
    operation->additionalOptions( additionalOptionsV );
  }

  if ( !localV.is_unspecified() )
  {
    operation->local( { localV, 0 } );
  }

  return operation;
}

}
