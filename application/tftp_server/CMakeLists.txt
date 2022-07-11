# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Thomas Vogt, thomas@thomas-vogt.de

cmake_minimum_required( VERSION 3.23 )

find_package(
  Boost
  REQUIRED
  COMPONENTS
    thread )

add_executable( tftp_server )

target_sources( tftp_server PRIVATE tftp_server.cpp )

target_link_libraries(
  tftp_server

  PRIVATE
    tftp
    helper
    Boost::thread
    # Windows Socket must be explicitly linked in MinGW
    $<$<PLATFORM_ID:Windows>:wsock32 ws2_32> )

target_compile_definitions(
  tftp_server

  PRIVATE
    # Disable C++17 deprecation warning for BOOST in MSVC (Version 1.67)
    $<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING> )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS ${CMAKE_CURRENT_SOURCE_DIR} )

install( TARGETS tftp_server COMPONENT runtime )
