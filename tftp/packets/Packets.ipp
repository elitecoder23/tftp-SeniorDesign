/**
* @file
* @copyright
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* @author Thomas Vogt, thomas@thomas-vogt.de
*
* @brief Definition of Namespace Tftp::Packets.
**/

#ifndef TFTP_PACKETS_PACKETS_IPP
#define TFTP_PACKETS_PACKETS_IPP

namespace Tftp::Packets {

template< std::unsigned_integral IntT >
Options::value_type TftpOptions_setOption( KnownOptions option, IntT value )
{
  return { std::string{ TftpOptions_name( option ) }, std::to_string( value ) };
}

template< std::unsigned_integral IntT >
std::pair< bool, std::optional< IntT > >
TftpOptions_getOption(
  const Options &options,
  KnownOptions option,
  const IntT min,
  const IntT max )
{
  auto optionIt{ options.find( TftpOptions_name( option ) ) };

  // option not set
  if ( optionIt == options.end() )
  {
    return { true, {} };
  }

  const auto optionValue{ std::stoull( optionIt->second ) };

  if ( ( optionValue < min ) || ( optionValue > max ) )
  {
    return { false, {} };
  }

  return { true, static_cast< IntT >( optionValue ) };
}

}

#endif
