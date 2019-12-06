# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Thomas Vogt, thomas@thomas-vogt.de

cmake_minimum_required( VERSION 3.15)

find_package( Boost REQUIRED COMPONENTS program_options)

# prepare Version.cpp
configure_file(
  Version.cpp.in
  Version.cpp)


add_library( tftp)

target_sources(
  tftp

  PRIVATE
    DataHandler.hpp
    ErrorCodeDescription.hpp
    ErrorCodeDescription.cpp
    PacketHandler.hpp
    PacketHandler.cpp
    ReceiveDataHandler.hpp
    RequestTypeDescription.hpp
    RequestTypeDescription.cpp
    Tftp.hpp
    TftpConfiguration.hpp
    TftpConfiguration.cpp
    TftpException.hpp
    TftpLogger.hpp
    TransmitDataHandler.hpp
    Version.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/Version.cpp)

target_include_directories(
  tftp
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)

target_compile_options(
  tftp

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:-W3>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>)

target_compile_definitions(
  tftp

  PRIVATE
    # Disable C++17 deprecation warning for BOOST in MSVC (Version 1.67)
    $<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING>)

target_link_libraries( tftp helper Boost::program_options)

target_compile_features( tftp PUBLIC cxx_std_17)


add_library( tftp_test OBJECT)

target_sources(
  tftp_test

  PRIVATE
    VersionTest.cpp)

target_compile_options(
  tftp_test

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:-W3>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Werror>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Werror>)

target_compile_definitions(
  tftp_test

  PRIVATE
    # Disable C++17 deprecation warning for BOOST in MSVC (Version 1.67)
    $<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING>)

target_compile_features( tftp_test PUBLIC cxx_std_17)

target_link_libraries( tftp_test PUBLIC tftp)


add_subdirectory( file)
add_subdirectory( packets)
add_subdirectory( options)
add_subdirectory( client)
add_subdirectory( server)

# INSTALL
include( GNUInstallDirs)

install(
  TARGETS tftp
  COMPONENT runtime
  EXPORT tftp-config
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMPONENT development
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
  PATTERN "*.hpp"
  PATTERN "*Test.*" EXCLUDE
  PATTERN "implementation" EXCLUDE)

install(
  EXPORT tftp-config
  COMPONENT development
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)
