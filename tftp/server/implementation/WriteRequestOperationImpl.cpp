/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, Thomas@Thomas-Vogt.de
 *
 * @brief Definition of class Tftp::Server::WriteRequestOperationImpl.
 **/

#include "WriteRequestOperationImpl.hpp"

#include <tftp/TftpLogger.hpp>
#include <tftp/TftpException.hpp>
#include <tftp/ReceiveDataHandler.hpp>

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/ErrorPacket.hpp>

namespace Tftp::Server {

WriteRequestOperationImpl::WriteRequestOperationImpl(
  boost::asio::io_service &ioService,
  const TftpServerInternal &tftpServer,
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const UdpAddressType &remote,
  const Options::OptionList &clientOptions,
  const UdpAddressType &local) :
  OperationImpl(
    ioService,
    tftpServer,
    completionHandler,
    remote,
    clientOptions,
    local),
  dataHandler( dataHandler),
  receiveDataSize( DefaultDataSize),
  lastReceivedBlockNumber( 0)
{
}

void WriteRequestOperationImpl::start()
{
  BOOST_LOG_FUNCTION();

  try
  {
    auto respOptions{ options()};

    // option negotiation leads to empty option list
    if (respOptions.empty())
    {
      // Then no NOACK is sent back - a simple ACK is sent.
      send( Packets::AcknowledgementPacket( Packets::BlockNumber{ 0U}));
    }
    else
    {
      //validate received options

      // check blocksize option
      if ( auto blocksize{ respOptions.blocksize()}; blocksize)
      {
        receiveDataSize = *blocksize;

        // set receive data size if necessary
        if ( receiveDataSize > DefaultDataSize)
        {
          maxReceivePacketSize(
            receiveDataSize + DefaultTftpDataPacketHeaderSize);
        }
      }

      // check timeout option
      if ( auto timeoutOption{ respOptions.timeoutOption()}; timeoutOption)
      {
        receiveTimeout( *timeoutOption);
      }

      // check transfer size option
       if ( auto transferSizeOption{ respOptions.transferSizeOption()})
       {
        if ( !dataHandler->receivedTransferSize( *transferSizeOption))
        {
          Packets::ErrorPacket errorPacket{
            ErrorCode::DiskFullOrAllocationExceeds,
            "FILE TO BIG"};

          send( errorPacket);

          // send transfer error to be in sync with TFTP client
          finished( TransferStatus::TransferError, std::move( errorPacket));
          return;
        }
      }

      // send OACK
      send( Packets::OptionsAcknowledgementPacket{ respOptions});
    }

    // start receive loop
    OperationImpl::start();
  }
  catch ( ...)
  {
    finished( TransferStatus::CommunicationError);
  }
}

void WriteRequestOperationImpl::finished(
  const TransferStatus status,
  ErrorInfo &&errorInfo) noexcept
{
  BOOST_LOG_FUNCTION();

  OperationImpl::finished( status, std::move( errorInfo));
  dataHandler->finished();
}

void WriteRequestOperationImpl::handleDataPacket(
  const UdpAddressType &,
  const Packets::DataPacket &dataPacket)
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
    "RX: " << static_cast< std::string>( dataPacket);

  // Check retransmission
  if (dataPacket.blockNumber() == lastReceivedBlockNumber)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
      "Retransmission of last packet - only send ACK";

    send( Packets::AcknowledgementPacket( lastReceivedBlockNumber));

    // receive next packet
    receive();

    return;
  }

  // check not expected block
  if (dataPacket.blockNumber() != lastReceivedBlockNumber.next())
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Unexpected packet";

    Packets::ErrorPacket errorPacket(
      ErrorCode::IllegalTftpOperation,
      "Wrong block number");

    send( errorPacket);

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket));

    return;
  }

  // check for too much data
  if (dataPacket.dataSize() > receiveDataSize)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Too much data received";

    Packets::ErrorPacket errorPacket(
      ErrorCode::IllegalTftpOperation,
      "Too much data");

    send( errorPacket);

   // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket));

    return;
  }

  // call data handler
  dataHandler->receviedData( dataPacket.data());

  // increment block number
  lastReceivedBlockNumber++;

  send( Packets::AcknowledgementPacket( lastReceivedBlockNumber));

  // if received data size is smaller then the expected -> last packet has been
  // received
  if (dataPacket.dataSize() < receiveDataSize)
  {
    finished( TransferStatus::Successful);
  }
  else
  {
    // receive next packet
    receive();
  }
}

void WriteRequestOperationImpl::handleAcknowledgementPacket(
  const UdpAddressType &,
  const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_FUNCTION();

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) << "RX ERROR: " <<
    static_cast< std::string>( acknowledgementPacket);

  Packets::ErrorPacket errorPacket(
    ErrorCode::IllegalTftpOperation,
    "ACK not expected");

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket));
}

}
