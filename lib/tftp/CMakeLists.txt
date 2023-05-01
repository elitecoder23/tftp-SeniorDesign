# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Thomas Vogt, thomas@thomas-vogt.de

cmake_minimum_required( VERSION 3.25 )

find_package( Boost REQUIRED COMPONENTS program_options )
find_package( fmt REQUIRED )

# prepare Version.cpp
configure_file( Version.cpp.in Version.cpp )

add_library( tftp )

include( GenerateExportHeader )
generate_export_header( tftp )

target_sources(
  tftp

  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}/..
      FILES
        DataHandler.hpp
        ReceiveDataHandler.hpp
        RequestTypeDescription.hpp
        Tftp.hpp
        TftpConfiguration.hpp
        TftpException.hpp
        TftpLogger.hpp
        TftpOptionsConfiguration.hpp
        TransferStatusDescription.hpp
        TransmitDataHandler.hpp
        Version.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/tftp_export.h

  PRIVATE
    RequestTypeDescription.cpp
    TftpConfiguration.cpp
    TftpOptionsConfiguration.cpp
    TransferStatusDescription.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/Version.cpp )

target_compile_features( tftp PUBLIC cxx_std_20 )

target_compile_definitions(
  tftp

  PRIVATE
    # Disable C++17 deprecation warning for BOOST in MSVC (Version 1.67)
    $<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING> )

target_compile_options(
  tftp

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:-W3>

    # remove absolute paths from __FILE__ macro
    #$<$<CXX_COMPILER_ID:GNU>:-fmacro-prefix-map=${PROJECT_SOURCE_DIR}/=>
    # warning level
    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>

    # remove absolute paths from __FILE__ macro
    #$<$<CXX_COMPILER_ID:Clang>:-fmacro-prefix-map=${PROJECT_SOURCE_DIR}/=>
    # warning level
    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra> )

target_link_libraries(
  tftp

  PUBLIC
    helper
    Boost::program_options

  PRIVATE
    fmt::fmt
    # Windows Socket must be explicitly linked in MinGW
    $<$<PLATFORM_ID:Windows>:wsock32>
    $<$<PLATFORM_ID:Windows>:ws2_32> )

add_library( tftp_test OBJECT )

target_sources(
  tftp_test

  PRIVATE
    test/TftpOptionsConfigurationTest.cpp
    test/VersionTest.cpp )

target_compile_features( tftp_test PUBLIC cxx_std_20 )

target_compile_definitions(
  tftp_test

  PRIVATE
    # Disable C++17 deprecation warning for BOOST in MSVC (Version 1.67)
    $<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING> )

target_compile_options(
  tftp_test

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:-W3>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra> )

target_link_libraries( tftp_test PUBLIC tftp )


add_subdirectory( file )
add_subdirectory( packets )
add_subdirectory( client )
add_subdirectory( server )

set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS ${CMAKE_CURRENT_SOURCE_DIR} )


install(
  TARGETS tftp
  RUNTIME_DEPENDENCY_SET tftp-runtime-deps
  EXPORT tftp-config
  COMPONENT runtime
  ARCHIVE COMPONENT development
  FILE_SET HEADERS COMPONENT development )

install(
  EXPORT tftp-config
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  COMPONENT development )
