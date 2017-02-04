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
  
  tftp/packet/AcknowledgementPacket.hpp
  tftp/packet/AcknowledgementPacket.cpp
  tftp/packet/BaseErrorPacket.hpp
  tftp/packet/BaseErrorPacket.cpp
  tftp/packet/BlockNumber.hpp
  tftp/packet/BlockNumber.cpp
  tftp/packet/DataPacket.hpp
  tftp/packet/DataPacket.cpp
  tftp/packet/ErrorPacket.hpp
  tftp/packet/ErrorPacket.cpp
  tftp/packet/OptionsAcknowledgementPacket.hpp
  tftp/packet/OptionsAcknowledgementPacket.cpp
  tftp/packet/PacketFactory.hpp
  tftp/packet/PacketFactory.cpp
  tftp/packet/ReadRequestPacket.hpp
  tftp/packet/ReadRequestPacket.cpp
  tftp/packet/ReadWriteRequestPacket.hpp
  tftp/packet/ReadWriteRequestPacket.cpp
  tftp/packet/TftpPacket.hpp
  tftp/packet/TftpPacket.cpp
  tftp/packet/WriteRequestPacket.hpp
  tftp/packet/WriteRequestPacket.cpp
  
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

  tftp/server/Server.hpp
  tftp/server/TftpServer.hpp
  tftp/server/TftpServer.cpp
  tftp/server/TftpServerOperation.hpp
  tftp/server/implementation/TftpServerImpl.hpp
  tftp/server/implementation/TftpServerImpl.cpp
  tftp/server/implementation/TftpServerBaseErrorOperation.hpp
  tftp/server/implementation/TftpServerBaseErrorOperation.cpp
  tftp/server/implementation/TftpServerErrorOperation.hpp
  tftp/server/implementation/TftpServerErrorOperation.cpp
  tftp/server/implementation/TftpServerInternal.hpp
  tftp/server/implementation/TftpServerOperationImpl.hpp
  tftp/server/implementation/TftpServerOperationImpl.cpp
  tftp/server/implementation/TftpServerReadRequestOperationImpl.hpp
  tftp/server/implementation/TftpServerReadRequestOperationImpl.cpp
  tftp/server/implementation/TftpServerWriteRequestOperationImpl.hpp
  tftp/server/implementation/TftpServerWriteRequestOperationImpl.cpp
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
  
  tftp/packet/AcknowledgementPacketTest.cpp
  tftp/packet/BaseErrorPacketTest.cpp
  tftp/packet/BlockNumberTest.cpp
  tftp/packet/DataPacketTest.cpp
  tftp/packet/ErrorPacketTest.cpp
  tftp/packet/OptionsAcknowledgementPacketTest.cpp
  tftp/packet/ReadRequestPacketTest.cpp
  tftp/packet/WriteRequestPacketTest.cpp)

target_include_directories( tftp_test PRIVATE $<TARGET_PROPERTY:tftp,INCLUDE_DIRECTORIES>)

set( 
  DOXYGEN_TAGFILES 
  "libhelper.TAGFILE=../../libhelper/html")

GENERATE_DOCUMENTATION()
