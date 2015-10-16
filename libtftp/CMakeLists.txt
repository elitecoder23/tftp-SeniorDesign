# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# 
# $Date$
# $Revision$
# Thomas Vogt, Thomas@Thomas-Vogt.de

cmake_minimum_required( VERSION 3.3)

project ( "TFTP library")

find_package( Boost REQUIRED)

add_library(
	tftp
	
	tftp/TftpPacketHandler.cpp
    tftp/TftpConfiguration.cpp
    
	tftp/packet/AcknowledgementPacket.cpp
	tftp/packet/BaseErrorPacket.cpp
	tftp/packet/BlockNumber.cpp
	tftp/packet/DataPacket.cpp
	tftp/packet/ErrorPacket.cpp
	tftp/packet/OptionsAcknowledgementPacket.cpp
	tftp/packet/PacketFactory.cpp
	tftp/packet/ReadRequestPacket.cpp
	tftp/packet/ReadWriteRequestPacket.cpp
	tftp/packet/test/BlockNumberTest.cpp
	tftp/packet/test/PacketTest.cpp
	tftp/packet/TftpPacket.cpp
	tftp/packet/WriteRequestPacket.cpp
	
	tftp/options/Option.cpp
	tftp/options/OptionList.cpp
	tftp/options/StringOption.cpp
	tftp/options/test/IntegerOptionNegotiationEntryTest.cpp
	tftp/options/test/OptionNegotiationTest.cpp
	
	tftp/file/MemoryFile.cpp
	tftp/file/StreamFile.cpp
	
	tftp/client/TftpClient.cpp
	tftp/client/implementation/TftpClientImpl.cpp
	tftp/client/implementation/TftpClientOperationImpl.cpp
	tftp/client/implementation/TftpClientReadRequestOperationImpl.cpp
	tftp/client/implementation/TftpClientWriteRequestOperationImpl.cpp
	
	tftp/server/TftpServer.cpp
	tftp/server/implementation/TftpServerImpl.cpp
	tftp/server/implementation/TftpServerBaseErrorOperation.cpp
	tftp/server/implementation/TftpServerErrorOperation.cpp
	tftp/server/implementation/TftpServerOperationImpl.cpp
	tftp/server/implementation/TftpServerReadRequestOperationImpl.cpp
	tftp/server/implementation/TftpServerWriteRequestOperationImpl.cpp
	)

target_include_directories( tftp SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories( tftp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries( tftp helper)

set( 
	DOXYGEN_TAGFILES 
	"libhelper.TAGFILE=../../libhelper/html")
	
GENERATE_DOCUMENTATION()
