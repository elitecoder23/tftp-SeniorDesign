# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Thomas Vogt, thomas@thomas-vogt.de

cmake_minimum_required( VERSION 3.18 )

target_sources(
  tftp

  PRIVATE
    Operation.hpp
    Server.hpp
    TftpServer.hpp
    TftpServer.cpp
    implementation/OperationImpl.hpp
    implementation/OperationImpl.cpp
    implementation/ReadRequestOperationImpl.hpp
    implementation/ReadRequestOperationImpl.cpp
    implementation/TftpServerImpl.hpp
    implementation/TftpServerImpl.cpp
    implementation/WriteRequestOperationImpl.hpp
    implementation/WriteRequestOperationImpl.cpp)

#target_sources( tftp_test PRIVATE)
