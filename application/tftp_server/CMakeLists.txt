# $Date$
# $Revision$
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# 
# Thomas Vogt, Thomas@Thomas-Vogt.de

cmake_minimum_required( VERSION 3.7)

project( "TFTP Server")

find_package( 
  Boost
  REQUIRED
  COMPONENTS
  program_options
  system
  log_setup
  log
  filesystem
  thread
  chrono)

add_executable(
  tftp_server
  
  tftp_server.cpp
  TftpServerApplication.cpp)

target_link_libraries( 
  tftp_server 
  
  tftp 
  helper 
  boost_application
  
  ${Boost_LIBRARIES}
  ${BOOST_THREAD_LIBRARY})
