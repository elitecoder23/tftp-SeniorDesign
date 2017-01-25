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
 * @brief Definition of class Tftp::Client::TftpClientReadRequestOperationImpl.
 **/

#include "TftpClientReadRequestOperationImpl.hpp"
#include <tftp/TftpException.hpp>
#include <tftp/TftpReceiveDataOperationHandler.hpp>
#include <tftp/packet/PacketFactory.hpp>

#include <helper/Dump.hpp>
#include <helper/Logger.hpp>

namespace Tftp {
namespace Client {

TftpClientReadRequestOperationImpl::TftpClientReadRequestOperationImpl(
  TftpReceiveDataOperationHandler &handler,
  const TftpClientInternal &tftpClientInternal,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode,
  const UdpAddressType &from) :
  TftpClientOperationImpl(
    RequestType::Read,
    tftpClientInternal,
    serverAddress,
    filename,
    mode,
    from),
  handler( handler),
  receiveDataSize( DefaultDataSize),
  lastReceivedBlockNumber( 0)
{
}

TftpClientReadRequestOperationImpl::TftpClientReadRequestOperationImpl(
  TftpReceiveDataOperationHandler &handler,
  const TftpClientInternal &tftpClientInternal,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode) :
  TftpClientOperationImpl(
    RequestType::Read,
    tftpClientInternal,
    serverAddress,
    filename,
    mode),
  handler( handler),
  receiveDataSize( DefaultDataSize),
  lastReceivedBlockNumber( 0)
{
}

void TftpClientReadRequestOperationImpl::operator ()( void)
{
  try
  {
    receiveDataSize = DefaultDataSize;
    lastReceivedBlockNumber = 0;

    // send read request packet
    sendFirst( ReadRequestPacket( getFilename(), getMode(), getOptions()));

    // wait for answers
    TftpClientOperationImpl::operator ()();
  }
  catch ( ...)
  {
    handler.finishedOperation();

    throw;
  }

  handler.finishedOperation();
}

void TftpClientReadRequestOperationImpl::handleDataPacket(
  const UdpAddressType &,
  const DataPacket &dataPacket)
{
  BOOST_LOG_TRIVIAL( info) << "RX: " << dataPacket.toString();

  // check retransmission of last packet
  if (dataPacket.getBlockNumber() == lastReceivedBlockNumber)
  {
    BOOST_LOG_TRIVIAL( info) << "Received last data package again. Re-ACK them";

    // Retransmit last ACK packet
    send( AcknowledgementPacket( lastReceivedBlockNumber));

    return;
  }

  // check unexpected block number
  if (dataPacket.getBlockNumber() != lastReceivedBlockNumber.next())
  {
    BOOST_LOG_TRIVIAL( error) << "Wrong Data packet block number";

    // send error packet
    send( ErrorPacket(
      ErrorCode::ILLEGAL_TFTP_OPERATION,
      "Block Number not expected"));

    // Operation completed
    finished();

    //! @throw CommunicationException when the DATA packet has an invalid block
    //! number
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( "Wrong Data packet block number") <<
      PacketTypeInfo( PacketType::Data));
  }

  // check for too much data
  if (dataPacket.getDataSize() > receiveDataSize)
  {
    BOOST_LOG_TRIVIAL( error) << "Too much data received";

    // send error packet
    send( ErrorPacket(
      ErrorCode::ILLEGAL_TFTP_OPERATION,
      "Too much data"));

    // Operation completed
    finished();

    //! @throw InvalidPacketException When to much data has been received.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( "To much data received") <<
      PacketTypeInfo( PacketType::Data));
  }

  // call call-back
  handler.receviedData( dataPacket.getData());

  // increment received block number
  lastReceivedBlockNumber++;

  // send ACK
  send( AcknowledgementPacket( lastReceivedBlockNumber));

  // if received data size is smaller then the expected
  if (dataPacket.getDataSize() < receiveDataSize)
  {
    // last packet has been received and operation is finished
    finished();
  }
  else
  {
    // otherwise wait for next data package
    receive();
  }
}

void TftpClientReadRequestOperationImpl::handleAcknowledgementPacket(
  const UdpAddressType &,
  const AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_TRIVIAL( info) <<
    "RX ERROR: " << acknowledgementPacket.toString();

  // send Error
  send( ErrorPacket(
    ErrorCode::ILLEGAL_TFTP_OPERATION,
    "ACK not expected"));

  // Operation completed
  finished();

  //! @throw CommunicationException Always, because this packet is invalid.
  BOOST_THROW_EXCEPTION( CommunicationException() <<
    AdditionalInfo( "Unexpected packet received") <<
    PacketTypeInfo( PacketType::Acknowledgement));
}

void TftpClientReadRequestOperationImpl::handleOptionsAcknowledgementPacket(
  const UdpAddressType &,
  const OptionsAcknowledgementPacket &optionsAcknowledgementPacket)
{
  BOOST_LOG_TRIVIAL( info) <<
    "RX ERROR: " << optionsAcknowledgementPacket.toString();

  OptionList options = optionsAcknowledgementPacket.getOptions();

  // check empty options
  if (options.getOptions().empty())
  {
    BOOST_LOG_TRIVIAL( error) << "Received option list is empty";

    send( ErrorPacket(
      ErrorCode::ILLEGAL_TFTP_OPERATION,
      "Empty OACK not allowed"));

    // Operation completed
    finished();

    //! @throw CommunicationException When OACK response is empty.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( "Received option list is empty"));
  }

  // perform option negotiation
  OptionList negotiatedOptions = getOptions().negotiateClient( options);

  // Check empty options list
  if (negotiatedOptions.getOptions().empty())
  {
    BOOST_LOG_TRIVIAL( error) << "Option negotiation failed";

    send( ErrorPacket(
      ErrorCode::TFTP_OPTION_REFUSED,
      "Option negotiation failed"));

    // Operation completed
    finished();

    //! @throw OptionNegotiationException Option negotiation failed.
    BOOST_THROW_EXCEPTION( OptionNegotiationException() <<
      AdditionalInfo( "Option negotiation failed"));
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
    if (!handler.receivedTransferSize( negotiatedOptions.getTransferSizeOption()))
    {
      send( ErrorPacket(
        ErrorCode::DISK_FULL_OR_ALLOCATION_EXCEEDS,
        "FILE TO BIG"));

      // Operation completed
      finished();

      //! @throw TftpException When file is to big.
      BOOST_THROW_EXCEPTION( TftpException() <<
        AdditionalInfo( "FILE TO BIG"));
    }
  }

  // send Acknowledgement with block number set to 0
  send( AcknowledgementPacket( 0));

  // receive next packet
  receive();
}

}
}
