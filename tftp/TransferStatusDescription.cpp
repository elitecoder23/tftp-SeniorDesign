/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::TransferStatusDescription.
 **/

#include "TransferStatusDescription.hpp"

#include <ostream>

namespace Tftp {

using namespace std::literals;

TransferStatusDescription::TransferStatusDescription():
  Description{
    { "Successful"sv,               TransferStatus::Successful},
    { "Communication Error"sv,      TransferStatus::CommunicationError},
    { "Request Error"sv,            TransferStatus::RequestError},
    { "Option Negotiation Error"sv, TransferStatus::OptionNegotiationError},
    { "Transfer Error"sv,           TransferStatus::TransferError},
    { "Aborted"sv,                  TransferStatus::Aborted},
  }
{
}

std::ostream& operator<<(
  std::ostream& stream,
  const TransferStatus transferStatus)
{
  return (stream << TransferStatusDescription::instance().name( transferStatus));
}

}


