/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Client::WriteRequestOperationImpl.
 **/

#include "WriteRequestOperationImpl.hpp"

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/WriteRequestPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>
#include <tftp/TransmitDataHandler.hpp>

#include <helper/Dump.hpp>

namespace Tftp::Client {

WriteRequestOperationImpl::WriteRequestOperationImpl(
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
    completionHandler,
    remote},
  optionNegotiationHandler{optionNegotiationHandler},
  dataHandler{ dataHandler},
  filename{ filename},
  mode{ mode},
  optionsConfiguration{ optionsConfiguration },
  additionalOptions{ additionalOptions },
  transmitDataSize{ DefaultDataSize},
  lastDataPacketTransmitted{ false},
  lastTransmittedBlockNumber{ 0U},
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

WriteRequestOperationImpl::WriteRequestOperationImpl(
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
  const boost::asio::ip::udp::endpoint &local):
  OperationImpl{
    ioContext,
    tftpTimeout,
    tftpRetries,
    completionHandler,
    remote,
    local},
  optionNegotiationHandler{optionNegotiationHandler},
  dataHandler{ dataHandler},
  filename{ filename},
  mode{ mode},
  optionsConfiguration{ optionsConfiguration },
  additionalOptions{ additionalOptions },
  transmitDataSize{ DefaultDataSize},
  lastDataPacketTransmitted{ false},
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

void WriteRequestOperationImpl::request()
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
      options.emplace( blockSizeOption( *optionsConfiguration.blockSizeOption ) );
    }

    // Timeout Option
    if ( optionsConfiguration.timeoutOption )
    {
      options.emplace( timeoutOption( *optionsConfiguration.timeoutOption ) );
    }

    // Add transfer size option if requested.
    if ( optionsConfiguration.handleTransferSizeOption )
    {
      // If the handler supplies a transfer size
      if ( transferSize = dataHandler->requestedTransferSize(); transferSize )
      {
        // set transfer size TFTP option
        options.emplace( transferSizeOption( *transferSize ) );
      }
    }

    // send write request packet
    sendFirst( Packets::WriteRequestPacket{
      filename,
      mode,
      options } );

    // wait for answers
    receiveFirst();
  }
  catch (...)
  {
    finished( TransferStatus::CommunicationError );
  }
}

void WriteRequestOperationImpl::finished(
  const TransferStatus status,
  ErrorInfo &&errorInfo) noexcept
{
  OperationImpl::finished( status, std::move( errorInfo));
  dataHandler->finished();
}

void WriteRequestOperationImpl::sendData()
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

void WriteRequestOperationImpl::dataPacket(
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

void WriteRequestOperationImpl::acknowledgementPacket(
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
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "Invalid block number received";

    Packets::ErrorPacket errorPacket{
      ErrorCode::IllegalTftpOperation,
      "Wrong block number"};

    send( errorPacket);

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

void WriteRequestOperationImpl::optionsAcknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "RX: " << static_cast< std::string>( optionsAcknowledgementPacket );

  //! @todo check OACK has been received direct after WRQ (lastRxBlocknumber)

  const auto &remoteOptions{ optionsAcknowledgementPacket.options() };

  // check empty options
  if ( remoteOptions.empty() )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Received option list is empty";

    Packets::ErrorPacket errorPacket{
      ErrorCode::IllegalTftpOperation,
      "Empty OACK not allowed" };

    send( errorPacket );

    finished( TransferStatus::TransferError, std::move( errorPacket ) );
    return;
  }

  // Perform additional Option Negotiation
  if ( !optionNegotiationHandler( remoteOptions ) )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error)
      << "Option negotiation failed";

    Packets::ErrorPacket errorPacket{
      ErrorCode::TftpOptionRefused,
      "Option negotiation failed" };

    send( errorPacket );

    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
    return;
  }

  // check block size option
  if ( optionsConfiguration.blockSizeOption )
  {
    auto [blockSizeValid, blockSizeValue] = blockSizeOption(
      remoteOptions,
      *optionsConfiguration.blockSizeOption );

    if ( !blockSizeValid )
    {
      BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
        << "Block Size Option negotiation failed";

      Packets::ErrorPacket errorPacket{
        ErrorCode::TftpOptionRefused,
        "Block size Option negotiation failed" };

      send( errorPacket);

      // Operation completed
      finished( TransferStatus::OptionNegotiationError, std::move( errorPacket ) );
      return;
    }

    if ( blockSizeValue )
    {
      transmitDataSize = *blockSizeValue;
    }
  }

  // check timeout option
  if ( optionsConfiguration.timeoutOption )
  {
    auto [timeoutValid, timeoutValue] = timeoutOption(
      remoteOptions,
      *optionsConfiguration.timeoutOption );

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
      receiveTimeout( *timeoutValue );
    }
  }

  // check transfer size option
  if ( optionsConfiguration.handleTransferSizeOption )
  {
    auto [transferSizeValid, transferSizeValue] = transferSizeOption(
      remoteOptions );

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
      if ( *transferSizeValue != * transferSize )
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

  // send data
  sendData();

  // wait for next packet
  receive();
}

}
