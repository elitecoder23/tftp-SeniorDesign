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

namespace Tftp::Client {

WriteRequestOperationImpl::WriteRequestOperationImpl(
  boost::asio::io_service &ioService,
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const TftpClientInternal &tftpClient,
  const boost::asio::ip::udp::endpoint &remote,
  const std::string &filename,
  const TransferMode mode,
  const boost::asio::ip::udp::endpoint &local):
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
  lastTransmittedBlockNumber( 0U)
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

    Options::OptionList reqOptions{ options()};

    // Add transfer size option with size '0' if requested.
    if (configuration().handleTransferSizeOption)
    {
      // If the handler supplies a transfer size
      if ( auto transferSize{ dataHandler->requestedTransferSize()}; transferSize)
      {
        // set transfer size TFTP option
        reqOptions.transferSizeOption( *transferSize);
      }
    }

    // send write request packet
    sendFirst( Packets::WriteRequestPacket(
      filename(),
      mode(),
      reqOptions));

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
  const boost::asio::ip::udp::endpoint &,
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
  const boost::asio::ip::udp::endpoint &,
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
  const boost::asio::ip::udp::endpoint &,
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
  if (auto blocksize{ negotiatedOptions.blocksize()}; blocksize)
  {
    transmitDataSize = *blocksize;
  }

  // check timeout option
  if ( auto timeoutOption{ negotiatedOptions.timeoutOption()}; timeoutOption)
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
