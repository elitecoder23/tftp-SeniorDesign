# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.20 )

target_sources(
  tftp

  PUBLIC
    FILE_SET HEADERS
      FILES
        AcknowledgementPacket.hpp
        BlockNumber.hpp
        DataPacket.hpp
        ErrorCodeDescription.hpp
        ErrorPacket.hpp
        Options.hpp
        Options.ipp
        OptionsAcknowledgementPacket.hpp
        Packet.hpp
        PacketException.hpp
        PacketHandler.hpp
        Packets.hpp
        PacketStatistic.hpp
        PacketTypeDescription.hpp
        ReadRequestPacket.hpp
        ReadWriteRequestPacket.hpp
        TftpOptions.hpp
        WriteRequestPacket.hpp

  PRIVATE
    AcknowledgementPacket.cpp
    BlockNumber.cpp
    DataPacket.cpp
    ErrorCodeDescription.cpp
    ErrorPacket.cpp
    Options.cpp
    OptionsAcknowledgementPacket.cpp
    Packet.cpp
    PacketHandler.cpp
    PacketStatistic.cpp
    PacketTypeDescription.cpp
    ReadRequestPacket.cpp
    ReadWriteRequestPacket.cpp
    TftpOptions.cpp
    WriteRequestPacket.cpp )

target_sources(
  tftp_test

  PRIVATE
    test/AcknowledgementPacketTest.cpp
    test/BlockNumberTest.cpp
    test/DataPacketTest.cpp
    test/ErrorPacketTest.cpp
    test/OptionsAcknowledgementPacketTest.cpp
    test/OptionsTest.cpp
    test/PacketTest.cpp
    test/ReadRequestPacketTest.cpp
    test/ReadWriteRequestPacketTest.cpp
    test/TftpOptionsTest.cpp
    test/WriteRequestPacketTest.cpp )
