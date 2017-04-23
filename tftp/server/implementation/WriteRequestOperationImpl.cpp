/*
 * $Date$
 * $Revision$
 */
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

namespace Tftp {
namespace Server {

WriteRequestOperationImpl::WriteRequestOperationImpl(
  boost::asio::io_service &ioService,
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const TftpServerInternal &tftpServerInternal,
  const UdpAddressType &clientAddress,
  const Options::OptionList &clientOptions,
  const UdpAddressType &serverAddress) :
  OperationImpl(
    ioService,
    completionHandler,
    tftpServerInternal,
    clientAddress,
    clientOptions,
    serverAddress),
  dataHandler( dataHandler),
  receiveDataSize( DefaultDataSize),
  lastReceivedBlockNumber( 0)
{
}

WriteRequestOperationImpl::WriteRequestOperationImpl(
  boost::asio::io_service &ioService,
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const TftpServerInternal &tftpServerInternal,
  const UdpAddressType &clientAddress,
  const Options::OptionList &clientOptions) :
  OperationImpl(
    ioService,
    completionHandler,
    tftpServerInternal,
    clientAddress,
    clientOptions),
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
    // option negotiation leads to empty option list
    if (!getOptions().hasOptions())
    {
      // Then no NOACK is sent back - a simple ACK is sent.
      send( Packets::AcknowledgementPacket( 0));
    }
    else
    {
      //validate received options

      // check blocksize option
      if ( 0 != getOptions().getBlocksizeOption())
      {
        receiveDataSize = getOptions().getBlocksizeOption();

        // set receive data size if necessary
        if ( receiveDataSize > DefaultDataSize)
        {
          setMaxReceivePacketSize(
            receiveDataSize + DefaultTftpDataPacketHeaderSize);
        }
      }

      // check timeout option
      if ( 0 != getOptions().getTimeoutOption())
      {
        setReceiveTimeout( getOptions().getTimeoutOption());
      }

      // check transfer size option
      if ( getOptions().hasTransferSizeOption())
      {
        if ( !dataHandler->receivedTransferSize(
          getOptions().getTransferSizeOption()))
        {
          Packets::ErrorPacket errorPacket(
            ErrorCode::DiskFullOrAllocationExceeds,
            "FILE TO BIG");

          send( errorPacket);

          // send transfer error to be in sync with TFTP client
          finished( TransferStatus::TransferError, std::move( errorPacket));
          return;
        }
      }

      // send OACK
      send( Packets::OptionsAcknowledgementPacket( getOptions()));
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
  if (dataPacket.getBlockNumber() == lastReceivedBlockNumber)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
      "Retransmission of last packet - only send ACK";

    send( Packets::AcknowledgementPacket( lastReceivedBlockNumber));

    // receive next packet
    receive();

    return;
  }

  // check not expected block
  if (dataPacket.getBlockNumber() != lastReceivedBlockNumber.next())
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
  if (dataPacket.getDataSize() > receiveDataSize)
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
  dataHandler->receviedData( dataPacket.getData());

  // increment block number
  lastReceivedBlockNumber++;

  send( Packets::AcknowledgementPacket( lastReceivedBlockNumber));

  // if received data size is smaller then the expected -> last packet has been
  // received
  if (dataPacket.getDataSize() < receiveDataSize)
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
}
