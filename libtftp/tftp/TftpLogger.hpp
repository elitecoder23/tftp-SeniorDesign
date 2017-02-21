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
 * @brief Declaration of logger Tftp::TftpLogger.
 **/

#ifndef TFTP_TFTPLOGGER_HPP
#define TFTP_TFTPLOGGER_HPP

#include <tftp/Tftp.hpp>

#include <helper/Logger.hpp>

#include <boost/log/sources/global_logger_storage.hpp>

namespace Tftp {

BOOST_LOG_INLINE_GLOBAL_LOGGER_CTOR_ARGS( TftpLogger, channel_severity_logger_mt, (boost::log::keywords::channel = "tftp"))

}

#endif
