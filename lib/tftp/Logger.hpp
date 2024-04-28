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
 * @brief Declaration of Logger Tftp::Logger.
 **/

#ifndef TFTP_LOGGER_HPP
#define TFTP_LOGGER_HPP

#include "tftp/Tftp.hpp"

#include "helper/Logger.hpp"

#include <boost/log/sources/global_logger_storage.hpp>

namespace Tftp {

//! TFTP Logger
BOOST_LOG_INLINE_GLOBAL_LOGGER_CTOR_ARGS(
  Logger,
  Helper::ChannelSeverityLoggerMt,
  ( boost::log::keywords::channel = "Tftp" ) )

}

#endif
