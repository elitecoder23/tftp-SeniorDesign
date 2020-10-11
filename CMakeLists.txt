# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Thomas Vogt, thomas@thomas-vogt.de

cmake_minimum_required( VERSION 3.18 )

project(
  tftp
  VERSION 1.0.0.0
  DESCRIPTION "TFTP Protocol Library and Applications"
  HOMEPAGE_URL "https://git.thomas-vogt.de/Thomas-Vogt/tftp" )

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
find_program(
  CLANG_TIDY_EXE
  NAMES
    clang-tidy-10
    clang-tidy-9
    clang-tidy )

if( CLANG_TIDY_EXE )
  set( CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE} )
endif()

enable_testing()

# Install Configuration
include( GNUInstallDirs )


# get dependencies
include( FetchContent )

FetchContent_Declare(
  cmake_common
  GIT_REPOSITORY https://git.thomas-vogt.de/Thomas-Vogt/cmake_common.git
  GIT_TAG        master )

if( IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake_common )
  set( FETCHCONTENT_SOURCE_DIR_CMAKE_COMMON ${CMAKE_CURRENT_SOURCE_DIR}/cmake_common )
endif()

FetchContent_GetProperties( cmake_common )
if( NOT cmake_common_POPULATED )
  FetchContent_Populate( cmake_common )
  # Add own CMake scripts to module path
  list( APPEND CMAKE_MODULE_PATH "${cmake_common_SOURCE_DIR}" )
endif()


FetchContent_Declare(
  helper
  GIT_REPOSITORY https://git.thomas-vogt.de/Thomas-Vogt/helper.git
  GIT_TAG        master )

if( IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/helper )
  set( FETCHCONTENT_SOURCE_DIR_HELPER ${CMAKE_CURRENT_SOURCE_DIR}/helper )
endif()

FetchContent_MakeAvailable( helper )


add_subdirectory( tftp )
add_subdirectory( application )

add_subdirectory( tftp_api_doc )


set( CPACK_GENERATOR ZIP )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION} )
set( CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} )
set( CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR} )
set( CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH} )
include( CPack )
