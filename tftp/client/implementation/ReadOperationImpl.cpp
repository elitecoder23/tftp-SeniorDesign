/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Client::ReadOperationImpl.
 **/

#include "ReadOperationImpl.hpp"

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>
#include <tftp/packets/TftpOptions.hpp>
#include <tftp/packets/Options.hpp>

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>
#include <tftp/ReceiveDataHandler.hpp>

#include <helper/Dump.hpp>

#include <utility>

namespace Tftp::Client {

ReadOperationImpl::ReadOperationImpl(
  boost::asio::io_context &ioContext,
  ReadOperationConfiguration configuration ):
  OperationImpl{
    ioContext,
    configuration.tftpTimeout,
    configuration.tftpRetries,
    static_cast< uint16_t >( Packets::DefaultTftpDataPacketHeaderSize
      + std::max(
        Packets::DefaultDataSize,
        configuration.optionsConfiguration.blockSizeOption.get_value_or(
          Packets::DefaultDataSize ) ) ),
    configuration.completionHandler,
    configuration.remote,
    configuration.local },
  configurationV{ std::move( configuration ) }
{
  BOOST_LOG_FUNCTION()

  if ( !configurationV.optionNegotiationHandler
    || !configurationV.dataHandler )
  {
    BOOST_THROW_EXCEPTION( TftpException()
      << Helper::AdditionalInfo{ "Parameter Invalid" }
      << TransferPhaseInfo{ TransferPhase::Initialisation } );
  }
}

void ReadOperationImpl::request()
{
  BOOST_LOG_FUNCTION()

  try
  {
    // Reset data handler
    configurationV.dataHandler->reset();

    receiveDataSize = Packets::DefaultDataSize;
    lastReceivedBlockNumber = 0U;

    // initialise Options with additional options
    Packets::Options options{ configurationV.additionalOptions };

    // Block size Option
    if ( configurationV.optionsConfiguration.blockSizeOption )
    {
      options.try_emplace(
        Packets::TftpOptions_name( Packets::KnownOptions::BlockSize ),
        std::to_string( *configurationV.optionsConfiguration.blockSizeOption ) );
    }

    // Timeout Option
    if ( configurationV.optionsConfiguration.timeoutOption )
    {
      options.try_emplace(
        Packets::TftpOptions_name( Packets::KnownOptions::Timeout ),
        std::to_string( static_cast< uint16_t >(
          configurationV.optionsConfiguration.timeoutOption->count() ) ) );
    }

    // Add transfer size option with size '0' if requested.
    if ( configurationV.optionsConfiguration.handleTransferSizeOption )
    {
      // assure that transfer size is set to zero for read request
      options.try_emplace(
        Packets::TftpOptions_name( Packets::KnownOptions::TransferSize ),
        "0" );
    }

    // send read request packet
    sendFirst( Packets::ReadRequestPacket{
      configurationV.filename,
      configurationV.mode,
      std::move( options ) } );

    // wait for answers
    receiveFirst();
  }
  catch ( const boost::exception &e )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Exception during request " << boost::diagnostic_information( e );

    finished( TransferStatus::CommunicationError );
  }
}

void ReadOperationImpl::finished(
  const TransferStatus status,
  ErrorInfo &&errorInfo ) noexcept
{
  // inform base class
  OperationImpl::finished( status, std::move( errorInfo ) );

  // Inform data handler
  configurationV.dataHandler->finished();
}

void ReadOperationImpl::dataPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::DataPacket &dataPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "RX: " << static_cast< std::string>( dataPacket );

  // check retransmission of last packet
  if ( dataPacket.blockNumber() == lastReceivedBlockNumber )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
      << "Received last data package again. Re-ACK them";

    // Retransmit last ACK packet
    send( Packets::AcknowledgementPacket{ lastReceivedBlockNumber } );

    // if received data size is smaller than the expected
    if ( dataPacket.dataSize() < receiveDataSize )
    {
      // last packet has been received and operation is finished
      if ( configurationV.dally )
      {
        // wait for potential retry of Data.
        receiveDally();
      }
      else
      {
        finished( TransferStatus::Successful );
      }
    }
    else
    {
      // otherwise, wait for next data package
      receive();
    }

    return;
  }

  // check unexpected block number
  if ( dataPacket.blockNumber() != lastReceivedBlockNumber.next() )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Wrong Data packet block number";

    // send error packet
    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::IllegalTftpOperation,
      "Block Number not expected" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );
    return;
  }

  // check for too much data
  if ( dataPacket.dataSize() > receiveDataSize )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Too much data received";

    // send error packet
    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::IllegalTftpOperation,
      "Too much data" };
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );
    return;
  }

  // if block number is 1 -> DATA of write without Options
  if ( ( dataPacket.blockNumber() == static_cast< uint16_t >( 1U ) )
    && ( !oackReceived ) )
  {
    // If options negotiation is aborted by callback - Abort Operation
    Packets::Options options{};
    if ( !configurationV.optionNegotiationHandler( options ) )
    {
      BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
        << "Option Negotiation failed";

      Packets::ErrorPacket errorPacket{
        Packets::ErrorCode::TftpOptionRefused,
        "Option Negotiation Failed" };

      send( errorPacket );

      finished( TransferStatus::TransferError, std::move( errorPacket ) );
      return;
    }
  }

  // pass data
  configurationV.dataHandler->receivedData( dataPacket.data() );

  // increment received block number
  lastReceivedBlockNumber++;

  // send ACK
  send( Packets::AcknowledgementPacket( lastReceivedBlockNumber ) );

  // if received data size is smaller than the expected
  if ( dataPacket.dataSize() < receiveDataSize )
  {
    // last packet has been received and operation is finished
    if ( configurationV.dally )
    {
      // wait for potential retry of Data.
      receiveDally();
    }
    else
    {
      finished( TransferStatus::Successful );
    }
  }
  else
  {
    // otherwise, wait for next data package
    receive();
  }
}

void ReadOperationImpl::acknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::AcknowledgementPacket &acknowledgementPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "RX ERROR: " << static_cast< std::string>( acknowledgementPacket );

  // send Error
  Packets::ErrorPacket errorPacket{
    Packets::ErrorCode::IllegalTftpOperation,
    "ACK not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void ReadOperationImpl::optionsAcknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "RX: " << static_cast< std::string>( optionsAcknowledgementPacket );

  if ( lastReceivedBlockNumber != Packets::BlockNumber{ 0U } )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "OACK must occur after RRQ";

    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::IllegalTftpOperation,
      "OACK must occur after RRQ" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );
    return;
  }

  auto remoteOptions{ optionsAcknowledgementPacket.options() };

  // check empty options - not allowed
  if ( remoteOptions.empty() )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Received option list is empty";

    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::IllegalTftpOperation,
      "Empty OACK not allowed" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );
    return;
  }

  // Block Size Option
  const auto [ blockSizeValid, blockSizeValue ] =
    Packets::Options_getOption< uint16_t >(
      remoteOptions,
      Packets::TftpOptions_name( Packets::KnownOptions::BlockSize ),
      Packets::BlockSizeOptionMin,
      Packets::BlockSizeOptionMax );

  if ( !configurationV.optionsConfiguration.blockSizeOption && blockSizeValue )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Block Size Option not expected";

    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::TftpOptionRefused,
      "Block Size Option not expected" };

    send( errorPacket );

    // Operation completed
    finished(
      TransferStatus::OptionNegotiationError,
      std::move( errorPacket ) );
    return;
  }

  if ( !blockSizeValid )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Block Size Option decoding failed";

    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::TftpOptionRefused,
      "Block Size Option decoding failed" };

    send( errorPacket );

    // Operation completed
    finished(
      TransferStatus::OptionNegotiationError,
      std::move( errorPacket ) );
    return;
  }

  if ( blockSizeValue )
  {
    if ( *blockSizeValue > *configurationV.optionsConfiguration.blockSizeOption )
    {
      BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
        << "Received Block Size Option bigger than negotiated";

      Packets::ErrorPacket errorPacket{
        Packets::ErrorCode::TftpOptionRefused,
        "Block size Option negotiation failed" };

      send( errorPacket );

      // Operation completed
      finished(
        TransferStatus::OptionNegotiationError,
        std::move( errorPacket ) );
      return;
    }

    receiveDataSize = *blockSizeValue;
  }

  // Timeout Option
  const auto [ timeoutValid, timeoutValue ] =
    Packets::Options_getOption< uint8_t>(
      remoteOptions,
      Packets::TftpOptions_name( Packets::KnownOptions::Timeout ),
      Packets::TimeoutOptionMin,
      Packets::TimeoutOptionMax );

  if ( !configurationV.optionsConfiguration.timeoutOption && timeoutValue )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Timeout Option not expected";

    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::TftpOptionRefused,
      "Timeout Option not expected" };

    send( errorPacket );

    // Operation completed
    finished(
      TransferStatus::OptionNegotiationError,
      std::move( errorPacket ) );
    return;
  }

  if ( !timeoutValid )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Timeout Option decoding failed";

    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::TftpOptionRefused,
      "Timeout Option decoding failed" };

    send( errorPacket);

    // Operation completed
    finished(
      TransferStatus::OptionNegotiationError,
      std::move( errorPacket ) );
    return;
  }

  if ( timeoutValue )
  {
    // Timeout Option Response from Server must be equal to Client Value
    if ( std::chrono::seconds{ *timeoutValue }
      != *configurationV.optionsConfiguration.timeoutOption )
    {
      BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
        << "Timeout Option not equal to requested";

      Packets::ErrorPacket errorPacket{
        Packets::ErrorCode::TftpOptionRefused,
        "Timeout Option not equal to requested" };

      send( errorPacket);

      // Operation completed
      finished(
        TransferStatus::OptionNegotiationError,
        std::move( errorPacket ) );
      return;
    }

    receiveTimeout( std::chrono::seconds{ *timeoutValue } );
  }

  // Transfer Size Option
  const auto [ transferSizeValid, transferSizeValue ] =
    Packets::Options_getOption< uint64_t>(
      remoteOptions,
      Packets::TftpOptions_name( Packets::KnownOptions::TransferSize ) );

  if ( !configurationV.optionsConfiguration.handleTransferSizeOption
    && transferSizeValue )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Transfer Size Option not expected";

    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::TftpOptionRefused,
      "Transfer Size Option not expected" };

    send( errorPacket );

    // Operation completed
    finished(
      TransferStatus::OptionNegotiationError,
      std::move( errorPacket ) );
    return;
  }

  if ( !transferSizeValid )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Transfer Size Option decoding failed";

    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::TftpOptionRefused,
      "Transfer Size Option decoding failed" };

    send( errorPacket );

    // Operation completed
    finished(
      TransferStatus::OptionNegotiationError,
      std::move( errorPacket ) );
    return;
  }

  if ( transferSizeValue )
  {
    if ( !configurationV.dataHandler->receivedTransferSize( *transferSizeValue ) )
    {
      Packets::ErrorPacket errorPacket{
        Packets::ErrorCode::DiskFullOrAllocationExceeds,
        "File to big" };

      send( errorPacket );

      // Operation completed
      finished(
        TransferStatus::OptionNegotiationError,
        std::move( errorPacket ) );
      return;
    }
  }

  // Perform additional Option Negotiation
  if ( !configurationV.optionNegotiationHandler( remoteOptions ) )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Option negotiation failed";

    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::TftpOptionRefused,
      "Option negotiation failed" };

    send( errorPacket );

    // Operation completed
    finished(
      TransferStatus::OptionNegotiationError,
      std::move( errorPacket ) );
    return;
  }

  // check that remaining remote options are empty
  if ( !remoteOptions.empty() )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Option negotiation failed - unexpected options";

    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::TftpOptionRefused,
      "Unexpected options" };

    send( errorPacket );

    // Operation completed
    finished(
      TransferStatus::OptionNegotiationError,
      std::move( errorPacket ) );
    return;
  }

  // indicate Options acknowledgement
  oackReceived = true;

  // send Acknowledgment with block number set to 0
  send( Packets::AcknowledgementPacket{ Packets::BlockNumber{ 0 } } );

  // receive next packet
  receive();
}

}
