/*
 * $Date$
 * $Revision$
 */
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

#include <tftp/options/IntegerOption.hpp>
#include <tftp/options/StringOption.hpp>
#include <tftp/TftpException.hpp>

#include <helper/Logger.hpp>

#include <algorithm>

namespace Tftp {
namespace Options {

OptionList::OptionList()
{
}

OptionList::OptionList(
  RawOptionsType::const_iterator begin,
  RawOptionsType::const_iterator end)
{
  while (begin != end)
  {
    //! @todo add throw of exception
    RawOptionsType::const_iterator nameBegin = begin;
    RawOptionsType::const_iterator nameEnd = std::find( nameBegin, end, 0);

    if (nameEnd==end)
    {
      //! @throw InvalidPacketException on Unexpected end of input data
      BOOST_THROW_EXCEPTION( InvalidPacketException() <<
        AdditionalInfo( "Unexpected end of input data"));
    }

    RawOptionsType::const_iterator valueBegin = nameEnd + 1;

    if (valueBegin == end)
    {
      //! @throw InvalidPacketException on Unexpected end of input data
      BOOST_THROW_EXCEPTION( InvalidPacketException() <<
        AdditionalInfo( "Unexpected end of input data"));
    }

    RawOptionsType::const_iterator valueEnd = std::find( valueBegin, end, 0);

    if (valueEnd == end)
    {
      //! @throw InvalidPacketException on Unexpected end of input data
      BOOST_THROW_EXCEPTION( InvalidPacketException() <<
        AdditionalInfo( "Unexpected end of input data"));
    }

    string name( nameBegin, nameEnd);
    string value( valueBegin, valueEnd);

    // insert option as string option
    options.insert( std::make_pair(
      name,
      std::make_shared< StringOption>( name, value)));

    begin = valueEnd + 1;
  }
}

bool OptionList::hasOptions() const
{
  return !options.empty();
}

const OptionList::OptionMap& OptionList::getOptions() const
{
  return options;
}

OptionList::OptionMap& OptionList::getOptions()
{
  return options;
}

OptionList::RawOptionsType OptionList::getRawOptions() const
{
  RawOptionsType rawOptions;

  // copy options
  for ( const auto &option : options)
  {
    // option name
    rawOptions.insert( rawOptions.end(), option.first.begin(), option.first.end());

    // name value divider
    rawOptions.push_back( 0);

    // option value
    const string value( *(option.second));
    rawOptions.insert( rawOptions.end(), value.begin(), value.end());

    // option terminator
    rawOptions.push_back( 0);
  }

  return rawOptions;
}

void OptionList::setOptions( const OptionMap &options)
{
	this->options = options;
}

bool OptionList::hasOption( const string &name) const
{
  return options.count( name) >= 1;
}

bool OptionList::hasOption( const KnownOptions option) const
{
  string optionName = Option::getOptionName( option);

  if ( optionName.empty())
  {
    return false;
  }

  return hasOption( optionName);
}

const OptionList::OptionPointer OptionList::getOption( const string &name) const
{
  OptionMap::const_iterator it =
    options.find( name);

  return (it != options.end()) ?
    it->second : OptionPointer();
}

void OptionList::setOption( const string &name, const string &value)
{
  // If option already exists remove it first
  if (hasOption( name))
  {
    removeOption( name);
  }

  // Add option
  options.insert( std::make_pair(
    name,
    OptionPointer( new StringOption( name, value))));
}

void OptionList::setOption( const OptionPointer option)
{
  // If option already exists remove it first
  if (hasOption( option->getName()))
  {
    removeOption( option->getName());
  }

  // Add option
  options.insert( std::make_pair( option->getName(), option));
}

void OptionList::removeOption( const std::string &name)
{
  options.erase( name);
}

void OptionList::removeOption( const KnownOptions option)
{
  string optionName = Option::getOptionName( option);

  if (!optionName.empty())
  {
    options.erase( optionName);
  }
}

void OptionList::addBlocksizeOptionClient(
  uint16_t requestedBlocksize,
  uint16_t minBlocksize)
{
  assert(
    (requestedBlocksize >= TFTP_OPTION_BLOCKSIZE_MIN) &&
    (requestedBlocksize <= TFTP_OPTION_BLOCKSIZE_MAX));

  assert(
    (minBlocksize >= TFTP_OPTION_BLOCKSIZE_MIN) &&
    (minBlocksize <= TFTP_OPTION_BLOCKSIZE_MAX));

  assert( minBlocksize <= requestedBlocksize);

  OptionPointer entry = OptionPointer(
    new BlockSizeOptionClient(
      Option::getOptionName( KnownOptions::BLOCKSIZE),
      requestedBlocksize,
      NegotiateMinMaxRange< uint16_t>( minBlocksize, requestedBlocksize)));

  setOption( entry);
}

void OptionList::addBlocksizeOptionServer(
  const uint16_t minBlocksize,
  const uint16_t maxBlocksize)
{
  assert(
    (minBlocksize >= TFTP_OPTION_BLOCKSIZE_MIN) &&
    (minBlocksize <= TFTP_OPTION_BLOCKSIZE_MAX));

  assert(
    (maxBlocksize >= TFTP_OPTION_BLOCKSIZE_MIN) &&
    (maxBlocksize <= TFTP_OPTION_BLOCKSIZE_MAX));

  assert( minBlocksize <= maxBlocksize);

  OptionPointer entry = OptionPointer(
    new BlockSizeOptionServer(
      Option::getOptionName( KnownOptions::BLOCKSIZE),
      maxBlocksize,
      NegotiateMinMaxSmaller< uint16_t>( minBlocksize, maxBlocksize)));

  setOption( entry);
}

uint16_t OptionList::getBlocksizeOption() const
{
  OptionMap::const_iterator optionIt = options.find(
    Option::getOptionName( KnownOptions::BLOCKSIZE));

  // option not set
  if (optionIt == options.end())
  {
    return 0;
  }

  const BlockSizeOptionBase* integerOption =
    dynamic_cast< const BlockSizeOptionBase*>(
      optionIt->second.get());

  // invalid cast
  if (!integerOption)
  {
    return 0;
  }

  return *integerOption;
}

void OptionList::addTimeoutOptionClient( const uint8_t timeout)
{
  // satisfy TFTP spec (MAX is not checked because this is the maximum range of uint8_t)
  assert( timeout >= TFTP_OPTION_TIMEOUT_MIN);

  OptionPointer entry = OptionPointer(
    new TimeoutOptionClient(
      Option::getOptionName( KnownOptions::TIMEOUT),
      timeout,
      NegotiateExactValue< uint8_t>( timeout)));

  setOption( entry);
}

void OptionList::addTimeoutOptionServer(
  const uint8_t minTimeout,
  const uint8_t maxTimeout)
{
  // satisfy TFTP spec (MAX is not checked because this is the maximum range of uint8_t)
  assert( minTimeout >= TFTP_OPTION_TIMEOUT_MIN);

  assert( maxTimeout >= TFTP_OPTION_TIMEOUT_MIN);

  assert( (minTimeout <= maxTimeout));

  OptionPointer entry = OptionPointer(
    new TimeoutOptionServer(
      Option::getOptionName( KnownOptions::TIMEOUT),
      maxTimeout,
      NegotiateMinMaxRange< uint8_t>( minTimeout, maxTimeout)));

  setOption( entry);
}

uint8_t OptionList::getTimeoutOption() const
{
  OptionMap::const_iterator optionIt = options.find(
    Option::getOptionName( KnownOptions::TIMEOUT));

  // option not set
  if (optionIt == options.end())
  {
    return 0;
  }

  const TimeoutOptionBase* integerOption =
    dynamic_cast< const TimeoutOptionBase*>(
      optionIt->second.get());

  // invalid cast
  if (!integerOption)
  {
    return 0;
  }

  return *integerOption;
}

void OptionList::addTransferSizeOption( const uint64_t transferSize)
{
  OptionPointer entry = OptionPointer(
    new TransferSizeOptionServerClient(
      Option::getOptionName( KnownOptions::TRANSFER_SIZE),
      transferSize,
      NegotiateAlwaysPass< uint64_t>()));

  setOption( entry);
}

void OptionList::removeTransferSizeOption()
{
  removeOption( Option::getOptionName( KnownOptions::TRANSFER_SIZE));
}

bool OptionList::hasTransferSizeOption() const
{
  return hasOption( Option::getOptionName( KnownOptions::TRANSFER_SIZE));
}

uint64_t OptionList::getTransferSizeOption() const
{
  OptionMap::const_iterator optionIt = options.find(
    Option::getOptionName( KnownOptions::TRANSFER_SIZE));

  // option not set
  if (optionIt == options.end())
  {
    return 0;
  }

  const TransferSizeOptionBase* integerOption =
    dynamic_cast< const TransferSizeOptionBase*>(
      optionIt->second.get());

  // invalid cast
  if (!integerOption)
  {
    return 0;
  }

  return *integerOption;
}

OptionList OptionList::negotiateServer( const OptionList &clientOptions) const
{
  OptionList negotiatedOptions;

  // iterate over each received option
  for ( const auto & clientOption : clientOptions.getOptions())
  {
    OptionMap::const_iterator negotiationEntryIt = options.find(
      clientOption.first);

    // not found -> ignore option
    if (negotiationEntryIt == options.end())
    {
      continue;
    }

    // negotiate option
    OptionPointer newOptionValue = negotiationEntryIt->second->negotiate(
      *(clientOption.second));

    // negotiation has returned a value -> copy option to output list
    if (newOptionValue)
    {
      negotiatedOptions.setOption( newOptionValue);
    }
  }

  return negotiatedOptions;
}

OptionList OptionList::negotiateClient( const OptionList &serverOptions) const
{
  // we make sure, that the received options are not empty
  assert( !serverOptions.getOptions().empty());

  OptionList negotiatedOptions;

  // iterate over each received option
  for ( const auto & serverOption : serverOptions.getOptions())
  {
    // find negotiation entry
    OptionMap::const_iterator negotiationEntryIt = options.find(
      serverOption.first);

    // not found -> server sent an option, which cannot come from us
    if (negotiationEntryIt == options.end())
    {
      return OptionList();
    }

    // negotiate option, if failed also fail on top level
    OptionPointer newOptionValue = negotiationEntryIt->second->negotiate(
      (*serverOption.second));

    // check negotiation result
    if (!newOptionValue)
    {
      // negotiation failed -> return immediately
      return OptionList();
    }

    // negotiation has returned a value -> copy option to output list
    negotiatedOptions.setOption( newOptionValue);
  }

  return negotiatedOptions;
}

OptionList::string OptionList::toString() const
{
  if ( options.empty())
  {
    return "(NONE)";
  }

  string result;

  // iterate over all options
  for ( const auto option : options)
  {
    result += option.second->toString() + ";";
  }

  return result;
}

}
}
