/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Client::ReadRequestOperationImpl.
 **/

#include "ReadRequestOperationImpl.hpp"

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>
#include <tftp/ReceiveDataHandler.hpp>

#include <helper/Dump.hpp>

namespace Tftp::Client {

ReadRequestOperationImpl::ReadRequestOperationImpl(
  boost::asio::io_context &ioContext,
  const uint8_t tftpTimeout,
  const uint16_t tftpRetries,
  OptionNegotiationHandler optionNegotiationHandler,
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const TransferMode mode,
  const TftpOptionsConfiguration &optionsConfiguration,
  const Options &additionalOptions ) :
  OperationImpl{
    ioContext,
    tftpTimeout,
    tftpRetries,
    completionHandler,
    remote},
  optionNegotiationHandler{ optionNegotiationHandler},
  dataHandler{ dataHandler},
  filename{ filename},
  mode{ mode},
  optionsConfiguration{ optionsConfiguration },
  additionalOptions{ additionalOptions },
  oackReceived{ false},
  receiveDataSize{ DefaultDataSize},
  lastReceivedBlockNumber{ 0U}
{
  BOOST_LOG_FUNCTION()

  if ( !optionNegotiationHandler || !dataHandler || !completionHandler)
  {
    BOOST_THROW_EXCEPTION( TftpException()
      << Helper::AdditionalInfo( "Parameter Invalid")
      << TransferPhaseInfo( TransferPhase::Initialisation ) );
  }
}

ReadRequestOperationImpl::ReadRequestOperationImpl(
  boost::asio::io_context &ioContext,
  const uint8_t tftpTimeout,
  const uint16_t tftpRetries,
  OptionNegotiationHandler optionNegotiationHandler,
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const TransferMode mode,
  const TftpOptionsConfiguration &optionsConfiguration,
  const Options &additionalOptions,
  const boost::asio::ip::udp::endpoint &local) :
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
  oackReceived{ false},
  receiveDataSize{ DefaultDataSize},
  lastReceivedBlockNumber{ 0U}
{
  BOOST_LOG_FUNCTION()

  if ( !optionNegotiationHandler || !dataHandler || !completionHandler)
  {
    BOOST_THROW_EXCEPTION( TftpException()
      << Helper::AdditionalInfo( "Parameter Invalid")
      << TransferPhaseInfo( TransferPhase::Initialisation));
  }
}

void ReadRequestOperationImpl::request()
{
  BOOST_LOG_FUNCTION()

  try
  {
    // Reset data handler
    dataHandler->reset();

    receiveDataSize = DefaultDataSize;
    lastReceivedBlockNumber = 0U;

    // initialise Options with additional options
    Options options{ additionalOptions };

    // Block size Option
    if ( optionsConfiguration.blockSizeOption )
    {
      options.emplace( blockSizeOption( *optionsConfiguration.blockSizeOption ) );
    }

    // timeout Option
    if ( optionsConfiguration.timeoutOption )
    {
      options.emplace( timeoutOption( *optionsConfiguration.timeoutOption ) );
    }

    // Add transfer size option with size '0' if requested.
    if ( optionsConfiguration.handleTransferSizeOption )
    {
      // assure that transfer size is set to zero for read request
      options.emplace( transferSizeOption( 0U ) );
    }

    // send read request packet
    sendFirst(
      Packets::ReadRequestPacket{ filename, mode, options } );

    // wait for answers
    receiveFirst();
  }
  catch ( ...)
  {
    finished( TransferStatus::CommunicationError);
  }
}

void ReadRequestOperationImpl::finished(
  const TransferStatus status,
  ErrorInfo &&errorInfo) noexcept
{
  // inform base class
  OperationImpl::finished( status, std::move( errorInfo));

  // Inform data handler
  dataHandler->finished();
}

void ReadRequestOperationImpl::dataPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::DataPacket &dataPacket)
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
      "Block Number not expected"};
    send( errorPacket );

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ));
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
    if (!optionNegotiationHandler( {} ) )
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

  // call call-back
  dataHandler->receivedData( dataPacket.data() );

  // increment received block number
  lastReceivedBlockNumber++;

  // send ACK
  send( Packets::AcknowledgementPacket( lastReceivedBlockNumber ) );

  // if received data size is smaller then the expected
  if ( dataPacket.dataSize() < receiveDataSize )
  {
    // last packet has been received and operation is finished
    finished( TransferStatus::Successful );
  }
  else
  {
    // otherwise wait for next data package
    receive();
  }
}

void ReadRequestOperationImpl::acknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "RX ERROR: " << static_cast< std::string>( acknowledgementPacket );

  // send Error
  Packets::ErrorPacket errorPacket{
    ErrorCode::IllegalTftpOperation,
    "ACK not expected" };

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket ) );
}

void ReadRequestOperationImpl::optionsAcknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket )
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::info )
    << "RX ERROR: " << static_cast< std::string>( optionsAcknowledgementPacket );

  // @todo check OACK comes directly after RRQ (last received blockNumber)

  const auto &remoteOptions{ optionsAcknowledgementPacket.options() };

  // check empty options
  if ( remoteOptions.empty() )
  {
    BOOST_LOG_SEV( TftpLogger::get(), Helper::Severity::error )
      << "Received option list is empty";

    Packets::ErrorPacket errorPacket{
      ErrorCode::IllegalTftpOperation,
      "Empty OACK not allowed" };

    send( errorPacket);

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket ) );
    return;
  }

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
      receiveDataSize = *blockSizeValue;

      // set maximum receive data size if necessary
      if ( receiveDataSize > DefaultDataSize)
      {
        maxReceivePacketSize(
          receiveDataSize + DefaultTftpDataPacketHeaderSize );
      }
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
      if ( !dataHandler->receivedTransferSize( *transferSizeValue ) )
      {
        Packets::ErrorPacket errorPacket{
          ErrorCode::DiskFullOrAllocationExceeds,
          "File to big" };

        send( errorPacket);

        // Operation completed
        finished( TransferStatus::TransferError, std::move( errorPacket ) );
        return;
      }
    }
  }

  // indicate Options acknowledgement
  oackReceived = true;

  // send Acknowledgment with block number set to 0
  send( Packets::AcknowledgementPacket{ Packets::BlockNumber{ 0 } } );

  // receive next packet
  receive();
}

}
