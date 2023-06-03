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

#include "Options.hpp"

#include <tftp/packets/PacketException.hpp>

#include <helper/Exception.hpp>

#include <boost/exception/all.hpp>

namespace Tftp::Packets {

std::string Options_toString( const Options &options )
{
  if ( options.empty() )
  {
    return "(NONE)";
  }

  std::string result{};

  // iterate over all options
  for ( const auto &[ name, value ] : options )
  {
    result += name;
    result += ":";
    result += value;
    result += ";";
  }

  return result;
}

Options Options_options( RawOptionsSpan rawOptions )
{
  Options options{};

  for( auto begin = rawOptions.begin(); begin != rawOptions.end(); )
  {
    auto nameBegin{ begin };

    // Option Name is delimited by "\0" character
    auto nameEnd{ std::find( nameBegin, rawOptions.end(), 0 ) };

    if ( nameEnd == rawOptions.end() )
    {
      BOOST_THROW_EXCEPTION( Packets::InvalidPacketException()
        << Helper::AdditionalInfo{ "Unexpected end of input data" } );
    }

    auto valueBegin{ nameEnd + 1U };

    if ( valueBegin == rawOptions.end() )
    {
      BOOST_THROW_EXCEPTION( Packets::InvalidPacketException()
        << Helper::AdditionalInfo{ "Unexpected end of input data"} );
    }

    // Option Value is delimited by "\0" character
    auto valueEnd{ std::find( valueBegin, rawOptions.end(), 0 ) };

    if ( valueEnd == rawOptions.end() )
    {
      BOOST_THROW_EXCEPTION( Packets::InvalidPacketException()
        << Helper::AdditionalInfo{ "Unexpected end of input data" } );
    }

    options.emplace(
      std::string{ nameBegin, nameEnd },
      std::string{ valueBegin, valueEnd } );

    begin = valueEnd + 1U;
  }

  return options;
}

RawOptions Options_rawOptions( const Options &options )
{
  RawOptions rawOptions{};

  // copy options
  for ( const auto &[ name, option ] : options )
  {
    // option name
    rawOptions.insert( rawOptions.end(), name.begin(), name.end() );

    // name value divider
    rawOptions.push_back( 0 );

    // option value
    rawOptions.insert( rawOptions.end(), option.begin(), option.end() );

    // option terminator
    rawOptions.push_back( 0 );
  }

  return rawOptions;
}

}
