# $Date$
# $Revision$
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# 
# Thomas Vogt, Thomas@Thomas-Vogt.de

cmake_minimum_required( VERSION 3.7)

project ( "TFTP library")

find_package( Boost REQUIRED COMPONENTS program_options)

add_library(
  tftp
  
  ErrorCodeDescription.hpp
  ErrorCodeDescription.cpp
  OperationHandler.hpp
  PacketHandler.hpp
  PacketHandler.cpp
  ReceiveDataOperationHandler.hpp
  RequestTypeDescription.hpp
  RequestTypeDescription.cpp
  Tftp.hpp
  TftpConfiguration.hpp
  TftpConfiguration.cpp
  TftpException.hpp
  TftpLogger.hpp
  TransmitDataOperationHandler.hpp
  
  packets/AcknowledgementPacket.hpp
  packets/AcknowledgementPacket.cpp
  packets/BaseErrorPacket.hpp
  packets/BaseErrorPacket.cpp
  packets/BlockNumber.hpp
  packets/BlockNumber.cpp
  packets/DataPacket.hpp
  packets/DataPacket.cpp
  packets/ErrorPacket.hpp
  packets/ErrorPacket.cpp
  packets/OptionsAcknowledgementPacket.hpp
  packets/OptionsAcknowledgementPacket.cpp
  packets/Packet.hpp
  packets/Packet.cpp
  packets/PacketException.hpp
  packets/PacketFactory.hpp
  packets/PacketFactory.cpp
  packets/Packets.hpp
  packets/ReadRequestPacket.hpp
  packets/ReadRequestPacket.cpp
  packets/ReadWriteRequestPacket.hpp
  packets/ReadWriteRequestPacket.cpp
  packets/WriteRequestPacket.hpp
  packets/WriteRequestPacket.cpp
  
  options/IntegerOption.hpp
  options/Option.hpp
  options/Option.cpp
  options/OptionList.hpp
  options/OptionList.cpp
  options/Options.hpp
  options/StringOption.hpp
  options/StringOption.cpp

  file/File.hpp
  file/TftpFile.hpp
  file/MemoryFile.hpp
  file/MemoryFile.cpp
  file/StreamFile.hpp
  file/StreamFile.ipp
  file/NullSinkFile.hpp
  file/NullSinkFile.hpp
  
  client/Client.hpp
  client/Operation.hpp  
  client/TftpClient.hpp
  client/TftpClient.cpp
  client/implementation/OperationImpl.hpp
  client/implementation/OperationImpl.cpp
  client/implementation/ReadRequestOperationImpl.hpp
  client/implementation/ReadRequestOperationImpl.cpp
  client/implementation/TftpClientImpl.hpp
  client/implementation/TftpClientImpl.cpp
  client/implementation/TftpClientInternal.hpp
  client/implementation/WriteRequestOperationImpl.hpp
  client/implementation/WriteRequestOperationImpl.cpp

  server/Operation.hpp
  server/Server.hpp
  server/TftpServer.hpp
  server/TftpServer.cpp
  server/implementation/BaseErrorOperation.hpp
  server/implementation/BaseErrorOperation.cpp
  server/implementation/ErrorOperation.hpp
  server/implementation/ErrorOperation.cpp
  server/implementation/OperationImpl.hpp
  server/implementation/OperationImpl.cpp
  server/implementation/ReadRequestOperationImpl.hpp
  server/implementation/ReadRequestOperationImpl.cpp
  server/implementation/TftpServerImpl.hpp
  server/implementation/TftpServerImpl.cpp
  server/implementation/TftpServerInternal.hpp
  server/implementation/WriteRequestOperationImpl.hpp
  server/implementation/WriteRequestOperationImpl.cpp
  )

target_include_directories( tftp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries( tftp helper Boost::boost Boost::program_options)

# Add winsock for windows targets
if(WIN32)
  target_link_libraries( tftp wsock32 ws2_32)
endif()

add_library( 
  tftp_test 
  OBJECT
  
  options/IntegerOptionTest.cpp
  options/OptionListTest.cpp
  options/StringOptionTest.cpp
  
  packets/AcknowledgementPacketTest.cpp
  packets/BaseErrorPacketTest.cpp
  packets/BlockNumberTest.cpp
  packets/DataPacketTest.cpp
  packets/ErrorPacketTest.cpp
  packets/OptionsAcknowledgementPacketTest.cpp
  packets/ReadRequestPacketTest.cpp
  packets/WriteRequestPacketTest.cpp)

target_include_directories( tftp_test PRIVATE $<TARGET_PROPERTY:tftp,INCLUDE_DIRECTORIES>)

add_documentation( TARGET tftp)
