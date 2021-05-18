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
Options TftpOptions_options(
  RawOptions::const_iterator begin,
  RawOptions::const_iterator end )
{
  Options options{};

  while ( begin != end)
  {
    auto nameBegin{ begin};
    auto nameEnd{ std::find( nameBegin, end, 0)};

    if ( nameEnd == end)
    {
      BOOST_THROW_EXCEPTION( Packets::InvalidPacketException()
        << Helper::AdditionalInfo{ "Unexpected end of input data"});
    }

    auto valueBegin{ nameEnd + 1U};

    if ( valueBegin == end)
    {
      BOOST_THROW_EXCEPTION( Packets::InvalidPacketException()
        << Helper::AdditionalInfo{ "Unexpected end of input data"});
    }

    auto valueEnd{ std::find( valueBegin, end, 0)};

    if ( valueEnd == end)
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
    rawOptions.push_back( 0);

    // option value
    rawOptions.insert( rawOptions.end(), option.begin(), option.end());

    // option terminator
    rawOptions.push_back( 0);
  }

  return rawOptions;
}

}
