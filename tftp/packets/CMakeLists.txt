# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Thomas Vogt, thomas@thomas-vogt.de

cmake_minimum_required( VERSION 3.13)

target_sources(
  tftp

  PRIVATE
    AcknowledgementPacket.hpp
    AcknowledgementPacket.cpp
    BlockNumber.hpp
    BlockNumber.cpp
    DataPacket.hpp
    DataPacket.cpp
    ErrorPacket.hpp
    ErrorPacket.cpp
    OptionsAcknowledgementPacket.hpp
    OptionsAcknowledgementPacket.cpp
    Packet.hpp
    Packet.cpp
    PacketException.hpp
    Packets.hpp
    ReadRequestPacket.hpp
    ReadRequestPacket.cpp
    ReadWriteRequestPacket.hpp
    ReadWriteRequestPacket.cpp
    WriteRequestPacket.hpp
    WriteRequestPacket.cpp)

target_sources(
  tftp_test

  PRIVATE
    AcknowledgementPacketTest.cpp
    BlockNumberTest.cpp
    DataPacketTest.cpp
    ErrorPacketTest.cpp
    OptionsAcknowledgementPacketTest.cpp
    ReadRequestPacketTest.cpp
    ReadWriteRequestPacketTest.cpp
    WriteRequestPacketTest.cpp)
