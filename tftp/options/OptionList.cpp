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

#include <tftp/packets/PacketException.hpp>

#include <tftp/TftpLogger.hpp>

#include <helper/Logger.hpp>
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

  while (begin != end)
  {
    auto nameBegin{ begin};
    auto nameEnd{ std::find( nameBegin, end, 0)};

    if (nameEnd==end)
    {
      BOOST_THROW_EXCEPTION( Packets::InvalidPacketException()
        << AdditionalInfo( "Unexpected end of input data"));
    }

    auto valueBegin{ nameEnd + 1};

    if (valueBegin == end)
    {
      BOOST_THROW_EXCEPTION( Packets::InvalidPacketException()
        << AdditionalInfo( "Unexpected end of input data"));
    }

    auto valueEnd{ std::find( valueBegin, end, 0)};

    if (valueEnd == end)
    {
      BOOST_THROW_EXCEPTION( Packets::InvalidPacketException()
        << AdditionalInfo( "Unexpected end of input data"));
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
  return optionsValue.empty();
}

const Options& OptionList::options() const
{
  return optionsValue;
}

bool OptionList::has( std::string_view name) const
{
  return optionsValue.count( name) >= 1;
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

std::string_view OptionList::option(  std::string_view name) const
{
  auto it{ optionsValue.find( name)};

  return (it != optionsValue.end()) ?
    it->second : std::string_view{};
}

void OptionList::option(
  std::string_view name,
  std::string_view value,
  NegotiateOption negotiateOption)
{
  // If option already exists remove it first
  if (has( name))
  {
    remove( name);
  }

  // Add option
  optionsValue.emplace( std::make_pair(
    name,
    value));

  optionsNegotiationValue.emplace( std::make_pair(
    name,
    std::move( negotiateOption)));
}

void OptionList::remove( std::string_view name)
{
  // Find and remove options value
  if ( auto pos{ optionsValue.find( name)}; pos != optionsValue.end())
  {
    optionsValue.erase( pos );
  }

  // Find and remove options negotiation
  if (
    auto pos{ optionsNegotiationValue.find( name)};
    pos != optionsNegotiationValue.end())
  {
    optionsNegotiationValue.erase( pos );
  }
}

void OptionList::remove( const KnownOptions option)
{
  auto optionN{ optionName( option)};

  if (!optionN.empty())
  {
    remove( optionN);
  }
}

void OptionList::blocksizeClient(
  uint16_t requestedBlocksize,
  uint16_t minBlocksize)
{
  //! @todo change assertion to exception throw
  assert(
    (requestedBlocksize >= BlocksizeOptionMin) &&
    (requestedBlocksize <= BlocksizeOptionMax));

  assert(
    (minBlocksize >= BlocksizeOptionMin) &&
    (minBlocksize <= BlocksizeOptionMax));

  assert( minBlocksize <= requestedBlocksize);

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
  //! @todo change assertion to exception throw
  assert(
    (minBlocksize >= BlocksizeOptionMin) &&
    (minBlocksize <= BlocksizeOptionMax));

  assert(
    (maxBlocksize >= BlocksizeOptionMin) &&
    (maxBlocksize <= BlocksizeOptionMax));

  assert( minBlocksize <= maxBlocksize);

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
  auto optionIt{ optionsValue.find( optionName( KnownOptions::BlockSize))};

  // option not set
  if (optionIt == optionsValue.end())
  {
    return {};
  }

  return safeCast< uint16_t>( OptionNegotiation::toInt( optionIt->second));
}

void OptionList::timeoutOptionClient( const uint8_t timeout)
{
  //! @todo change assertion to exception throw
  // satisfy TFTP spec (MAX is not checked because this is the maximum range of uint8_t)
  assert( timeout >= TimeoutOptionMin);

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
  //! @todo change assertion to exception throw
  // satisfy TFTP spec (MAX is not checked because this is the maximum range of uint8_t)
  assert( minTimeout >= TimeoutOptionMin);

  assert( maxTimeout >= TimeoutOptionMin);

  assert( (minTimeout <= maxTimeout));

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
  auto optionIt{ optionsValue.find( optionName( KnownOptions::Timeout))};

  // option not set
  if (optionIt == optionsValue.end())
  {
    return {};
  }

  return safeCast< uint8_t >( OptionNegotiation::toInt( optionIt->second));
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
  auto pos{ optionsValue.find( optionName( KnownOptions::TransferSize))};

  // option not set
  if ( pos == optionsValue.end())
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
    auto serverOption{ optionsValue.find( clientOptionName)};
    auto optionNegotiation{ optionsNegotiationValue.find( clientOptionName)};

    // not found -> ignore option
    if ((serverOption == optionsValue.end()
      || (optionNegotiation == optionsNegotiationValue.end())))
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
  if (serverOptions.empty())
  {
    return {};
  }

  OptionList negotiatedOptions{};

  // iterate over each received option
  for ( const auto & [serverOptionName, serverOptionValue] : serverOptions)
  {
    // find negotiation entry
    auto clientOption{ optionsValue.find( serverOptionName)};
    auto optionNegotiation{ optionsNegotiationValue.find( serverOptionName)};

    // not found -> server sent an option, which cannot come from us
    if ((clientOption == optionsValue.end()
      || (optionNegotiation == optionsNegotiationValue.end())))
    {
      return OptionList();
    }

    // negotiate option, if failed also fail on top level
    auto newOptionValue{ optionNegotiation->second( serverOptionValue)};

    // check negotiation result
    if (newOptionValue.empty())
    {
      // negotiation failed -> return immediately
      return OptionList();
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
