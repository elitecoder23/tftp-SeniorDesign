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

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>
#include <tftp/ReceiveDataHandler.hpp>

#include <helper/Dump.hpp>

#include <utility>

namespace Tftp::Client {

ReadOperationImpl::ReadOperationImpl(
  boost::asio::io_context &ioContext,
  const uint8_t tftpTimeout,
  const uint16_t tftpRetries,
  const bool dally,
  TftpClient::ReadOperationConfiguration configuration ):
  OperationImpl{
    ioContext,
    tftpTimeout,
    tftpRetries,
    static_cast< uint16_t >( DefaultTftpDataPacketHeaderSize
      + std::max( DefaultDataSize, configuration.optionsConfiguration.blockSizeOption.get_value_or( DefaultDataSize ) ) ),
    configuration.completionHandler,
    configuration.remote,
    configuration.local },
  dally{ dally },
  configurationV{ std::move( configuration ) },
  oackReceived{ false },
  receiveDataSize{ DefaultDataSize },
  lastReceivedBlockNumber{ 0U }
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

    receiveDataSize = DefaultDataSize;
    lastReceivedBlockNumber = 0U;

    // initialise Options with additional options
    Options options{ configurationV.additionalOptions };

    // Block size Option
    if ( configurationV.optionsConfiguration.blockSizeOption )
    {
      options.emplace( Packets::TftpOptions_setOption(
        KnownOptions::BlockSize,
        *configurationV.optionsConfiguration.blockSizeOption ) );
    }

    // Timeout Option
    if ( configurationV.optionsConfiguration.timeoutOption )
    {
      options.emplace( Packets::TftpOptions_setOption(
        KnownOptions::Timeout,
        *configurationV.optionsConfiguration.timeoutOption ) );
    }

    // Add transfer size option with size '0' if requested.
    if ( configurationV.optionsConfiguration.handleTransferSizeOption )
    {
      // assure that transfer size is set to zero for read request
      options.emplace( Packets::TftpOptions_setOption(
        KnownOptions::TransferSize,
        0U ) );
    }

    // send read request packet
    sendFirst( Packets::ReadRequestPacket{
      configurationV.filename,
      configurationV.mode,
      options } );

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
      if ( dally )
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
      ErrorCode::IllegalTftpOperation,
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
      ErrorCode::IllegalTftpOperation,
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
    if ( !configurationV.optionNegotiationHandler( {} ) )
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
    if ( dally )
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
    ErrorCode::IllegalTftpOperation,
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
  if ( configurationV.optionsConfiguration.blockSizeOption )
  {
    auto [ blockSizeValid, blockSizeValue ] =
      Packets::TftpOptions_getOption< uint16_t >(
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
          ErrorCode::TftpOptionRefused,
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
  }
  remoteOptions.erase(
    std::string{ Packets::TftpOptions_name( KnownOptions::BlockSize ) } );

  // check timeout option
  if ( configurationV.optionsConfiguration.timeoutOption )
  {
    auto [ timeoutValid, timeoutValue ] =
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
      finished(
        TransferStatus::OptionNegotiationError,
        std::move( errorPacket ) );
      return;
    }

    if ( timeoutValue )
    {
      if ( *timeoutValue != *configurationV.optionsConfiguration.timeoutOption )
      {
        BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
          << "Timeout Option not equal to sent";

        Packets::ErrorPacket errorPacket{
          ErrorCode::TftpOptionRefused,
          "Timeout Option negotiation failed" };

        send( errorPacket);

        // Operation completed
        finished(
          TransferStatus::OptionNegotiationError,
          std::move( errorPacket ) );
        return;
      }

      receiveTimeout( *timeoutValue );
    }
  }
  remoteOptions.erase(
    std::string{ Packets::TftpOptions_name( KnownOptions::Timeout ) } );

  // check transfer size option
  if ( configurationV.optionsConfiguration.handleTransferSizeOption )
  {
    auto [ transferSizeValid, transferSizeValue ] =
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
          ErrorCode::DiskFullOrAllocationExceeds,
          "File to big" };

        send( errorPacket );

        // Operation completed
        finished(
          TransferStatus::OptionNegotiationError,
          std::move( errorPacket ) );
        return;
      }
    }
  }
  remoteOptions.erase(
    std::string{ Packets::TftpOptions_name( KnownOptions::TransferSize ) } );

  // Perform additional Option Negotiation
  if ( !configurationV.optionNegotiationHandler( remoteOptions ) )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Option negotiation failed";

    Packets::ErrorPacket errorPacket{
      ErrorCode::TftpOptionRefused,
      "Option negotiation failed" };

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
