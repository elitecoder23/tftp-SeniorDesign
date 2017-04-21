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
 * @brief Definition of class Tftp::Client::ReadRequestOperationImpl.
 **/

#include "ReadRequestOperationImpl.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>
#include <tftp/ReceiveDataHandler.hpp>
#include <tftp/packets/PacketFactory.hpp>

#include <helper/Dump.hpp>

namespace Tftp {
namespace Client {

ReadRequestOperationImpl::ReadRequestOperationImpl(
  boost::asio::io_service &ioService,
  ReceiveDataHandlerPtr dataHandler,
  const TftpClientInternal &tftpClient,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode,
  const UdpAddressType &from,
  OperationCompletedHandler completionHandler) :
  OperationImpl(
    ioService,
    tftpClient,
    serverAddress,
    filename,
    mode,
    from,
    completionHandler),
  dataHandler( dataHandler),
  receiveDataSize( DefaultDataSize),
  lastReceivedBlockNumber( 0)
{
}

ReadRequestOperationImpl::ReadRequestOperationImpl(
  boost::asio::io_service &ioService,
  ReceiveDataHandlerPtr dataHandler,
  const TftpClientInternal &tftpClient,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode,
  OperationCompletedHandler completionHandler) :
  OperationImpl(
    ioService,
    tftpClient,
    serverAddress,
    filename,
    mode,
    completionHandler),
  dataHandler( dataHandler),
  receiveDataSize( DefaultDataSize),
  lastReceivedBlockNumber( 0)
{
}

void ReadRequestOperationImpl::start()
{
  try
  {
    receiveDataSize = DefaultDataSize;
    lastReceivedBlockNumber = 0;

    // send read request packet
    sendFirst(
      Packets::ReadRequestPacket( getFilename(), getMode(), getOptions()));

    // wait for answers
    OperationImpl::start();
  }
  catch ( ...)
  {
    finished( TransferStatus::CommunicationError);
  }
}

void ReadRequestOperationImpl::finished( const TransferStatus status) noexcept
{
  OperationImpl::finished( status);
  dataHandler->finished();
}

void ReadRequestOperationImpl::handleDataPacket(
  const UdpAddressType &,
  const Packets::DataPacket &dataPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX: " <<
    static_cast< std::string>( dataPacket);

  // check retransmission of last packet
  if (dataPacket.getBlockNumber() == lastReceivedBlockNumber)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) <<
      "Received last data package again. Re-ACK them";

    // Retransmit last ACK packet
    send( Packets::AcknowledgementPacket( lastReceivedBlockNumber));

    return;
  }

  // check unexpected block number
  if (dataPacket.getBlockNumber() != lastReceivedBlockNumber.next())
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Wrong Data packet block number";

    // send error packet
    send( Packets::ErrorPacket(
      ErrorCode::IllegalTftpOperation,
      "Block Number not expected"));

    // Operation completed
    finished( TransferStatus::TransferError);
    return;
  }

  // check for too much data
  if (dataPacket.getDataSize() > receiveDataSize)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Too much data received";

    // send error packet
    send( Packets::ErrorPacket(
      ErrorCode::IllegalTftpOperation,
      "Too much data"));

    // Operation completed
    finished( TransferStatus::TransferError);
    return;
  }

  // call call-back
  dataHandler->receviedData( dataPacket.getData());

  // increment received block number
  lastReceivedBlockNumber++;

  // send ACK
  send( Packets::AcknowledgementPacket( lastReceivedBlockNumber));

  // if received data size is smaller then the expected
  if (dataPacket.getDataSize() < receiveDataSize)
  {
    // last packet has been received and operation is finished
    finished( TransferStatus::Successful);
  }
  else
  {
    // otherwise wait for next data package
    receive();
  }
}

void ReadRequestOperationImpl::handleAcknowledgementPacket(
  const UdpAddressType &,
  const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX ERROR: " <<
    static_cast< std::string>( acknowledgementPacket);

  // send Error
  send( Packets::ErrorPacket(
    ErrorCode::IllegalTftpOperation,
    "ACK not expected"));

  // Operation completed
  finished( TransferStatus::TransferError);
}

void ReadRequestOperationImpl::handleOptionsAcknowledgementPacket(
  const UdpAddressType &,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket)
{
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX ERROR: " <<
    static_cast< std::string>( optionsAcknowledgementPacket);

  OptionList options = optionsAcknowledgementPacket.getOptions();

  // check empty options
  if (options.getOptions().empty())
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Received option list is empty";

    send( Packets::ErrorPacket(
      ErrorCode::IllegalTftpOperation,
      "Empty OACK not allowed"));

    // Operation completed
    finished( TransferStatus::TransferError);
    return;
  }

  // perform option negotiation
  OptionList negotiatedOptions = getOptions().negotiateClient( options);

  // Check empty options list
  if (negotiatedOptions.getOptions().empty())
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Option negotiation failed";

    send( Packets::ErrorPacket(
      ErrorCode::TftpOptionRefused,
      "Option negotiation failed"));

    // Operation completed
    finished( TransferStatus::OptionNegotiationError);
    return;
  }

  // check blocksize option
  if (0 != negotiatedOptions.getBlocksizeOption())
  {
    receiveDataSize = negotiatedOptions.getBlocksizeOption();

    // set maximum receive data size if necessary
    if (receiveDataSize > DefaultDataSize)
    {
      setMaxReceivePacketSize(
        receiveDataSize + DefaultTftpDataPacketHeaderSize);
    }
  }

  // check timeout option
  if (0 != negotiatedOptions.getTimeoutOption())
  {
    setReceiveTimeout( negotiatedOptions.getTimeoutOption());
  }

  // check transfer size option
  if (negotiatedOptions.hasTransferSizeOption())
  {
    if (!dataHandler->receivedTransferSize( negotiatedOptions.getTransferSizeOption()))
    {
      send( Packets::ErrorPacket(
        ErrorCode::DiskFullOrAllocationExceeds,
        "FILE TO BIG"));

      // Operation completed
      finished( TransferStatus::TransferError);
      return;
    }
  }

  // send Acknowledgement with block number set to 0
  send( Packets::AcknowledgementPacket( 0));

  // receive next packet
  receive();
}

}
}
