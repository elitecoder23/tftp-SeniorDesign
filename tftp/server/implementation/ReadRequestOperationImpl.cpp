/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class Tftp::Server::ReadRequestOperationImpl.
 **/

#include "ReadRequestOperationImpl.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>
#include <tftp/TransmitDataHandler.hpp>
#include <tftp/TftpConfiguration.hpp>

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/ErrorPacket.hpp>

namespace Tftp::Server {

ReadRequestOperationImpl::ReadRequestOperationImpl(
  boost::asio::io_service &ioService,
  const TftpServerInternal &tftpServer,
  TransmitDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const boost::asio::ip::udp::endpoint &remote,
  const Options::OptionList &clientOptions,
  const boost::asio::ip::udp::endpoint &local) :
  OperationImpl(
    ioService,
    tftpServer,
    completionHandler,
    remote,
    clientOptions,
    local),
  dataHandler( dataHandler),
  transmitDataSize( DefaultDataSize),
  lastDataPacketTransmitted( false),
  lastTransmittedBlockNumber( 0)
{
}

void ReadRequestOperationImpl::start()
{
  try
  {
    auto respOptions{ options()};

    // option negotiation leads to empty option list
    if ( respOptions.empty())
    {
      sendData();
    }
    else
    {
      // check blocksize option
      if ( auto blocksize{ respOptions.blocksize()}; blocksize)
      {
        transmitDataSize = *blocksize;
      }

      // check timeout option
      if ( auto timeoutOption{ respOptions.timeoutOption()}; timeoutOption)
      {
        receiveTimeout( *timeoutOption);
      }

      // check transfer size option
      if ( auto transferSizeOption{ respOptions.transferSizeOption()})
      {
        if ( 0U != *transferSizeOption)
        {
          BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
            "Received transfer size must be 0";

          Packets::ErrorPacket errorPacket(
            ErrorCode::TftpOptionRefused,
            "transfer size must be 0");
          send( errorPacket);

          // Operation completed
          finished( TransferStatus::TransferError, std::move( errorPacket));

          return;
        }

        // add transfer size to answer only, if handler supply it.
        if ( auto transferSize{ dataHandler->requestedTransferSize()}; transferSize)
        {
          respOptions.transferSizeOption( *transferSize);
        }
        else
        {
          respOptions.removeTransferSizeOption();
        }
      }

      // if transfer size option is the only option requested, but the handler
      // does not supply it -> empty OACK is not sent but data directly
      if ( !options().empty())
      {
        // Send OACK
        send( Packets::OptionsAcknowledgementPacket( options()));
      }
      else
      {
        // directly send data
        sendData();
      }
    }

    // start receive loop
    OperationImpl::start();
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
  BOOST_LOG_FUNCTION();

  OperationImpl::finished( status, std::move( errorInfo));
  dataHandler->finished();
}

void ReadRequestOperationImpl::sendData()
{
  BOOST_LOG_FUNCTION();

  lastTransmittedBlockNumber++;

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
    "Send Data: " << lastTransmittedBlockNumber;

  Packets::DataPacket data(
    lastTransmittedBlockNumber,
    dataHandler->sendData( transmitDataSize));

  if ( data.dataSize() < transmitDataSize)
  {
    lastDataPacketTransmitted = true;
  }

  // send data
  send( data);
}

void ReadRequestOperationImpl::handleDataPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::DataPacket &dataPacket)
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) << "RX ERROR: " <<
    static_cast< std::string>( dataPacket);

  Packets::ErrorPacket errorPacket(
    ErrorCode::IllegalTftpOperation,
    "DATA not expected");

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket));
}

void ReadRequestOperationImpl::handleAcknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
    "RX: " << static_cast< std::string>( acknowledgementPacket);

  // check retransmission
  if (acknowledgementPacket.blockNumber() == lastTransmittedBlockNumber.previous())
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
      "Received previous ACK packet: retry of last data package - "
      "IGNORE it due to Sorcerer's Apprentice Syndrome";

    // receive next packet
    receive();

    return;
  }

  // check invalid block number
  if (acknowledgementPacket.blockNumber() != lastTransmittedBlockNumber)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Invalid block number received";

    Packets::ErrorPacket errorPacket(
      ErrorCode::IllegalTftpOperation,
      "Block number not expected");

    send( errorPacket);

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket));

    return;
  }

  // if it was the last ACK of the last data packet - we are finished.
  if (lastDataPacketTransmitted)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
      "Last acknowledgement received";

    finished( TransferStatus::Successful);

    return;
  }

  // send data
  sendData();

  // receive next packet
  receive();
}

}
