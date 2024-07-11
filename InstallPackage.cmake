# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Thomas Vogt, thomas@thomas-vogt.de

cmake_minimum_required( VERSION 3.30 )

set( CPACK_PACKAGE_NAME "${CMAKE_PROJECT_DESCRIPTION}" )
set( CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}" )
set( CPACK_PACKAGE_VENDOR "Thomas Vogt" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION} )
set( CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} )
set( CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR} )
set( CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH} )
set( CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME} )
set( CPACK_PACKAGE_CHECKSUM SHA512 )

set( CPACK_SOURCE_IGNORE_FILES "cmake-.*" )
set( CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-Source" )

if( LINUX )
  # Linux specific generator
  set( CPACK_GENERATOR TBZ2 )
  set( CPACK_SOURCE_GENERATOR TBZ2 )
elseif( WIN32 )
  # Windows specific generator
  set( CPACK_GENERATOR ZIP )
  set( CPACK_SOURCE_GENERATOR ZIP )
endif()

include( CPack )
