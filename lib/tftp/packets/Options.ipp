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
 * @brief Definition of Module Tftp::Packets Options.
 **/

#ifndef TFTP_PACKETS_OPTIONS_IPP
#define TFTP_PACKETS_OPTIONS_IPP

namespace Tftp::Packets {

template< std::unsigned_integral IntT >
std::pair< bool, std::optional< IntT > >
Options_getOption(
  Options &options,
  const std::string &name,
  const IntT min,
  const IntT max )
{
  auto option{ options.extract( name ) };

  // option not set
  if ( !option )
  {
    return { true, {} };
  }

  try
  {
    const auto optionValue{ std::stoull( option.mapped() ) };

    if ( ( optionValue < min ) || ( optionValue > max ) )
    {
      return { false, {} };
    }

    return { true, static_cast< IntT >( optionValue ) };
  }
  catch ( const std::invalid_argument & )
  {
    return { true, {} };
  }
  catch ( const std::out_of_range & )
  {
    return { true, {} };
  }
  catch ( ... )
  {
    return { true, {} };
  }
}

}

#endif
