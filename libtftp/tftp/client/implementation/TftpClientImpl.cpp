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
 * @brief Definition of class Tftp::Client::TftpClientImpl.
 **/

#include "TftpClientImpl.hpp"

#include <tftp/client/implementation/TftpClientReadRequestOperationImpl.hpp>
#include <tftp/client/implementation/TftpClientWriteRequestOperationImpl.hpp>

namespace Tftp {
namespace Client {

TftpClientImpl::TftpClientImpl(
	const TftpConfiguration &configuration,
	const OptionList& additionalOptions):
	configuration( configuration),
	options( configuration.getClientOptions( additionalOptions))
{
}

Tftp::Client::TftpClientOperation TftpClientImpl::createReadRequestOperation(
	TftpReceiveDataOperationHandler &handler,
	const UdpAddressType &serverAddress,
	const string &filename,
	const TransferMode mode,
	const UdpAddressType &from)
{
	auto operation = std::make_shared< TftpClientReadRequestOperationImpl>(
		handler,
		*this,
		serverAddress,
		filename,
		mode,
		from);

	return std::bind( &TftpClientReadRequestOperationImpl::operator (), operation);
}

Tftp::Client::TftpClientOperation TftpClientImpl::createReadRequestOperation(
	TftpReceiveDataOperationHandler &handler,
	const UdpAddressType &serverAddress,
	const string &filename,
	const TransferMode mode)
{
	auto operation = std::make_shared< TftpClientReadRequestOperationImpl>(
		handler,
		*this,
		serverAddress,
		filename,
		mode);

	return std::bind( &TftpClientReadRequestOperationImpl::operator (), operation);
}

Tftp::Client::TftpClientOperation TftpClientImpl::createWriteRequestOperation(
	TftpTransmitDataOperationHandler &handler,
	const UdpAddressType &serverAddress,
	const string &filename,
	const TransferMode mode,
	const UdpAddressType &from)
{
	auto operation = std::make_shared< TftpClientWriteRequestOperationImpl>(
		handler,
		*this,
		serverAddress,
		filename,
		mode,
		from);

	return std::bind( &TftpClientWriteRequestOperationImpl::operator (), operation);
}

Tftp::Client::TftpClientOperation TftpClientImpl::createWriteRequestOperation(
	TftpTransmitDataOperationHandler &handler,
	const UdpAddressType &serverAddress,
	const string &filename,
	const TransferMode mode)
{
	auto operation = std::make_shared< TftpClientWriteRequestOperationImpl>(
		handler,
		*this,
		serverAddress,
		filename,
		mode);

	return std::bind( &TftpClientWriteRequestOperationImpl::operator (), operation);
}

const Tftp::TftpConfiguration& TftpClientImpl::getConfiguration( void) const
{
	return configuration;
}

const OptionList& TftpClientImpl::getOptionList( void) const
{
	return options;
}

}
}
