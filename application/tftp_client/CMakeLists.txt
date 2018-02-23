# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# 
# Thomas Vogt, Thomas@Thomas-Vogt.de

cmake_minimum_required( VERSION 3.10)

find_package( 
  Boost
  REQUIRED
  COMPONENTS
  program_options
  filesystem
  thread
  chrono)

add_executable(
  tftp_client
  
  tftp_client.cpp
  TftpClientApplication.cpp)

target_link_libraries( 
  tftp_client
  
  tftp
  helper
  
  Boost::boost
  Boost::program_options
  Boost::filesystem
  Boost::thread
  Boost::chrono)

# INSTALL
include( GNUInstallDirs)

install(
  TARGETS tftp_client
  COMPONENT runtime
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
