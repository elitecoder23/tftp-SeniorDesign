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
#include <tftp/ReceiveDataOperationHandler.hpp>
#include <tftp/packets/PacketFactory.hpp>

#include <helper/Dump.hpp>

namespace Tftp {
namespace Client {

ReadRequestOperationImpl::ReadRequestOperationImpl(
  ReceiveDataOperationHandler &handler,
  const TftpClientInternal &tftpClient,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode,
  const UdpAddressType &from) :
  OperationImpl(
    RequestType::Read,
    tftpClient,
    serverAddress,
    filename,
    mode,
    from),
  handler( handler),
  receiveDataSize( DefaultDataSize),
  lastReceivedBlockNumber( 0)
{
}

ReadRequestOperationImpl::ReadRequestOperationImpl(
  ReceiveDataOperationHandler &handler,
  const TftpClientInternal &tftpClient,
  const UdpAddressType &serverAddress,
  const string &filename,
  const TransferMode mode) :
  OperationImpl(
    RequestType::Read,
    tftpClient,
    serverAddress,
    filename,
    mode),
  handler( handler),
  receiveDataSize( DefaultDataSize),
  lastReceivedBlockNumber( 0)
{
}

void ReadRequestOperationImpl::operator ()( void)
{
  try
  {
    receiveDataSize = DefaultDataSize;
    lastReceivedBlockNumber = 0;

    // send read request packet
    sendFirst(
      Packets::ReadRequestPacket( getFilename(), getMode(), getOptions()));

    // wait for answers
    OperationImpl::operator ()();
  }
  catch ( ...)
  {
    handler.finishedOperation();

    throw;
  }

  handler.finishedOperation();
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
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Too much data received";

    // send error packet
    send( Packets::ErrorPacket(
      ErrorCode::IllegalTftpOperation,
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
  send( Packets::AcknowledgementPacket( lastReceivedBlockNumber));

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
  finished();

  //! @throw CommunicationException Always, because this packet is invalid.
  BOOST_THROW_EXCEPTION( CommunicationException() <<
    AdditionalInfo( "Unexpected packet received") <<
    PacketTypeInfo( PacketType::Acknowledgement));
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
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error) <<
      "Option negotiation failed";

    send( Packets::ErrorPacket(
      ErrorCode::TftpOptionRefused,
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
      send( Packets::ErrorPacket(
        ErrorCode::DiskFullOrAllocationExceeds,
        "FILE TO BIG"));

      // Operation completed
      finished();

      //! @throw TftpException When file is to big.
      BOOST_THROW_EXCEPTION( TftpException() <<
        AdditionalInfo( "FILE TO BIG"));
    }
  }

  // send Acknowledgement with block number set to 0
  send( Packets::AcknowledgementPacket( 0));

  // receive next packet
  receive();
}

}
}
