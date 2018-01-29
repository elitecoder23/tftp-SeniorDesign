/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class Tftp::Client::WriteRequestOperationImpl.
 **/

#include "WriteRequestOperationImpl.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>
#include <tftp/TransmitDataHandler.hpp>
#include <tftp/packets/PacketFactory.hpp>

#include <helper/Dump.hpp>

namespace Tftp {
namespace Client {

WriteRequestOperationImpl::WriteRequestOperationImpl(
  boost::asio::io_service &ioService,
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const TftpClientInternal &tftpClient,
  const UdpAddressType &remote,
  const std::string &filename,
  const TransferMode mode,
  const UdpAddressType &local):
  OperationImpl(
    ioService,
    completionHandler,
    tftpClient,
    remote,
    filename,
    mode,
    local),
  dataHandler( dataHandler),
  transmitDataSize( DefaultDataSize),
  lastDataPacketTransmitted( false),
  lastTransmittedBlockNumber( 0)
{
}

void WriteRequestOperationImpl::start()
{
  BOOST_LOG_FUNCTION();

  try
  {
    transmitDataSize = DefaultDataSize;
    lastDataPacketTransmitted = false;
    lastTransmittedBlockNumber = 0;

    // if Transfer Size option is set, query them from the handler
    if (options().hasTransferSizeOption())
    {
      uint64_t transferSize;

      // If the handler supplies a transfer size
      if (dataHandler->requestedTransferSize( transferSize))
      {
        // set transfer size TFTP option
        options().addTransferSizeOption( transferSize);
      }
      else
      {
        // otherwise remove this option
        options().removeTransferSizeOption();
      }
    }

    // send write request packet
    sendFirst( Packets::WriteRequestPacket(
      filename(),
      mode(),
      options()));

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
  BOOST_LOG_FUNCTION();

  lastTransmittedBlockNumber++;

  Packets::DataPacket data(
    lastTransmittedBlockNumber,
    dataHandler->sendData( transmitDataSize));

  if (data.dataSize() < transmitDataSize)
  {
    lastDataPacketTransmitted = true;
  }

  // send packet
  send( data);
}

void WriteRequestOperationImpl::handleDataPacket(
  const UdpAddressType &,
  const Packets::DataPacket &dataPacket)
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX ERROR: " <<
    static_cast< std::string>( dataPacket);

  send( Packets::ErrorPacket(
    ErrorCode::IllegalTftpOperation,
    "DATA not expected"));

  // Operation completed
  finished( TransferStatus::TransferError);
}

void WriteRequestOperationImpl::handleAcknowledgementPacket(
  const UdpAddressType &,
  const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX: " <<
    static_cast< std::string>( acknowledgementPacket);

  // check retransmission
  if (acknowledgementPacket.blockNumber() == lastTransmittedBlockNumber.previous())
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
      "Received previous ACK packet: retry of last data package - "
      "IGNORE it due to Sorcerer's Apprentice Syndrome";

    return;
  }

  // check invalid block number
  if (acknowledgementPacket.blockNumber() != lastTransmittedBlockNumber)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Invalid block number received";

    send( Packets::ErrorPacket(
      ErrorCode::IllegalTftpOperation,
      "Wrong block number"));

    finished( TransferStatus::TransferError);
    return;
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

void WriteRequestOperationImpl::handleOptionsAcknowledgementPacket(
  const UdpAddressType &,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket)
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX: " <<
    static_cast< std::string>( optionsAcknowledgementPacket);

  const auto &remoteOptions( optionsAcknowledgementPacket.options());

  // check empty options
  if (remoteOptions.empty())
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Received option list is empty";

    Packets::ErrorPacket errorPacket(
      ErrorCode::IllegalTftpOperation,
      "Empty OACK not allowed");

    send( errorPacket);

    finished( TransferStatus::TransferError, std::move( errorPacket));
    return;
  }

  // perform option negotiation
  const auto negotiatedOptions{ options().negotiateClient( remoteOptions)};

  if (negotiatedOptions.empty())
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Option negotiation failed";

    send( Packets::ErrorPacket(
      ErrorCode::TftpOptionRefused,
      "Option negotiation failed"));

    finished( TransferStatus::OptionNegotiationError);
    return;
  }

  // check blocksize option
  if (0 != negotiatedOptions.getBlocksizeOption())
  {
    transmitDataSize = negotiatedOptions.getBlocksizeOption();
  }

  // check timeout option
  if (0 != negotiatedOptions.getTimeoutOption())
  {
    receiveTimeout( negotiatedOptions.getTimeoutOption());
  }

  // Transfer size option is not checked here (already performed during option negotiation)

  // send data
  sendData();

  // wait for next packet
  receive();
}

}
}
