/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Options::OptionList.
 **/

#include "OptionList.hpp"

#include <tftp/options/OptionNegotiation.hpp>
#include <tftp/TftpException.hpp>

#include <tftp/packets/PacketException.hpp>

#include <helper/SafeCast.hpp>

#include <algorithm>

namespace Tftp::Options {

std::string_view OptionList::optionName( const KnownOptions option) noexcept
{
  switch ( option)
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

Options OptionList::options(
  RawOptions::const_iterator begin,
  RawOptions::const_iterator end)
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

OptionList::RawOptions OptionList::rawOptions( const Options &options)
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

std::string OptionList::toString( const Options &options)
{
  if ( options.empty())
  {
    return "(NONE)";
  }

  std::string result{};

  // iterate over all options
  for ( const auto &[name,option] : options)
  {
    result += name;
    result += ":";
    result += option;
    result += ";";
  }

  return result;
}

bool OptionList::empty() const
{
  return optionsV.empty();
}

const Options& OptionList::options() const
{
  return optionsV;
}

bool OptionList::has( std::string_view name) const
{
  return optionsV.count( name) >= 1;
}

bool OptionList::has( const KnownOptions option) const
{
  auto optionN{ optionName( option)};

  if ( optionN.empty())
  {
    return false;
  }

  return has( optionN);
}

std::string_view OptionList::option( std::string_view name) const
{
  auto it{ optionsV.find( name)};

  return (it != optionsV.end()) ?
    it->second : std::string_view{};
}

void OptionList::option(
  std::string_view name,
  std::string_view value,
  NegotiateOption negotiateOption)
{
  // If option already exists remove it first
  if ( has( name))
  {
    remove( name);
  }

  // Add option
  optionsV.emplace( name, value);
  optionsNegotiationV.emplace( name, std::move( negotiateOption));
}

void OptionList::remove( std::string_view name)
{
  // Find and remove options value
  if ( auto pos{ optionsV.find( name)}; pos != optionsV.end())
  {
    optionsV.erase( pos );
  }

  // Find and remove options negotiation
  if (
    auto pos{ optionsNegotiationV.find( name)};
    pos != optionsNegotiationV.end())
  {
    optionsNegotiationV.erase( pos );
  }
}

void OptionList::remove( const KnownOptions option)
{
  const auto optionN{ optionName( option)};

  if ( !optionN.empty())
  {
    remove( optionN);
  }
}

void OptionList::blocksizeClient(
  const uint16_t requestedBlocksize,
  const uint16_t minBlocksize)
{
  if (
      ((requestedBlocksize < BlocksizeOptionMin)
        || (requestedBlocksize > BlocksizeOptionMax))
    ||
      ((minBlocksize < BlocksizeOptionMin)
        || (minBlocksize > BlocksizeOptionMax))
    ||
      (minBlocksize > requestedBlocksize))
  {
    BOOST_THROW_EXCEPTION( TftpException()
      << Helper::AdditionalInfo( "Invalid Parameters"));
  }

  option(
    optionName( KnownOptions::BlockSize),
    OptionNegotiation::toString( requestedBlocksize),
    std::bind(
      &NegotiateMinMaxRange::negotiate,
      NegotiateMinMaxRange{ minBlocksize, requestedBlocksize},
      std::placeholders::_1));
}

void OptionList::blocksizeServer(
  const uint16_t minBlocksize,
  const uint16_t maxBlocksize)
{
  if (
      ((minBlocksize < BlocksizeOptionMin)
      || (minBlocksize > BlocksizeOptionMax))
    ||
      ((maxBlocksize < BlocksizeOptionMin)
        || (maxBlocksize > BlocksizeOptionMax))
    ||
      (minBlocksize > maxBlocksize))
  {
    BOOST_THROW_EXCEPTION( TftpException()
      << Helper::AdditionalInfo( "Invalid Parameters"));
  }

  option(
    optionName( KnownOptions::BlockSize),
    OptionNegotiation::toString( maxBlocksize),
    std::bind(
      &NegotiateMinMaxRange::negotiate,
      NegotiateMinMaxSmaller{ minBlocksize, maxBlocksize},
      std::placeholders::_1));
}

std::optional< uint16_t> OptionList::blocksize() const
{
  auto optionIt{ optionsV.find( optionName( KnownOptions::BlockSize ))};

  // option not set
  if ( optionIt == optionsV.end() )
  {
    return {};
  }

  return Helper::safeCast< uint16_t >( OptionNegotiation::toInt( optionIt->second ));
}

void OptionList::timeoutOptionClient( const uint8_t timeout )
{
  // satisfy TFTP spec (MAX is not checked because this is the maximum range of uint8_t)
  if ( timeout < TimeoutOptionMin)
  {
    BOOST_THROW_EXCEPTION( TftpException()
      << Helper::AdditionalInfo( "Invalid Parameters"));
  }

  option(
    optionName( KnownOptions::Timeout),
    OptionNegotiation::toString( timeout),
    std::bind(
      &NegotiateExactValue::negotiate,
      NegotiateExactValue{ timeout},
      std::placeholders::_1));
}

void OptionList::timeoutOptionServer(
  const uint8_t minTimeout,
  const uint8_t maxTimeout)
{
  // satisfy TFTP spec (MAX is not checked because this is the maximum range of uint8_t)
  if ( ( minTimeout < TimeoutOptionMin)
    || ( maxTimeout < TimeoutOptionMin)
    || ( minTimeout > maxTimeout))
  {
    BOOST_THROW_EXCEPTION( TftpException()
      << Helper::AdditionalInfo( "Invalid Parameters"));
  }

  option(
    optionName( KnownOptions::Timeout),
    OptionNegotiation::toString( maxTimeout),
    std::bind(
      &NegotiateMinMaxRange::negotiate,
      NegotiateMinMaxRange{ minTimeout, maxTimeout},
      std::placeholders::_1));
}

std::optional< uint8_t> OptionList::timeoutOption() const
{
  auto optionIt{ optionsV.find( optionName( KnownOptions::Timeout))};

  // option not set
  if (optionIt == optionsV.end())
  {
    return {};
  }

  return Helper::safeCast< uint8_t >( OptionNegotiation::toInt( optionIt->second));
}

void OptionList::transferSizeOption( const uint64_t transferSize)
{
  option(
    optionName( KnownOptions::TransferSize),
    OptionNegotiation::toString( transferSize),
    std::bind(
      &NegotiateMinMaxRange::negotiate,
      NegotiateAlwaysPass{},
      std::placeholders::_1));
}

void OptionList::removeTransferSizeOption()
{
  remove( KnownOptions::TransferSize);
}

std::optional< uint64_t> OptionList::transferSizeOption() const
{
  auto pos{ optionsV.find( optionName( KnownOptions::TransferSize))};

  // option not set
  if ( pos == optionsV.end())
  {
    return {};
  }

  return static_cast< uint64_t >( OptionNegotiation::toInt( pos->second));
}

OptionList OptionList::negotiateServer( const Options &clientOptions) const
{
  OptionList negotiatedOptions{};

  // iterate over each received option
  for ( const auto & [clientOptionName, clientOptionValue] : clientOptions)
  {
    auto serverOption{ optionsV.find( clientOptionName)};
    auto optionNegotiation{ optionsNegotiationV.find( clientOptionName)};

    // not found -> ignore option
    if ((serverOption == optionsV.end()
      || (optionNegotiation == optionsNegotiationV.end())))
    {
      continue;
    }

    // negotiate option
    auto newOptionValue{ optionNegotiation->second( clientOptionValue)};

    // negotiation has returned a value -> copy option to output list
    if ( !newOptionValue.empty())
    {
      negotiatedOptions.option(
        clientOptionName,
        newOptionValue,
        optionNegotiation->second);
    }
  }

  return negotiatedOptions;
}

OptionList OptionList::negotiateClient( const Options &serverOptions) const
{
  // we make sure, that the received options are not empty
  if ( serverOptions.empty())
  {
    // normally this should not happen. Empty result is error anyway.
    return {};
  }

  OptionList negotiatedOptions{};

  // iterate over each received option
  for ( const auto & [serverOptionName, serverOptionValue] : serverOptions)
  {
    // find negotiation entry
    auto clientOption{ optionsV.find( serverOptionName)};
    auto optionNegotiation{ optionsNegotiationV.find( serverOptionName)};

    // not found -> server sent an option, which cannot come from us
    if ((clientOption == optionsV.end()
      || (optionNegotiation == optionsNegotiationV.end())))
    {
      return {};
    }

    // negotiate option, if failed also fail on top level
    auto newOptionValue{ optionNegotiation->second( serverOptionValue)};

    // check negotiation result
    if ( newOptionValue.empty())
    {
      // negotiation failed -> return immediately
      return {};
    }

    // negotiation has returned a value -> copy option to output list
    negotiatedOptions.option(
      serverOptionName,
      newOptionValue,
      optionNegotiation->second);
  }

  return negotiatedOptions;
}

}
