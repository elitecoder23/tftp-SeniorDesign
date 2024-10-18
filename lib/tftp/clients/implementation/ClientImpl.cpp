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
 * @brief Definition of Class Tftp::Clients::ClientImpl.
 **/

#include "ClientImpl.hpp"

#include <tftp/clients/implementation/ReadOperationImpl.hpp>
#include <tftp/clients/implementation/WriteOperationImpl.hpp>

namespace Tftp::Clients {

ClientImpl::ClientImpl( boost::asio::io_context &ioContext ) :
  ioContext{ ioContext }
{
}

ClientImpl::~ClientImpl() = default;

Client& ClientImpl::tftpTimeoutDefault(
  const std::chrono::seconds timeout )
{
  tftpTimeoutDefaultV = timeout;
  return *this;
}

Client& ClientImpl::tftpRetriesDefault( const uint16_t retries )
{
  tftpRetriesDefaultV = retries;
  return *this;
}

Client& ClientImpl::dallyDefault( const bool dally )
{
  dallyDefaultV = dally;
  return *this;
}

Client& ClientImpl::optionsConfigurationDefault(
  TftpOptionsConfiguration optionsConfiguration )
{
  optionsConfigurationDefaultV = std::move( optionsConfiguration );
  return *this;
}

Client& ClientImpl::additionalOptions(
  Packets::Options additionalOptions )
{
  additionalOptionsV = std::move( additionalOptions );
  return *this;
}

Client& ClientImpl::localDefault( const boost::asio::ip::address local )
{
  localV = local;
  return *this;
}

ReadOperationPtr ClientImpl::readOperation()
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

WriteOperationPtr ClientImpl::writeOperation()
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
