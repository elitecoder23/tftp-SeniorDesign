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

#include "Packets.hpp"

#include <tftp/packets/PacketException.hpp>

namespace Tftp::Packets {

std::string_view TftpOptions_name( KnownOptions option ) noexcept
{
  switch ( option )
  {
    case KnownOptions::BlockSize:
      return "blksize";

    case KnownOptions::Timeout:
      return "timeout";

    case KnownOptions::TransferSize:
      return "tsize";

    default:
      return {};
  }
}

std::string TftpOptions_toString( const Options &options )
{
  if ( options.empty() )
  {
    return "(NONE)";
  }

  std::string result{};

  // iterate over all options
  for ( const auto &[name, option] : options )
  {
    result += name;
    result += ":";
    result += option;
    result += ";";
  }

  return result;
}
Options TftpOptions_options( RawOptionsSpan rawOptions )
{
  Options options{};

  for( auto begin = rawOptions.begin(); begin != rawOptions.end(); )
  {
    auto nameBegin{ begin};
    auto nameEnd{ std::find( nameBegin, rawOptions.end(), 0)};

    if ( nameEnd == rawOptions.end())
    {
      BOOST_THROW_EXCEPTION( Packets::InvalidPacketException()
        << Helper::AdditionalInfo{ "Unexpected end of input data"});
    }

    auto valueBegin{ nameEnd + 1U};

    if ( valueBegin == rawOptions.end())
    {
      BOOST_THROW_EXCEPTION( Packets::InvalidPacketException()
        << Helper::AdditionalInfo{ "Unexpected end of input data"});
    }

    auto valueEnd{ std::find( valueBegin, rawOptions.end(), 0)};

    if ( valueEnd == rawOptions.end())
    {
      BOOST_THROW_EXCEPTION( Packets::InvalidPacketException()
        << Helper::AdditionalInfo{ "Unexpected end of input data"});
    }

    options.emplace(
      std::string{ nameBegin, nameEnd},
      std::string{ valueBegin, valueEnd});

    begin = valueEnd + 1U;
  }

  return options;
}
RawOptions TftpOptions_rawOptions( const Options &options )
{
  RawOptions rawOptions{};

  // copy options
  for ( const auto &[name, option] : options)
  {
    // option name
    rawOptions.insert( rawOptions.end(), name.begin(), name.end());

    // name value divider
    rawOptions.push_back( 0 );

    // option value
    rawOptions.insert( rawOptions.end(), option.begin(), option.end());

    // option terminator
    rawOptions.push_back( 0 );
  }

  return rawOptions;
}

}
