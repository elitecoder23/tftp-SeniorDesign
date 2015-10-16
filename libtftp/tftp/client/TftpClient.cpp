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
 * @brief Definition of class TftpClient.
 **/

#include "TftpClient.hpp"

#include <tftp/client/implementation/TftpClientImpl.hpp>

using namespace Tftp::Client;

TftpClientPtr TftpClient::createInstance(
	const TftpConfiguration &configuration,
	const OptionList& additionalOptions)
{
	return std::make_shared< TftpClientImpl>( configuration, additionalOptions);
}
