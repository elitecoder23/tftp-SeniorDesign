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
  OptionNegotiationHandler optionNegotiationHandler,
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const TftpClientInternal &tftpClient,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const TransferMode mode,
  const Options::OptionList &clientOptions):
  OperationImpl{
    ioContext,
    completionHandler,
    tftpClient,
    remote},
  optionNegotiationHandler{optionNegotiationHandler},
  dataHandler{ dataHandler},
  filename{ filename},
  mode{ mode},
  clientOptions{ clientOptions},
  transmitDataSize{ DefaultDataSize},
  lastDataPacketTransmitted{ false},
  lastTransmittedBlockNumber{ 0U}
{
}

WriteRequestOperationImpl::WriteRequestOperationImpl(
  boost::asio::io_context &ioContext,
  OptionNegotiationHandler optionNegotiationHandler,
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const TftpClientInternal &tftpClient,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const TransferMode mode,
  const Options::OptionList &clientOptions,
  const boost::asio::ip::udp::endpoint &local):
  OperationImpl{
    ioContext,
    completionHandler,
    tftpClient,
    remote,
    local},
  optionNegotiationHandler{optionNegotiationHandler},
  dataHandler{ dataHandler},
  filename{ filename},
  mode{ mode},
  clientOptions{ clientOptions},
  transmitDataSize{ DefaultDataSize},
  lastDataPacketTransmitted{ false},
  lastTransmittedBlockNumber{ 0U}
{
}

void WriteRequestOperationImpl::start()
{
  BOOST_LOG_FUNCTION()

  try
  {
    transmitDataSize = DefaultDataSize;
    lastDataPacketTransmitted = false;
    lastTransmittedBlockNumber = 0;

    // Add transfer size option with size '0' if requested.
    if (configuration().handleTransferSizeOption)
    {
      // If the handler supplies a transfer size
      if ( auto transferSize{ dataHandler->requestedTransferSize()}; transferSize)
      {
        // set transfer size TFTP option
        clientOptions.transferSizeOption( *transferSize);
      }
    }

    // send write request packet
    sendFirst( Packets::WriteRequestPacket{
      filename,
      mode,
      clientOptions.options()});

    // wait for answers
    OperationImpl::start();
  }
  catch (...)
  {
    finished( TransferStatus::CommunicationError);
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
    dataHandler->sendData( transmitDataSize)};

  if (data.dataSize() < transmitDataSize)
  {
    lastDataPacketTransmitted = true;
  }

  // send packet
  send( data);
}

void WriteRequestOperationImpl::dataPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::DataPacket &dataPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
    << "RX ERROR: " << static_cast< std::string>( dataPacket);

  Packets::ErrorPacket errorPacket{
    ErrorCode::IllegalTftpOperation,
    "DATA not expected"};

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket));
}

void WriteRequestOperationImpl::acknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
    << "RX: " << static_cast< std::string>( acknowledgementPacket);

  // check retransmission
  if (acknowledgementPacket.blockNumber() == lastTransmittedBlockNumber.previous())
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
      << "Received previous ACK packet: retry of last data package - "
         "IGNORE it due to Sorcerer's Apprentice Syndrome";

    return;
  }

  // check invalid block number
  if (acknowledgementPacket.blockNumber() != lastTransmittedBlockNumber)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
      << "Invalid block number received";

    Packets::ErrorPacket errorPacket{
      ErrorCode::IllegalTftpOperation,
      "Wrong block number"};

    send( errorPacket);

    finished( TransferStatus::TransferError, std::move( errorPacket));
    return;
  }

  // if blocknumber is 0 -> ACK of write without Options
  if ( acknowledgementPacket.blockNumber() == static_cast< uint16_t >( 0U))
  {
    auto negotiatedOptions{ optionNegotiationHandler( {})};

    // If empty options is returned - Abort Operation
    if (!negotiatedOptions)
    {
      BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
        << "Option Negotiation failed";

      Packets::ErrorPacket errorPacket{
        ErrorCode::TftpOptionRefused,
        "Option Negotiation Failed"};

      send( errorPacket);

      finished( TransferStatus::TransferError, std::move( errorPacket));
      return;
    }
  }

  // if ACK for last data packet - QUIT
  if (lastDataPacketTransmitted)
  {
    finished( TransferStatus::Successful);
    return;
  }

  // send data
  sendData();

  // wait for next packet
  receive();
}

void WriteRequestOperationImpl::optionsAcknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
    << "RX: " << static_cast< std::string>( optionsAcknowledgementPacket);

  const auto &remoteOptions{ optionsAcknowledgementPacket.options()};

  // check empty options
  if (remoteOptions.empty())
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
      << "Received option list is empty";

    Packets::ErrorPacket errorPacket{
      ErrorCode::IllegalTftpOperation,
      "Empty OACK not allowed"};

    send( errorPacket);

    finished( TransferStatus::TransferError, std::move( errorPacket));
    return;
  }

  // perform option negotiation
  const auto negotiatedOptions{ optionNegotiationHandler( remoteOptions)};

  if ( !negotiatedOptions)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
      << "Option negotiation failed";

    Packets::ErrorPacket errorPacket{
      ErrorCode::TftpOptionRefused,
      "Option negotiation failed"};

    send( errorPacket);

    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket));
    return;
  }

  // check blocksize option
  if ( auto blocksize{ negotiatedOptions->blocksize()}; blocksize)
  {
    transmitDataSize = *blocksize;
  }

  // check timeout option
  if ( auto timeoutOption{ negotiatedOptions->timeoutOption()}; timeoutOption)
  {
    receiveTimeout( *timeoutOption);
  }

  // Transfer size option is not checked here (already performed during option negotiation)

  // send data
  sendData();

  // wait for next packet
  receive();
}

}
