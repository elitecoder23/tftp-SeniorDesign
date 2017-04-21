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
 * @brief Definition of class Tftp::Server::ReadRequestOperationImpl.
 **/

#include "ReadRequestOperationImpl.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>
#include <tftp/TransmitDataHandler.hpp>
#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/ErrorPacket.hpp>

namespace Tftp {
namespace Server {

ReadRequestOperationImpl::ReadRequestOperationImpl(
  boost::asio::io_service &ioService,
  TransmitDataHandlerPtr dataHandler,
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
  transmitDataSize( DefaultDataSize),
  lastDataPacketTransmitted( false),
  lastTransmittedBlockNumber( 0)
{
}

ReadRequestOperationImpl::ReadRequestOperationImpl(
  boost::asio::io_service &ioService,
  TransmitDataHandlerPtr dataHandler,
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
  transmitDataSize( DefaultDataSize),
  lastDataPacketTransmitted( false),
  lastTransmittedBlockNumber( 0)
{
}

void ReadRequestOperationImpl::start()
{
  try
  {
    // option negotiation leads to empty option list
    if ( !getOptions().hasOptions())
    {
      sendData();
    }
    else
    {
      // check blocksize option
      if ( 0 != getOptions().getBlocksizeOption())
      {
        transmitDataSize = getOptions().getBlocksizeOption();
      }

      // check timeout option
      if ( 0 != getOptions().getTimeoutOption())
      {
        setReceiveTimeout( getOptions().getTimeoutOption());
      }

      // check transfer size option
      if ( getOptions().hasTransferSizeOption())
      {
        uint64_t transferSize;

        // add transfer size to answer only, if handler supply it.
        if ( dataHandler->requestedTransferSize( transferSize))
        {
          getOptions().addTransferSizeOption( transferSize);
        }
        else
        {
          getOptions().removeTransferSizeOption();
        }
      }

      // if transfer size option is the only option requested, but the handler
      // does not supply it -> empty OACK is not sent biut data directly
      if ( getOptions().hasOptions())
      {
        // Send OACK
        send( Packets::OptionsAcknowledgementPacket( getOptions()));
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
  OperationImpl::finished( status, std::move( errorInfo));
  dataHandler->finished();
}

void ReadRequestOperationImpl::sendData()
{
  lastTransmittedBlockNumber++;

  Packets::DataPacket data(
    lastTransmittedBlockNumber,
    dataHandler->sendData( transmitDataSize));

  if ( data.getDataSize() < transmitDataSize)
  {
    lastDataPacketTransmitted = true;
  }

  // send data
  send( data);
}

void ReadRequestOperationImpl::handleDataPacket(
  const UdpAddressType &,
  const Packets::DataPacket &dataPacket)
{
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
  const UdpAddressType &,
  const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX: " <<
    static_cast< std::string>( acknowledgementPacket);

  // check retransmission
  if (acknowledgementPacket.getBlockNumber() == lastTransmittedBlockNumber.previous())
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
      "Received previous ACK packet: retry of last data package - "
      "IGNORE it due to Sorcerer's Apprentice Syndrome";

    return;
  }

  // check invalid block number
  if (acknowledgementPacket.getBlockNumber() != lastTransmittedBlockNumber)
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
    finished( TransferStatus::Successful);

    return;
  }

  // send data
  sendData();

  // receive next packet
  receive();
}

}
}
