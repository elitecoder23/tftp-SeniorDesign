// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

Options Options_options( std::string_view rawOptions )
{
  Options options;

  for ( auto optionsString{ rawOptions }; !optionsString.empty(); )
  {
    const auto nameEnd{ optionsString.find( '\0' ) };

    if ( nameEnd == std::string_view::npos )
    {
      BOOST_THROW_EXCEPTION( Packets::InvalidPacketException()
        << Helper::AdditionalInfo{ "Unexpected end of input data" } );
    }

    std::string name{ optionsString.substr( 0, nameEnd ) };
    optionsString = optionsString.substr( nameEnd + 1U );

    const auto valueEnd{ optionsString.find( '\0' ) };

    if ( valueEnd == std::string_view::npos )
    {
      BOOST_THROW_EXCEPTION( Packets::InvalidPacketException()
        << Helper::AdditionalInfo{ "Unexpected end of input data" } );
    }

    std::string value{ optionsString.substr( 0, valueEnd ) };
    optionsString = optionsString.substr( valueEnd + 1U );

    options.emplace( std::move( name ), std::move( value ) );
  }

  return options;
}

RawOptions Options_rawOptions( const Options &options )
{
  RawOptions rawOptions;

  // copy options
  for ( const auto &[ name, option ] : options )
  {
    // option name
    auto rawName{ Helper::RawData_asRawData( name ) };
    rawOptions.insert( rawOptions.end(), rawName.begin(), rawName.end() );

    // name value divider
    rawOptions.push_back( std::byte{ 0 } );

    // option value
    auto rawValue{ Helper::RawData_asRawData( option ) };
    rawOptions.insert( rawOptions.end(), rawValue.begin(), rawValue.end() );

    // option terminator
    rawOptions.push_back( std::byte{ 0 } );
  }

  return rawOptions;
}

}
