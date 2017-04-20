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
  const TftpClientInternal &tftpClient,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode,
  const UdpAddressType &from,
  OperationCompletedHandler completionHandler):
  OperationImpl(
    ioService,
    RequestType::Write,
    tftpClient,
    serverAddress,
    filename,
    mode,
    from,
    completionHandler),
  dataHandler( dataHandler),
  transmitDataSize( DefaultDataSize),
  lastDataPacketTransmitted( false),
  lastTransmittedBlockNumber( 0)
{
}

WriteRequestOperationImpl::WriteRequestOperationImpl(
  boost::asio::io_service &ioService,
  TransmitDataHandlerPtr dataHandler,
  const TftpClientInternal &tftpClient,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode,
  OperationCompletedHandler completionHandler):
  OperationImpl(
    ioService,
    RequestType::Write,
    tftpClient,
    serverAddress,
    filename,
    mode,
    completionHandler),
  dataHandler( dataHandler),
  transmitDataSize( DefaultDataSize),
  lastDataPacketTransmitted( false),
  lastTransmittedBlockNumber( 0)
{
}

void WriteRequestOperationImpl::start()
{
  try
  {
    transmitDataSize = DefaultDataSize;
    lastDataPacketTransmitted = false;
    lastTransmittedBlockNumber = 0;

    // if Transfer Size option is set, query them from the handler
    if (getOptions().hasTransferSizeOption())
    {
      uint64_t transferSize;

      // If the handler supplies a transfer size
      if (dataHandler->requestedTransferSize( transferSize))
      {
        // set transfer size TFTP option
        getOptions().addTransferSizeOption( transferSize);
      }
      else
      {
        // otherwise remove this option
        getOptions().removeTransferSizeOption();
      }
    }

    // send write request packet
    sendFirst( Packets::WriteRequestPacket(
      getFilename(),
      getMode(),
      getOptions()));

    // wait for answers
    OperationImpl::start();
  }
  catch (...)
  {
    finished( false);
  }
}

void WriteRequestOperationImpl::finished( const bool successful) noexcept
{
  OperationImpl::finished( successful);
  dataHandler->finished();
}

void WriteRequestOperationImpl::sendData()
{
  lastTransmittedBlockNumber++;

  Packets::DataPacket data(
    lastTransmittedBlockNumber,
    dataHandler->sendData( transmitDataSize));

  if (data.getDataSize() < transmitDataSize)
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
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX ERROR: " <<
    static_cast< std::string>( dataPacket);

  send( Packets::ErrorPacket(
    ErrorCode::IllegalTftpOperation,
    "DATA not expected"));

  // Operation completed
  finished( false);
}

void WriteRequestOperationImpl::handleAcknowledgementPacket(
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

    send( Packets::ErrorPacket(
      ErrorCode::IllegalTftpOperation,
      "Wrong block number"));

    finished( false);
    return;
  }

  // if ACK for last data packet - QUIT
  if (lastDataPacketTransmitted)
  {
    finished( true);

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
  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info) << "RX: " <<
    static_cast< std::string>( optionsAcknowledgementPacket);

  OptionList options( optionsAcknowledgementPacket.getOptions());

  // check empty options
  if (options.getOptions().empty())
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Received option list is empty";

    send( Packets::ErrorPacket(
      ErrorCode::IllegalTftpOperation,
      "Empty OACK not allowed"));

    finished( false);
    return;
  }

  // perform option negotiation
  OptionList negotiatedOptions = getOptions().negotiateClient( options);
  if (negotiatedOptions.getOptions().empty())
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Option negotiation failed";

    send( Packets::ErrorPacket(
      ErrorCode::TftpOptionRefused,
      "Option negotiation failed"));

    finished( false);
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
    setReceiveTimeout( negotiatedOptions.getTimeoutOption());
  }

  // Transfer size option is not checked here (already performed during option negotiation)

  // send data
  sendData();

  // wait for next packet
  receive();
}

}
}
