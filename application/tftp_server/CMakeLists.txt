# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# 
# Thomas Vogt, Thomas@Thomas-Vogt.de

cmake_minimum_required( VERSION 3.11)

find_package(
  Boost
  REQUIRED
  COMPONENTS
    thread)

add_executable(
  tftp_server
  
  tftp_server.cpp
  TftpServerApplication.cpp)

target_link_libraries( 
  tftp_server 

  tftp 
  helper 

  $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>
  $<$<AND:$<CXX_COMPILER_ID:Clang>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,7.0>>:stdc++fs>

  Boost::thread)

# INSTALL
include( GNUInstallDirs)

install(
  TARGETS tftp_server
  COMPONENT runtime
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
