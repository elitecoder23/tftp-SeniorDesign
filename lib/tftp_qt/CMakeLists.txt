# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.31 )

find_package( Qt6 COMPONENTS Core QUIET )

if( NOT TARGET Qt::Core )
  return()
endif()

set( CMAKE_AUTOMOC ON )
#set( CMAKE_AUTOUIC ON )
#set( CMAKE_AUTORCC ON )

add_library( tftp_qt )

include( GenerateExportHeader )
generate_export_header( tftp_qt )

target_sources(
  tftp_qt

  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}/..

      FILES
        TftpQt.hpp
        PacketStatisticModel.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/tftp_qt_export.h

  PRIVATE
    PacketStatisticModel.cpp )

target_compile_features( tftp_qt PUBLIC cxx_std_23 )

target_compile_options(
  tftp_qt

  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    #$<$<CXX_COMPILER_ID:MSVC>:/Wall>
    # Disable Warning for exporting classes with std::* private members
    $<$<CXX_COMPILER_ID:MSVC>:/wd4251>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic> )

target_link_libraries(
  tftp_qt

  PUBLIC
    Qt::Core
    tftp

  PRIVATE
    helper_qt )

# disable clang tidy in binary dir by placing dummy .clang-tidy file
file(
  WRITE
  ${CMAKE_CURRENT_BINARY_DIR}/.clang-tidy
  "Checks: '-*,llvm-twine-local'" )


set_property(
  DIRECTORY ${PROJECT_SOURCE_DIR}
  APPEND
  PROPERTY DOC_PATHS ${CMAKE_CURRENT_SOURCE_DIR} )


install(
  TARGETS tftp_qt
  RUNTIME_DEPENDENCY_SET tftp-runtime-deps
  EXPORT tftp_qt-config
  COMPONENT runtime
  ARCHIVE COMPONENT development
  FILE_SET HEADERS COMPONENT development )

install(
  EXPORT tftp_qt-config
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  COMPONENT development )
