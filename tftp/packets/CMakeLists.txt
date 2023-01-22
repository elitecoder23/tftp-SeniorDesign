# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Thomas Vogt, thomas@thomas-vogt.de

cmake_minimum_required( VERSION 3.24 )

target_sources(
  tftp

  PRIVATE
    AcknowledgementPacket.hpp
    AcknowledgementPacket.cpp
    BlockNumber.hpp
    BlockNumber.cpp
    DataPacket.hpp
    DataPacket.cpp
    ErrorCodeDescription.hpp
    ErrorCodeDescription.cpp
    ErrorPacket.hpp
    ErrorPacket.cpp
    Options.hpp
    Options.ipp
    Options.cpp
    OptionsAcknowledgementPacket.hpp
    OptionsAcknowledgementPacket.cpp
    Packet.hpp
    Packet.cpp
    PacketException.hpp
    PacketHandler.hpp
    PacketHandler.cpp
    Packets.hpp
    PacketStatistic.hpp
    PacketStatistic.cpp
    PacketTypeDescription.hpp
    PacketTypeDescription.cpp
    ReadRequestPacket.hpp
    ReadRequestPacket.cpp
    ReadWriteRequestPacket.hpp
    ReadWriteRequestPacket.cpp
    TftpOptions.hpp
    TftpOptions.cpp
    WriteRequestPacket.hpp
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
    test/ReadRequestPacketTest.cpp
    test/ReadWriteRequestPacketTest.cpp
    test/TftpOptionsTest.cpp
    test/WriteRequestPacketTest.cpp )
