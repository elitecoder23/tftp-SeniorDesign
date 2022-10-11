/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Module Tftp::Packets TftpOptions.
 **/

#ifndef TFTP_PACKETS_TFTPOPTIONS_IPP
#define TFTP_PACKETS_TFTPOPTIONS_IPP

namespace Tftp::Packets {

template< std::unsigned_integral IntT >
std::pair< bool, std::optional< IntT > >
TftpOptions_getOption(
  const Options &options,
  std::string_view name,
  const IntT min,
  const IntT max )
{
  auto optionIt{ options.find( name ) };

  // option not set
  if ( optionIt == options.end() )
  {
    return { true, {} };
  }

  try
  {
    const auto optionValue{ std::stoull( optionIt->second ) };

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
