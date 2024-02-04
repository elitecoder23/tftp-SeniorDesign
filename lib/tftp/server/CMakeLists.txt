# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Thomas Vogt, thomas@thomas-vogt.de

cmake_minimum_required( VERSION 3.28 )

target_sources(
  tftp

  PUBLIC
    FILE_SET HEADERS
      FILES
        Operation.hpp
        ReadOperationConfiguration.hpp
        Server.hpp
        ServerConfiguration.hpp
        TftpServer.hpp
        WriteOperationConfiguration.hpp

  PRIVATE
    ReadOperationConfiguration.cpp
    ServerConfiguration.cpp
    TftpServer.cpp
    WriteOperationConfiguration.cpp

    implementation/OperationImpl.hpp
    implementation/OperationImpl.cpp
    implementation/ReadOperationImpl.hpp
    implementation/ReadOperationImpl.cpp
    implementation/TftpServerImpl.hpp
    implementation/TftpServerImpl.cpp
    implementation/WriteOperationImpl.hpp
    implementation/WriteOperationImpl.cpp )

#target_sources( tftp_test PRIVATE)
