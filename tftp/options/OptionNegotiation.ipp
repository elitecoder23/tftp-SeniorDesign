/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Options::OptionNegotiation.
 **/

#include <tftp/TftpException.hpp>

#include <helper/SafeCast.hpp>

#include <charconv>
#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace Tftp::Options {

inline std::string OptionNegotiation::toString( const uint64_t value )
{
  return std::to_string( value );
}

inline uint64_t OptionNegotiation::toInt( std::string_view value )
{
  uint64_t intValue{};
  auto result{ std::from_chars( value.data(), value.data() + value.size(), intValue )};

  if ( result.ec != std::errc{})
  {
    BOOST_THROW_EXCEPTION( OptionNegotiationException()
      << Helper::AdditionalInfo( "Integer Conversion failed" ));
  }

  return intValue;
}

inline std::string OptionNegotiation::negotiate(
  std::string_view optionValue ) const
{
  return negotiateInt( toInt( optionValue ));
}

}
