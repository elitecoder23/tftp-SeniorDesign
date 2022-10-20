/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Client::WriteOperationImpl.
 **/

#include "WriteOperationImpl.hpp"

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>
#include <tftp/packets/TftpOptions.hpp>

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>
#include <tftp/TransmitDataHandler.hpp>

#include <helper/Dump.hpp>

#include <utility>

namespace Tftp::Client {

WriteOperationImpl::WriteOperationImpl(
  boost::asio::io_context &ioContext,
  WriteOperationConfiguration configuration ):
  OperationImpl{
    ioContext,
    configuration.tftpTimeout,
    configuration.tftpRetries,
    Packets::DefaultMaxPacketSize,
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

void WriteOperationImpl::request()
{
  BOOST_LOG_FUNCTION()

  try
  {
    // Reset data handler
    configurationV.dataHandler->reset();

    transmitDataSize = Packets::DefaultDataSize;
    lastDataPacketTransmitted = false;
    lastTransmittedBlockNumber = 0U;
    lastReceivedBlockNumber = 0xFFFFU;

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

    // Add transfer size option if requested.
    if ( configurationV.optionsConfiguration.handleTransferSizeOption )
    {
      // If the handler supplies a transfer size
      if (
        transferSize = configurationV.dataHandler->requestedTransferSize();
        transferSize )
      {
        // set transfer size TFTP option
        options.try_emplace(
          Packets::TftpOptions_name( Packets::KnownOptions::TransferSize ),
          std::to_string( *transferSize ) );
      }
    }

    // send write request packet
    sendFirst( Packets::WriteRequestPacket{
      configurationV.filename,
      configurationV.mode,
      options } );

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

void WriteOperationImpl::finished(
  const TransferStatus status,
  ErrorInfo &&errorInfo ) noexcept
{
  // inform base class
  OperationImpl::finished( status, std::move( errorInfo ) );

  // Inform data handler
  configurationV.dataHandler->finished();
}

void WriteOperationImpl::sendData()
{
  BOOST_LOG_FUNCTION()

  lastTransmittedBlockNumber++;

  const Packets::DataPacket data{
    lastTransmittedBlockNumber,
    configurationV.dataHandler->sendData( transmitDataSize ) };

  if ( data.dataSize() < transmitDataSize )
  {
    lastDataPacketTransmitted = true;
  }

  // send packet
  send( data );
}

void WriteOperationImpl::dataPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::DataPacket &dataPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "RX ERROR: " << static_cast< std::string>( dataPacket );

  Packets::ErrorPacket errorPacket{
    Packets::ErrorCode::IllegalTftpOperation,
    "DATA not expected" };

  send( errorPacket );

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void WriteOperationImpl::acknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::AcknowledgementPacket &acknowledgementPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "RX: " << static_cast< std::string>( acknowledgementPacket );

  // check retransmission
  if ( acknowledgementPacket.blockNumber() == lastReceivedBlockNumber )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
      << "Received previous ACK packet: retry of last data package - "
         "IGNORE it due to Sorcerer's Apprentice Syndrome";

    return;
  }

  // check invalid block number
  if ( acknowledgementPacket.blockNumber() != lastTransmittedBlockNumber )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Invalid block number received";

    // send error packet
    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::IllegalTftpOperation,
      "Wrong block number" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );
    return;
  }

  lastReceivedBlockNumber = acknowledgementPacket.blockNumber();

  // if block number is 0 -> ACK of write without Options
  if ( acknowledgementPacket.blockNumber() == Packets::BlockNumber{ 0U } )
  {
    // If empty options is returned - Abort Operation
    if ( !configurationV.optionNegotiationHandler( {} ) )
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

  // if ACK for last data packet - QUIT
  if ( lastDataPacketTransmitted )
  {
    finished( TransferStatus::Successful );
    return;
  }

  // send data
  sendData();

  // wait for next packet
  receive();
}

void WriteOperationImpl::optionsAcknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "RX: " << static_cast< std::string>( optionsAcknowledgementPacket );

  if ( lastReceivedBlockNumber != Packets::BlockNumber{ 0xFFFFU } )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "OACK must occur after WRQ";

    Packets::ErrorPacket errorPacket{
      Packets::ErrorCode::IllegalTftpOperation,
      "OACK must occur after WRQ" };

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
    Packets::TftpOptions_getOption< uint16_t >(
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

    transmitDataSize = *blockSizeValue;

    remoteOptions.erase(
      std::string{ Packets::TftpOptions_name(
        Packets::KnownOptions::BlockSize ) } );
  }

  // Timeout Option
  const auto [ timeoutValid, timeoutValue ] =
    Packets::TftpOptions_getOption< uint8_t>(
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
    // Timout Option Response from Server must be equal to Client Value
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

    remoteOptions.erase(
      std::string{ Packets::TftpOptions_name(
        Packets::KnownOptions::Timeout ) } );
  }

  // Transfer Size Option
  const auto [ transferSizeValid, transferSizeValue ] =
    Packets::TftpOptions_getOption< uint64_t>(
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
    if ( *transferSizeValue != *transferSize )
    {
      Packets::ErrorPacket errorPacket{
        Packets::ErrorCode::TftpOptionRefused,
        "transfer size invalid" };

      send( errorPacket );

      // Operation completed
      finished(
        TransferStatus::OptionNegotiationError,
        std::move( errorPacket ) );
      return;
    }

    remoteOptions.erase(
      std::string{ Packets::TftpOptions_name(
        Packets::KnownOptions::TransferSize ) } );
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

  // send data
  sendData();

  // wait for next packet
  receive();
}

}
