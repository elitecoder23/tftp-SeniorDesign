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
#include <tftp/TransmitDataOperationHandler.hpp>
#include <tftp/packets/PacketFactory.hpp>

#include <helper/Dump.hpp>
#include <helper/Logger.hpp>

namespace Tftp {
namespace Client {

WriteRequestOperationImpl::WriteRequestOperationImpl(
  TransmitDataOperationHandler &handler,
  const TftpClientInternal &tftpClient,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode,
  const UdpAddressType &from):
  OperationImpl(
    RequestType::Write,
    tftpClient,
    serverAddress,
    filename,
    mode,
    from),
  handler( handler),
  transmitDataSize( DefaultDataSize),
  lastDataPacketTransmitted( false),
  lastTransmittedBlockNumber( 0)
{
}

WriteRequestOperationImpl::WriteRequestOperationImpl(
  TransmitDataOperationHandler &handler,
  const TftpClientInternal &tftpClient,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode):
  OperationImpl(
    RequestType::Write,
    tftpClient,
    serverAddress,
    filename,
    mode),
  handler( handler),
  transmitDataSize( DefaultDataSize),
  lastDataPacketTransmitted( false),
  lastTransmittedBlockNumber( 0)
{
}

void WriteRequestOperationImpl::operator()()
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
      if (handler.requestedTransferSize( transferSize))
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
    OperationImpl::operator()();
  }
  catch (...)
  {
    handler.finishedOperation();

    throw;
  }

  handler.finishedOperation();
}

void WriteRequestOperationImpl::sendData()
{
  lastTransmittedBlockNumber++;

  Packets::DataPacket data(
    lastTransmittedBlockNumber,
    handler.sendData( transmitDataSize));

  if (data.getDataSize() < transmitDataSize)
  {
    lastDataPacketTransmitted = true;
  }

  //! send packet
  send( data);
}

void WriteRequestOperationImpl::handleDataPacket(
  const UdpAddressType &,
  const Packets::DataPacket &dataPacket)
{
  BOOST_LOG_TRIVIAL( info) << "RX ERROR: " <<
    static_cast< std::string>( dataPacket);

  send( Packets::ErrorPacket(
    ErrorCode::IllegalTftpOperation,
    "DATA not expected"));

  // Operation completed
  finished();

  //! @throw CommunicationException Always, because this packet is invalid.
  BOOST_THROW_EXCEPTION( CommunicationException() <<
    AdditionalInfo( "Unexpected packet received") <<
    PacketTypeInfo( PacketType::Data));
}

void WriteRequestOperationImpl::handleAcknowledgementPacket(
	const UdpAddressType &,
	const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_TRIVIAL( info) << "RX: " <<
    static_cast< std::string>( acknowledgementPacket);

  // check retransmission
  if (acknowledgementPacket.getBlockNumber() == lastTransmittedBlockNumber.previous())
  {
    BOOST_LOG_TRIVIAL( info) <<
      "Received previous ACK packet: retry of last data package - "
      "IGNORE it due to Sorcerer's Apprentice Syndrome";

    return;
  }

  // check invalid block number
  if (acknowledgementPacket.getBlockNumber() != lastTransmittedBlockNumber)
  {
    BOOST_LOG_TRIVIAL( error) << "Invalid block number received";

    send( Packets::ErrorPacket(
      ErrorCode::IllegalTftpOperation,
      "Wrong block number"));

    finished();

    //! @throw CommunicationException When invalid block number has been received.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( "Invalid block number received") <<
      PacketTypeInfo( PacketType::Acknowledgement));
  }

  // if ACK for last data packet - QUIT
  if (lastDataPacketTransmitted)
  {
    finished();

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
  BOOST_LOG_TRIVIAL( info) << "RX: " <<
    static_cast< std::string>( optionsAcknowledgementPacket);

  OptionList options( optionsAcknowledgementPacket.getOptions());

  // check empty options
  if (options.getOptions().empty())
  {
    BOOST_LOG_TRIVIAL( error) << "Received option list is empty";

    send( Packets::ErrorPacket(
      ErrorCode::IllegalTftpOperation,
      "Empty OACK not allowed"));

    //! @throw CommunicationException When Option list is empty.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( "Received option list is empty") <<
      PacketTypeInfo( PacketType::OptionsAcknowledgement));
  }

  // perform option negotiation
  OptionList negotiatedOptions = getOptions().negotiateClient( options);
  if (negotiatedOptions.getOptions().empty())
  {
    BOOST_LOG_TRIVIAL( error) << "Option negotiation failed";

    send( Packets::ErrorPacket(
      ErrorCode::TftpOptionRefused,
      "Option negotiation failed"));

    //! @throw OptionNegotiationException When option negotiation failed.
    BOOST_THROW_EXCEPTION( OptionNegotiationException() <<
      AdditionalInfo( "Option negotiation failed") <<
      PacketTypeInfo( PacketType::OptionsAcknowledgement));
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
