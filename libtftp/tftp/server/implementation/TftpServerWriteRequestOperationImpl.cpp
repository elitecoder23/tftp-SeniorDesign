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
 * @brief Definition of class Tftp::Server::TftpServerWriteRequestOperationImpl.
 **/

#include "TftpServerWriteRequestOperationImpl.hpp"

#include <tftp/TftpException.hpp>
#include <tftp/TftpReceiveDataOperationHandler.hpp>

#include <tftp/packet/AcknowledgementPacket.hpp>
#include <tftp/packet/OptionsAcknowledgementPacket.hpp>
#include <tftp/packet/DataPacket.hpp>
#include <tftp/packet/ErrorPacket.hpp>

#include <helper/Logger.hpp>

namespace Tftp {
namespace Server {

TftpServerWriteRequestOperationImpl::TftpServerWriteRequestOperationImpl(
  TftpReceiveDataOperationHandler &handler,
  const TftpServerInternal &tftpServerInternal,
  const UdpAddressType &clientAddress,
  const OptionList &clientOptions,
  const UdpAddressType &serverAddress) :
  TftpServerOperationImpl(
    tftpServerInternal,
    clientAddress,
    clientOptions,
    serverAddress),
  handler( handler),
  receiveDataSize( DefaultDataSize),
  lastReceivedBlockNumber( 0)
{
}

TftpServerWriteRequestOperationImpl::TftpServerWriteRequestOperationImpl(
  TftpReceiveDataOperationHandler &handler,
  const TftpServerInternal &tftpServerInternal,
  const UdpAddressType &clientAddress,
  const OptionList &clientOptions) :
  TftpServerOperationImpl( tftpServerInternal, clientAddress, clientOptions),
  handler( handler),
  receiveDataSize( DefaultDataSize),
  lastReceivedBlockNumber( 0)
{
}

void TftpServerWriteRequestOperationImpl::operator()()
{
  try
  {
    // option negotiation leads to empty option list
    if (!getOptions().hasOptions())
    {
      // Then no NOACK is sent back - a simple ACK is sent.
      send( AcknowledgementPacket( 0));
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
        if ( !handler.receivedTransferSize(
          getOptions().getTransferSizeOption()))
        {
          send(
            ErrorPacket(
              ErrorCode::DISK_FULL_OR_ALLOCATION_EXCEEDS,
              "FILE TO BIG"));

          BOOST_THROW_EXCEPTION(
            TftpException() << AdditionalInfo( "FILE TO BIG"));
        }
      }

      // send OACK
      send( OptionsAcknowledgementPacket( getOptions()));
    }

    // start receive loop
    TftpServerOperationImpl::operator()();
  }
  catch ( ...)
  {
    handler.finishedOperation();

    throw;
  }

  handler.finishedOperation();
}

void TftpServerWriteRequestOperationImpl::handleDataPacket(
  const UdpAddressType &,
  const DataPacket &dataPacket)
{
  BOOST_LOG_TRIVIAL( info)<< "RX: " << dataPacket.toString();

  // Check retransmission
  if (dataPacket.getBlockNumber() == lastReceivedBlockNumber)
  {
    BOOST_LOG_TRIVIAL( info) << "Retransmission of last packet - only send ACK";

    send( AcknowledgementPacket( lastReceivedBlockNumber));

    return;
  }

  // check not expected block
  if (dataPacket.getBlockNumber() != lastReceivedBlockNumber.next())
  {
    BOOST_LOG_TRIVIAL( error) << "Unexpected packet";

    send( ErrorPacket(
      ErrorCode::ILLEGAL_TFTP_OPERATION,
      "Wrong block number"));

    // Operation completed
    finished();

    //! @throw CommunicationException On wrong block number.
    BOOST_THROW_EXCEPTION( CommunicationException() <<
      AdditionalInfo( "Wrong block number"));
  }

  // check for too much data
  if (dataPacket.getDataSize() > receiveDataSize)
  {
    BOOST_LOG_TRIVIAL( error) << "Too much data received";

    send( ErrorPacket(
      ErrorCode::ILLEGAL_TFTP_OPERATION,
      "Too much data"));

   // Operation completed
   finished();

   //! @throw CommunicationException When to much data is received.
   BOOST_THROW_EXCEPTION( CommunicationException() <<
     AdditionalInfo( "Too much data received"));
  }

  // call data handler
  handler.receviedData( dataPacket.getData());

  // increment block number
  lastReceivedBlockNumber++;

  send( AcknowledgementPacket( lastReceivedBlockNumber));

  // if received data size is smaller then the expected -> last packet has been
  // received
  if (dataPacket.getDataSize() < receiveDataSize)
  {
    finished();
  }
  else
  {
    // receive next packet
    receive();
  }
}

void TftpServerWriteRequestOperationImpl::handleAcknowledgementPacket(
  const UdpAddressType &,
  const AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_TRIVIAL( error) << "RX ERROR: " << acknowledgementPacket.toString();

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

}
}
