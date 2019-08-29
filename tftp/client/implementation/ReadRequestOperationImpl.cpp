/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Tftp::Client::ReadRequestOperationImpl.
 **/

#include "ReadRequestOperationImpl.hpp"

#include <tftp/packets/AcknowledgementPacket.hpp>
#include <tftp/packets/ReadRequestPacket.hpp>
#include <tftp/packets/DataPacket.hpp>
#include <tftp/packets/OptionsAcknowledgementPacket.hpp>

#include <tftp/TftpException.hpp>
#include <tftp/TftpLogger.hpp>
#include <tftp/ReceiveDataHandler.hpp>
#include <tftp/TftpConfiguration.hpp>

#include <helper/Dump.hpp>

namespace Tftp::Client {

ReadRequestOperationImpl::ReadRequestOperationImpl(
  boost::asio::io_context &ioContext,
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const TftpClientInternal &tftpClient,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const TransferMode mode,
  const Options::OptionList &clientOptions) :
  OperationImpl{
    ioContext,
    completionHandler,
    tftpClient,
    remote,
    filename,
    mode,
    clientOptions},
  dataHandler{ dataHandler},
  receiveDataSize{ DefaultDataSize},
  lastReceivedBlockNumber{ 0U}
{
}

ReadRequestOperationImpl::ReadRequestOperationImpl(
  boost::asio::io_context &ioContext,
  ReceiveDataHandlerPtr dataHandler,
  OperationCompletedHandler completionHandler,
  const TftpClientInternal &tftpClient,
  const boost::asio::ip::udp::endpoint &remote,
  std::string_view filename,
  const TransferMode mode,
  const Options::OptionList &clientOptions,
  const boost::asio::ip::udp::endpoint &local) :
  OperationImpl{
    ioContext,
    completionHandler,
    tftpClient,
    remote,
    filename,
    mode,
    clientOptions,
    local},
  dataHandler{ dataHandler},
  receiveDataSize{ DefaultDataSize},
  lastReceivedBlockNumber{ 0U}
{
}

void ReadRequestOperationImpl::start()
{
  BOOST_LOG_FUNCTION()

  try
  {
    receiveDataSize = DefaultDataSize;
    lastReceivedBlockNumber = 0U;

    Options::OptionList reqOptions{ options()};

    // Add transfer size option with size '0' if requested.
    if ( configuration().handleTransferSizeOption)
    {
      reqOptions.transferSizeOption( 0U);
    }

    // send read request packet
    sendFirst(
      Packets::ReadRequestPacket{ filename(), mode(), reqOptions.options()});

    // wait for answers
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
  // inform base class
  OperationImpl::finished( status, std::move( errorInfo));

  // Inform data handler
  dataHandler->finished();
}

void ReadRequestOperationImpl::dataPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::DataPacket &dataPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
    << "RX: " << static_cast< std::string>( dataPacket);

  // check retransmission of last packet
  if (dataPacket.blockNumber() == lastReceivedBlockNumber)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
      << "Received last data package again. Re-ACK them";

    // Retransmit last ACK packet
    send( Packets::AcknowledgementPacket( lastReceivedBlockNumber));

    return;
  }

  // check unexpected block number
  if (dataPacket.blockNumber() != lastReceivedBlockNumber.next())
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
      << "Wrong Data packet block number";

    // send error packet
    using namespace std::literals::string_view_literals;
    Packets::ErrorPacket errorPacket(
      ErrorCode::IllegalTftpOperation,
      "Block Number not expected"sv);
    send( errorPacket);

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket));
    return;
  }

  // check for too much data
  if (dataPacket.dataSize() > receiveDataSize)
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
      << "Too much data received";

    // send error packet
    using namespace std::literals::string_view_literals;
    Packets::ErrorPacket errorPacket(
      ErrorCode::IllegalTftpOperation,
      "Too much data"sv);
    send( errorPacket);

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket));
    return;
  }

  // call call-back
  dataHandler->receivedData( dataPacket.data());

  // increment received block number
  lastReceivedBlockNumber++;

  // send ACK
  send( Packets::AcknowledgementPacket( lastReceivedBlockNumber));

  // if received data size is smaller then the expected
  if (dataPacket.dataSize() < receiveDataSize)
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

void ReadRequestOperationImpl::acknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::AcknowledgementPacket &acknowledgementPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
    << "RX ERROR: " << static_cast< std::string>( acknowledgementPacket);

  // send Error
  using namespace std::literals::string_view_literals;
  Packets::ErrorPacket errorPacket(
    ErrorCode::IllegalTftpOperation,
    "ACK not expected"sv);

  send( errorPacket);

  // Operation completed
  finished( TransferStatus::TransferError, std::move( errorPacket));
}

void ReadRequestOperationImpl::optionsAcknowledgementPacket(
  const boost::asio::ip::udp::endpoint &,
  const Packets::OptionsAcknowledgementPacket &optionsAcknowledgementPacket)
{
  BOOST_LOG_FUNCTION()

  BOOST_LOG_SEV( TftpLogger::get(), severity_level::info)
    << "RX ERROR: " << static_cast< std::string>( optionsAcknowledgementPacket);

  const auto &remoteOptions{ optionsAcknowledgementPacket.options()};

  // check empty options
  if (remoteOptions.empty())
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
      << "Received option list is empty";

    using namespace std::literals::string_view_literals;
    Packets::ErrorPacket errorPacket(
      ErrorCode::IllegalTftpOperation,
      "Empty OACK not allowed"sv);

    send( errorPacket);

    // Operation completed
    finished( TransferStatus::TransferError, std::move( errorPacket));
    return;
  }

  // perform option negotiation
  const auto negotiatedOptions{ options().negotiateClient( remoteOptions)};

  // Check empty options list
  if (negotiatedOptions.empty())
  {
    BOOST_LOG_SEV( TftpLogger::get(), severity_level::error)
      << "Option negotiation failed";

    using namespace std::literals::string_view_literals;
    Packets::ErrorPacket errorPacket(
      ErrorCode::TftpOptionRefused,
      "Option negotiation failed"sv);

    send( errorPacket);

    // Operation completed
    finished( TransferStatus::OptionNegotiationError, std::move( errorPacket));
    return;
  }

  // check blocksize option
  if ( auto blocksizeOption{ negotiatedOptions.blocksize()}; blocksizeOption)
  {
    receiveDataSize = *blocksizeOption;

    // set maximum receive data size if necessary
    if (receiveDataSize > DefaultDataSize)
    {
      maxReceivePacketSize(
        receiveDataSize + DefaultTftpDataPacketHeaderSize);
    }
  }

  // check timeout option
  if ( auto timeoutOption{ negotiatedOptions.timeoutOption()}; timeoutOption)
  {
    receiveTimeout( *timeoutOption);
  }

  // check transfer size option
  if (auto transferSizeOption{ negotiatedOptions.transferSizeOption()}; transferSizeOption)
  {
    if ( !dataHandler->receivedTransferSize( *transferSizeOption))
    {
      using namespace std::literals::string_view_literals;
      Packets::ErrorPacket errorPacket(
        ErrorCode::DiskFullOrAllocationExceeds,
        "FILE TO BIG"sv);

      send( errorPacket);

      // Operation completed
      finished( TransferStatus::TransferError, std::move( errorPacket));
      return;
    }
  }

  // send Acknowledgment with block number set to 0
  send( Packets::AcknowledgementPacket{ Packets::BlockNumber{ 0}});

  // receive next packet
  receive();
}

}
