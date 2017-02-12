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
  
  tftp/OperationHandler.hpp
  tftp/PacketHandler.hpp
  tftp/PacketHandler.cpp
  tftp/ReceiveDataOperationHandler.hpp
  tftp/RequestTypeDescription.hpp
  tftp/RequestTypeDescription.cpp
  tftp/Tftp.hpp
  tftp/TftpConfiguration.hpp
  tftp/TftpConfiguration.cpp
  tftp/TftpException.hpp
  tftp/TransmitDataOperationHandler.hpp
  
  tftp/packets/AcknowledgementPacket.hpp
  tftp/packets/AcknowledgementPacket.cpp
  tftp/packets/BaseErrorPacket.hpp
  tftp/packets/BaseErrorPacket.cpp
  tftp/packets/BlockNumber.hpp
  tftp/packets/BlockNumber.cpp
  tftp/packets/DataPacket.hpp
  tftp/packets/DataPacket.cpp
  tftp/packets/ErrorPacket.hpp
  tftp/packets/ErrorPacket.cpp
  tftp/packets/OptionsAcknowledgementPacket.hpp
  tftp/packets/OptionsAcknowledgementPacket.cpp
  tftp/packets/Packet.hpp
  tftp/packets/Packet.cpp
  tftp/packets/PacketException.hpp
  tftp/packets/PacketFactory.hpp
  tftp/packets/PacketFactory.cpp
  tftp/packets/Packets.hpp
  tftp/packets/ReadRequestPacket.hpp
  tftp/packets/ReadRequestPacket.cpp
  tftp/packets/ReadWriteRequestPacket.hpp
  tftp/packets/ReadWriteRequestPacket.cpp
  tftp/packets/WriteRequestPacket.hpp
  tftp/packets/WriteRequestPacket.cpp
  
  tftp/options/Option.cpp
  tftp/options/OptionList.cpp
  tftp/options/StringOption.cpp
  
  tftp/file/File.hpp
  tftp/file/TftpFile.hpp
  tftp/file/MemoryFile.hpp
  tftp/file/MemoryFile.cpp
  tftp/file/StreamFile.hpp
  tftp/file/StreamFile.ipp
  tftp/file/NullSinkFile.hpp
  tftp/file/NullSinkFile.hpp
  
  tftp/client/Client.hpp
  tftp/client/TftpClient.hpp
  tftp/client/TftpClient.cpp
  tftp/client/TftpClientOperation.hpp  
  tftp/client/implementation/TftpClientImpl.hpp
  tftp/client/implementation/TftpClientImpl.cpp
  tftp/client/implementation/TftpClientInternal.hpp
  tftp/client/implementation/TftpClientOperationImpl.hpp
  tftp/client/implementation/TftpClientOperationImpl.cpp
  tftp/client/implementation/TftpClientReadRequestOperationImpl.hpp
  tftp/client/implementation/TftpClientReadRequestOperationImpl.cpp
  tftp/client/implementation/TftpClientWriteRequestOperationImpl.hpp
  tftp/client/implementation/TftpClientWriteRequestOperationImpl.cpp

  tftp/server/Operation.hpp
  tftp/server/Server.hpp
  tftp/server/TftpServer.hpp
  tftp/server/TftpServer.cpp
  tftp/server/implementation/BaseErrorOperation.hpp
  tftp/server/implementation/BaseErrorOperation.cpp
  tftp/server/implementation/ErrorOperation.hpp
  tftp/server/implementation/ErrorOperation.cpp
  tftp/server/implementation/OperationImpl.hpp
  tftp/server/implementation/OperationImpl.cpp
  tftp/server/implementation/ReadRequestOperationImpl.hpp
  tftp/server/implementation/ReadRequestOperationImpl.cpp
  tftp/server/implementation/TftpServerImpl.hpp
  tftp/server/implementation/TftpServerImpl.cpp
  tftp/server/implementation/TftpServerInternal.hpp
  tftp/server/implementation/WriteRequestOperationImpl.hpp
  tftp/server/implementation/WriteRequestOperationImpl.cpp
  )

target_include_directories( tftp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries( tftp helper ${Boost_LIBRARIES})

# Add winsock for windows targets
if(WIN32)
  target_link_libraries( tftp wsock32 ws2_32)
endif()

add_library( 
  tftp_test 
  OBJECT
  
  tftp/options/IntegerOptionTest.cpp
  tftp/options/OptionListTest.cpp
  tftp/options/StringOptionTest.cpp
  
  tftp/packets/AcknowledgementPacketTest.cpp
  tftp/packets/BaseErrorPacketTest.cpp
  tftp/packets/BlockNumberTest.cpp
  tftp/packets/DataPacketTest.cpp
  tftp/packets/ErrorPacketTest.cpp
  tftp/packets/OptionsAcknowledgementPacketTest.cpp
  tftp/packets/ReadRequestPacketTest.cpp
  tftp/packets/WriteRequestPacketTest.cpp)

target_include_directories( tftp_test PRIVATE $<TARGET_PROPERTY:tftp,INCLUDE_DIRECTORIES>)

set( 
  DOXYGEN_TAGFILES 
  "libhelper.TAGFILE=../../libhelper/html")

GENERATE_DOCUMENTATION()
