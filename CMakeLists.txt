# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.20 )

project(
  tftp
  #VERSION 0.0.0.0
  DESCRIPTION "TFTP Protocol Library and Applications"
  HOMEPAGE_URL "https://git.thomas-vogt.de/thomas-vogt/tftp" )

find_package( Git )

if( GIT_FOUND )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --verify --short HEAD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE PROJECT_GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE )
endif()

if( PROJECT_GIT_HASH )
  set( PROJECT_VERSION_FULL "${PROJECT_VERSION} (${PROJECT_GIT_HASH})" )
else()
  set( PROJECT_VERSION_FULL "${PROJECT_VERSION}" )
endif()
string( STRIP ${PROJECT_VERSION_FULL} PROJECT_VERSION_FULL )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

enable_testing()

include( GNUInstallDirs )

# Activate Dynamic Libraries for Boost on Windows Platform
if( WIN32 )
  set( Boost_USE_STATIC_LIBS OFF )
endif()

# TODO Disable Module scanning for the moment
set( CMAKE_CXX_SCAN_FOR_MODULES OFF )

include( FetchContent )

FetchContent_Declare(
  helper
  GIT_REPOSITORY https://git.thomas-vogt.de/thomas-vogt/helper.git
  GIT_TAG        main )

if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/helper )
  set( FETCHCONTENT_SOURCE_DIR_HELPER ${CMAKE_SOURCE_DIR}/helper )
endif()

FetchContent_MakeAvailable( helper )


# find clang-tidy and add options to execute
find_program( CLANG_TIDY_EXE NAMES clang-tidy )

if( CLANG_TIDY_EXE )
  set( CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE} -p )
endif()

# find iwyu and add options to execute
find_program( CLANG_IWYU_EXE NAMES iwyu )

if( CLANG_IWYU_EXE )
  #set( CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${CLANG_IWYU_EXE} )
endif()

add_subdirectory( lib )
add_subdirectory( app )
add_subdirectory( doc )

install(
  FILES LICENSE
  RENAME ${PROJECT_NAME}.LICENSE
  TYPE DOC
  COMPONENT runtime )

if( PROJECT_IS_TOP_LEVEL )
  include( InstallPackage.cmake )
endif()