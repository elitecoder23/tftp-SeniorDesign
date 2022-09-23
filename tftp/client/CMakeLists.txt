# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Thomas Vogt, thomas@thomas-vogt.de

cmake_minimum_required( VERSION 3.24 )

target_sources(
  tftp

  PRIVATE
    Client.hpp
    Operation.hpp
    TftpClient.hpp
    TftpClient.cpp
    implementation/OperationImpl.hpp
    implementation/OperationImpl.cpp
    implementation/ReadOperationImpl.hpp
    implementation/ReadOperationImpl.cpp
    implementation/TftpClientImpl.hpp
    implementation/TftpClientImpl.cpp
    implementation/WriteOperationImpl.hpp
    implementation/WriteOperationImpl.cpp )

#target_sources( tftp_test PRIVATE)
