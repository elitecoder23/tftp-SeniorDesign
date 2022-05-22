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

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>
#include <tftp/TransmitDataHandler.hpp>

#include <helper/Dump.hpp>

namespace Tftp::Client {

WriteOperationImpl::WriteOperationImpl(
  boost::asio::io_context &ioContext,
  const uint8_t tftpTimeout,
  const uint16_t tftpRetries,
  OptionNegotiationHandler optionNegotiationHandler,
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const TransferMode mode,
  const TftpOptionsConfiguration &optionsConfiguration,
  const Options &additionalOptions ):
  OperationImpl{
    ioContext,
    tftpTimeout,
    tftpRetries,
    DefaultMaxPacketSize,
    completionHandler,
    remote },
  optionNegotiationHandler{ optionNegotiationHandler },
  dataHandler{ dataHandler },
  filename{ filename },
  mode{ mode },
  optionsConfiguration{ optionsConfiguration },
  additionalOptions{ additionalOptions },
  transmitDataSize{ DefaultDataSize },
  lastDataPacketTransmitted{ false },
  lastTransmittedBlockNumber{ 0U },
  lastReceivedBlockNumber{ 0U }
{
  BOOST_LOG_FUNCTION()

  if ( !optionNegotiationHandler || !dataHandler || !completionHandler )
  {
    BOOST_THROW_EXCEPTION( TftpException()
      << Helper::AdditionalInfo{ "Parameter Invalid" }
      << TransferPhaseInfo{ TransferPhase::Initialisation } );
  }
}

WriteOperationImpl::WriteOperationImpl(
  boost::asio::io_context &ioContext,
  const uint8_t tftpTimeout,
  const uint16_t tftpRetries,
  OptionNegotiationHandler optionNegotiationHandler,
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const TransferMode mode,
  const TftpOptionsConfiguration &optionsConfiguration,
  const Options &additionalOptions,
  const boost::asio::ip::udp::endpoint &local ):
  OperationImpl{
    ioContext,
    tftpTimeout,
    tftpRetries,
    DefaultMaxPacketSize,
    completionHandler,
    remote,
    local},
  optionNegotiationHandler{ optionNegotiationHandler },
  dataHandler{ dataHandler },
  filename{ filename },
  mode{ mode },
  optionsConfiguration{ optionsConfiguration },
  additionalOptions{ additionalOptions },
  transmitDataSize{ DefaultDataSize },
  lastDataPacketTransmitted{ false },
  lastTransmittedBlockNumber{ 0U },
  lastReceivedBlockNumber{ 0U }
{
  BOOST_LOG_FUNCTION()

  if ( !optionNegotiationHandler || !dataHandler || !completionHandler )
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
    dataHandler->reset();

    transmitDataSize = DefaultDataSize;
    lastDataPacketTransmitted = false;
    lastTransmittedBlockNumber = 0U;
    lastReceivedBlockNumber = 0xFFFFU;

    // initialise Options with additional options
    Options options{ additionalOptions };

    // Block size Option
    if ( optionsConfiguration.blockSizeOption )
    {
      options.emplace( Packets::TftpOptions_setOption(
        KnownOptions::BlockSize,
        *optionsConfiguration.blockSizeOption ) );
    }

    // Timeout Option
    if ( optionsConfiguration.timeoutOption )
    {
      options.emplace( Packets::TftpOptions_setOption(
        KnownOptions::Timeout,
        *optionsConfiguration.timeoutOption ) );
    }

    // Add transfer size option if requested.
    if ( optionsConfiguration.handleTransferSizeOption )
    {
      // If the handler supplies a transfer size
      if ( transferSize = dataHandler->requestedTransferSize(); transferSize )
      {
        // set transfer size TFTP option
        options.emplace( Packets::TftpOptions_setOption(
          KnownOptions::Timeout,
          *transferSize ) );
      }
    }

    // send write request packet
    sendFirst(
      Packets::WriteRequestPacket{ filename, mode, options } );

    // wait for answers
    receiveFirst();
  }
  catch ( boost::exception &e )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Exception during request " << boost::diagnostic_information( e );

    finished( TransferStatus::CommunicationError );
  }
}

void WriteOperationImpl::finished(
  const TransferStatus status,
  ErrorInfo &&errorInfo) noexcept
{
  // inform base class
  OperationImpl::finished( status, std::move( errorInfo));

  // Inform data handler
  dataHandler->finished();
}

void WriteOperationImpl::sendData()
{
  BOOST_LOG_FUNCTION()

  lastTransmittedBlockNumber++;

  Packets::DataPacket data{
    lastTransmittedBlockNumber,
    dataHandler->sendData( transmitDataSize ) };

  if ( data.dataSize() < transmitDataSize )
  {
    lastDataPacketTransmitted = true;
  }

  // send packet
  send( data );
}

void WriteOperationImpl::dataPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::DataPacket &dataPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "RX ERROR: " << static_cast< std::string>( dataPacket );

  Packets::ErrorPacket errorPacket{
    ErrorCode::IllegalTftpOperation,
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
      ErrorCode::IllegalTftpOperation,
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
    if ( !optionNegotiationHandler( {} ) )
    {
      BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
        << "Option Negotiation failed";

      Packets::ErrorPacket errorPacket{
        ErrorCode::TftpOptionRefused,
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
      << "OACK Out of order";

    Packets::ErrorPacket errorPacket{
      ErrorCode::IllegalTftpOperation,
      "OACK not allowed here" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );
    return;
  }

  auto remoteOptions{ optionsAcknowledgementPacket.options() };

  // check empty options
  if ( remoteOptions.empty() )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Received option list is empty";

    Packets::ErrorPacket errorPacket{
      ErrorCode::IllegalTftpOperation,
      "Empty OACK not allowed" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );
    return;
  }

  // check block size option
  if ( optionsConfiguration.blockSizeOption )
  {
    auto [blockSizeValid, blockSizeValue] =
      Packets::TftpOptions_getOption< uint16_t>(
        remoteOptions,
        KnownOptions::BlockSize );

    if ( !blockSizeValid )
    {
      BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
        << "Block Size Option negotiation failed";

      Packets::ErrorPacket errorPacket{
        ErrorCode::TftpOptionRefused,
        "Block size Option negotiation failed" };

      send( errorPacket );

      // Operation completed
      finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
      return;
    }

    if ( blockSizeValue )
    {
      if ( *blockSizeValue > *optionsConfiguration.blockSizeOption )
      {
        BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
          << "Received Block Size Option bigger than negotiated";

        Packets::ErrorPacket errorPacket{
          ErrorCode::TftpOptionRefused,
          "Block size Option negotiation failed" };

        send( errorPacket);

        // Operation completed
        finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
        return;
      }

      transmitDataSize = *blockSizeValue;
    }
  }
  remoteOptions.erase( std::string{ Packets::TftpOptions_name( KnownOptions::BlockSize ) } );

  // check timeout option
  if ( optionsConfiguration.timeoutOption )
  {
    auto [timeoutValid, timeoutValue] =
      Packets::TftpOptions_getOption< uint8_t>(
        remoteOptions,
        KnownOptions::Timeout );

    if ( !timeoutValid )
    {
      BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
        << "Timeout Option negotiation failed";

      Packets::ErrorPacket errorPacket{
        ErrorCode::TftpOptionRefused,
        "Timeout Option negotiation failed" };

      send( errorPacket);

      // Operation completed
      finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
      return;
    }

    if ( timeoutValue )
    {
      if ( *timeoutValue != *optionsConfiguration.timeoutOption )
      {
        BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
          << "Timeout Option not equal to sent";

        Packets::ErrorPacket errorPacket{
          ErrorCode::TftpOptionRefused,
          "Timeout Option negotiation failed" };

        send( errorPacket);

        // Operation completed
        finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
        return;
      }

      receiveTimeout( *timeoutValue );
    }
  }
  remoteOptions.erase( std::string{ Packets::TftpOptions_name( KnownOptions::Timeout ) } );

  // check transfer size option
  if ( optionsConfiguration.handleTransferSizeOption )
  {
    auto [transferSizeValid, transferSizeValue] =
      Packets::TftpOptions_getOption< uint64_t>(
        remoteOptions,
        KnownOptions::TransferSize );

    if ( !transferSizeValid )
    {
      BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
        << "Transfer Size Option negotiation failed";

      Packets::ErrorPacket errorPacket{
        ErrorCode::TftpOptionRefused,
        "Transfer Size Option negotiation failed" };

      send( errorPacket);

      // Operation completed
      finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
      return;
    }

    if ( transferSizeValue )
    {
      if ( *transferSizeValue != *transferSize )
      {
        Packets::ErrorPacket errorPacket{
          ErrorCode::TftpOptionRefused,
          "transfer size invalid" };

        send( errorPacket );

        // Operation completed
        finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
        return;
      }
    }
  }
  remoteOptions.erase( std::string{ Packets::TftpOptions_name( KnownOptions::TransferSize ) } );

  // Perform additional Option Negotiation
  if ( !optionNegotiationHandler( remoteOptions ) )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Option negotiation failed";

    Packets::ErrorPacket errorPacket{
      ErrorCode::TftpOptionRefused,
      "Option negotiation failed" };

    send( errorPacket );

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  // send data
  sendData();

  // wait for next packet
  receive();
}

}
