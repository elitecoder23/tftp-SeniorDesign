/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class OptionList.
 **/

#include "OptionList.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>
#include <tftp/options/IntegerOption.hpp>
#include <tftp/options/StringOption.hpp>

#include <helper/Logger.hpp>

#include <algorithm>

namespace Tftp::Options {

OptionList::OptionList()
{
}

OptionList::OptionList(
  RawOptions::const_iterator begin,
  RawOptions::const_iterator end)
{
  while (begin != end)
  {
    //! @todo add throw of exception
    auto nameBegin{ begin};
    auto nameEnd{ std::find( nameBegin, end, 0)};

    if (nameEnd==end)
    {
      BOOST_THROW_EXCEPTION( InvalidPacketException() <<
        AdditionalInfo( "Unexpected end of input data"));
    }

    auto valueBegin{ nameEnd + 1};

    if (valueBegin == end)
    {
      BOOST_THROW_EXCEPTION( InvalidPacketException() <<
        AdditionalInfo( "Unexpected end of input data"));
    }

    auto valueEnd{ std::find( valueBegin, end, 0)};

    if (valueEnd == end)
    {
      BOOST_THROW_EXCEPTION( InvalidPacketException() <<
        AdditionalInfo( "Unexpected end of input data"));
    }

    std::string name( nameBegin, nameEnd);
    std::string value( valueBegin, valueEnd);

    // insert option as string option
    optionsValue.insert( std::make_pair(
      name,
      std::make_shared< StringOption>( name, value)));

    begin = valueEnd + 1;
  }
}

bool OptionList::empty() const
{
  return optionsValue.empty();
}

const OptionList::Options& OptionList::options() const
{
  return optionsValue;
}

OptionList::Options& OptionList::options()
{
  return optionsValue;
}

void OptionList::options( const Options &options)
{
  optionsValue = options;
}

OptionList::RawOptions OptionList::rawOptions() const
{
  RawOptions rawOptions;

  // copy options
  for ( const auto &option : optionsValue)
  {
    // option name
    rawOptions.insert( rawOptions.end(), option.first.begin(), option.first.end());

    // name value divider
    rawOptions.push_back( 0);

    // option value
    const std::string value( *(option.second));
    rawOptions.insert( rawOptions.end(), value.begin(), value.end());

    // option terminator
    rawOptions.push_back( 0);
  }

  return rawOptions;
}

bool OptionList::has( const std::string &name) const
{
  return optionsValue.count( name) >= 1;
}

bool OptionList::has( const KnownOptions option) const
{
  const auto optionName{ Option::optionName( option)};

  if ( optionName.empty())
  {
    return false;
  }

  return has( optionName);
}

const OptionPtr OptionList::get( const std::string &name) const
{
  auto it{ optionsValue.find( name)};

  return (it != optionsValue.end()) ?
    it->second : OptionPtr();
}

void OptionList::set( const std::string &name, const std::string &value)
{
  // If option already exists remove it first
  if (has( name))
  {
    remove( name);
  }

  // Add option
  optionsValue.insert( std::make_pair(
    name,
    std::make_shared< StringOption>( name, value)));
}

void OptionList::set( const std::string &&name, const std::string &&value)
{
  // If option already exists remove it first
  if (has( name))
  {
    remove( name);
  }

  // Add option
  optionsValue.insert( std::make_pair(
    name,
    std::make_shared< StringOption>( std::move( name), std::move( value))));
}

void OptionList::set( const OptionPtr option)
{
  // If option already exists remove it first
  if (has( option->name()))
  {
    remove( option->name());
  }

  // Add option
  optionsValue.insert( std::make_pair( option->name(), option));
}

void OptionList::remove( const std::string &name)
{
  optionsValue.erase( name);
}

void OptionList::remove( const KnownOptions option)
{
  const auto optionName{ Option::optionName( option)};

  if (!optionName.empty())
  {
    optionsValue.erase( optionName);
  }
}

void OptionList::blocksizeClient(
  uint16_t requestedBlocksize,
  uint16_t minBlocksize)
{
  assert(
    (requestedBlocksize >= BlocksizeOptionMin) &&
    (requestedBlocksize <= BlocksizeOptionMax));

  assert(
    (minBlocksize >= BlocksizeOptionMin) &&
    (minBlocksize <= BlocksizeOptionMax));

  assert( minBlocksize <= requestedBlocksize);

  OptionPtr entry( std::make_shared< BlockSizeOptionClient>(
      Option::optionName( KnownOptions::BlockSize),
      requestedBlocksize,
      NegotiateMinMaxRange< uint16_t>( minBlocksize, requestedBlocksize)));

  set( entry);
}

void OptionList::blocksizeServer(
  const uint16_t minBlocksize,
  const uint16_t maxBlocksize)
{
  assert(
    (minBlocksize >= BlocksizeOptionMin) &&
    (minBlocksize <= BlocksizeOptionMax));

  assert(
    (maxBlocksize >= BlocksizeOptionMin) &&
    (maxBlocksize <= BlocksizeOptionMax));

  assert( minBlocksize <= maxBlocksize);

  OptionPtr entry( std::make_shared< BlockSizeOptionServer>(
      Option::optionName( KnownOptions::BlockSize),
      maxBlocksize,
      NegotiateMinMaxSmaller< uint16_t>( minBlocksize, maxBlocksize)));

  set( entry);
}

std::optional< uint16_t> OptionList::blocksize() const
{
  auto optionIt{ optionsValue.find(
    Option::optionName( KnownOptions::BlockSize))};

  // option not set
  if (optionIt == optionsValue.end())
  {
    return {};
  }

  const BlockSizeOptionBase* integerOption =
    dynamic_cast< const BlockSizeOptionBase*>(
      optionIt->second.get());

  // invalid cast
  if (!integerOption)
  {
    return {};
  }

  return *integerOption;
}

void OptionList::timeoutOptionClient( const uint8_t timeout)
{
  // satisfy TFTP spec (MAX is not checked because this is the maximum range of uint8_t)
  assert( timeout >= TimeoutOptionMin);

  OptionPtr entry( std::make_shared< TimeoutOptionClient>(
      Option::optionName( KnownOptions::Timeout),
      timeout,
      NegotiateExactValue< uint8_t>( timeout)));

  set( entry);
}

void OptionList::timeoutOptionServer(
  const uint8_t minTimeout,
  const uint8_t maxTimeout)
{
  // satisfy TFTP spec (MAX is not checked because this is the maximum range of uint8_t)
  assert( minTimeout >= TimeoutOptionMin);

  assert( maxTimeout >= TimeoutOptionMin);

  assert( (minTimeout <= maxTimeout));

  OptionPtr entry( std::make_shared< TimeoutOptionServer>(
      Option::optionName( KnownOptions::Timeout),
      maxTimeout,
      NegotiateMinMaxRange< uint8_t>( minTimeout, maxTimeout)));

  set( entry);
}

std::optional< uint8_t> OptionList::timeoutOption() const
{
  auto optionIt{ optionsValue.find(
    Option::optionName( KnownOptions::Timeout))};

  // option not set
  if (optionIt == optionsValue.end())
  {
    return {};
  }

  const TimeoutOptionBase* integerOption =
    dynamic_cast< const TimeoutOptionBase*>(
      optionIt->second.get());

  // invalid cast
  if (!integerOption)
  {
    return {};
  }

  return *integerOption;
}

void OptionList::transferSizeOption( const uint64_t transferSize)
{
  OptionPtr entry( std::make_shared< TransferSizeOptionServerClient>(
      Option::optionName( KnownOptions::TransferSize),
      transferSize,
      NegotiateAlwaysPass< uint64_t>()));

  set( entry);
}

void OptionList::removeTransferSizeOption()
{
  remove( Option::optionName( KnownOptions::TransferSize));
}

std::optional< uint64_t> OptionList::transferSizeOption() const
{
  auto optionIt{ optionsValue.find(
    Option::optionName( KnownOptions::TransferSize))};

  // option not set
  if (optionIt == optionsValue.end())
  {
    return {};
  }

  const TransferSizeOptionBase* integerOption =
    dynamic_cast< const TransferSizeOptionBase*>(
      optionIt->second.get());

  // invalid cast
  if (!integerOption)
  {
    return {};
  }

  return *integerOption;
}

OptionList OptionList::negotiateServer( const OptionList &clientOptions) const
{
  OptionList negotiatedOptions;

  // iterate over each received option
  for ( const auto & clientOption : clientOptions.options())
  {
    auto negotiationEntryIt{ optionsValue.find( clientOption.first)};

    // not found -> ignore option
    if (negotiationEntryIt == optionsValue.end())
    {
      continue;
    }

    // negotiate option
    OptionPtr newOptionValue( negotiationEntryIt->second->negotiate(
      static_cast< std::string>( *(clientOption.second))));

    // negotiation has returned a value -> copy option to output list
    if (newOptionValue)
    {
      negotiatedOptions.set( newOptionValue);
    }
  }

  return negotiatedOptions;
}

OptionList OptionList::negotiateClient( const OptionList &serverOptions) const
{
  // we make sure, that the received options are not empty
  assert( !serverOptions.options().empty());

  OptionList negotiatedOptions;

  // iterate over each received option
  for ( const auto & serverOption : serverOptions.options())
  {
    // find negotiation entry
    auto negotiationEntryIt{ optionsValue.find( serverOption.first)};

    // not found -> server sent an option, which cannot come from us
    if (negotiationEntryIt == optionsValue.end())
    {
      return OptionList();
    }

    // negotiate option, if failed also fail on top level
    OptionPtr newOptionValue( negotiationEntryIt->second->negotiate(
      static_cast< std::string>(*serverOption.second)));

    // check negotiation result
    if (!newOptionValue)
    {
      // negotiation failed -> return immediately
      return OptionList();
    }

    // negotiation has returned a value -> copy option to output list
    negotiatedOptions.set( newOptionValue);
  }

  return negotiatedOptions;
}

std::string OptionList::toString() const
{
  if ( optionsValue.empty())
  {
    return "(NONE)";
  }

  std::string result;

  // iterate over all options
  for ( const auto &option : optionsValue)
  {
    result += option.second->toString() + ";";
  }

  return result;
}

}
