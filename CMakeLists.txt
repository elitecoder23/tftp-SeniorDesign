# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Thomas Vogt, thomas@thomas-vogt.de

cmake_minimum_required( VERSION 3.26 )

project(
  tftp
  VERSION 0.0.0.0
  DESCRIPTION "TFTP Protocol Library and Applications"
  HOMEPAGE_URL "https://git.thomas-vogt.de/thomas-vogt/tftp" )

set( PROJECT_VERSION_ADDITION "dev" )

find_package( Git )

if( GIT_FOUND )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --verify --short HEAD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE )

  set( PROJECT_VERSION_ADDITION "${PROJECT_VERSION_ADDITION}-${GIT_HASH}" )
endif()


set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

# find clang-tidy and add options to execute
find_program( CLANG_TIDY_EXE NAMES clang-tidy )

# Workaround, when CXX standard option differs for clang-tidy and g++
if( CLANG_TIDY_EXE AND ${CMAKE_CXX_COMPILER_ID} STREQUAL Clang )
  set( CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE} )
endif()

# find iwyu and add options to execute
find_program( CLANG_IWYU_EXE NAMES iwyu )

if( CLANG_IWYU_EXE )
  #set( CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${CLANG_IWYU_EXE} )
endif()

enable_testing()

include( GNUInstallDirs )

# Activate Dynamic Libraries for Boost on Windows Platform
if( WIN32 )
  set( Boost_USE_STATIC_LIBS OFF )
endif()

include( FetchContent )

FetchContent_Declare(
  cmake_common
  GIT_REPOSITORY https://git.thomas-vogt.de/thomas-vogt/cmake_common.git
  GIT_TAG        main )

if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/cmake_common )
  set( FETCHCONTENT_SOURCE_DIR_CMAKE_COMMON ${CMAKE_SOURCE_DIR}/cmake_common )
endif()

FetchContent_GetProperties( cmake_common )
if( NOT cmake_common_POPULATED )
  FetchContent_Populate( cmake_common )
  # Add own CMake scripts to module path
  list( APPEND CMAKE_MODULE_PATH "${cmake_common_SOURCE_DIR}" )
endif()


FetchContent_Declare(
  helper
  GIT_REPOSITORY https://git.thomas-vogt.de/thomas-vogt/helper.git
  GIT_TAG        main )

if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/helper )
  set( FETCHCONTENT_SOURCE_DIR_HELPER ${CMAKE_SOURCE_DIR}/helper )
endif()

FetchContent_MakeAvailable( helper )

add_subdirectory( lib )
add_subdirectory( app )
add_subdirectory( doc )


install(
  FILES LICENSE
  RENAME ${PROJECT_NAME}.LICENSE
  TYPE DOC
  COMPONENT runtime )

if( PROJECT_IS_TOP_LEVEL )
  set( CPACK_GENERATOR ZIP )
  set( CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION} )
  set( CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} )
  set( CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR} )
  set( CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH} )
  include( CPack )
endif()
