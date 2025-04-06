# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.31 )

find_package( Doxygen )
find_package( Boost )

if( NOT DOXYGEN_FOUND )
  return()
endif()

get_property(
  HELPER_DOC_PATHS
  DIRECTORY ${helper_SOURCE_DIR}
  PROPERTY DOC_PATHS )

get_property(
  TFTP_DOC_PATHS
  DIRECTORY ${tftp_SOURCE_DIR}
  PROPERTY DOC_PATHS )

set( DOXYGEN_PROJECT_NAME "${tftp_DESCRIPTION}" )
set( DOXYGEN_PROJECT_BRIEF "API Description" )
set( DOXYGEN_PROJECT_NUMBER "${tftp_VERSION}" )
#set( DOXYGEN_BUILTIN_STL_SUPPORT YES )
set( DOXYGEN_EXTRACT_PRIVATE NO )
set( DOXYGEN_EXTRACT_STATIC NO )
set( DOXYGEN_EXTRACT_PACKAGE YES )
set( DOXYGEN_QUIET YES )
#set( DOXYGEN_WARN_NO_PARAMDOC YES )
set( DOXYGEN_HTML_DYNAMIC_SECTIONS YES )
set( DOXYGEN_GENERATE_TREEVIEW YES )
set( DOXYGEN_INCLUDED_BY_GRAPH NO )

if( WIN32 )
  # shorten filename otherwise exceeds 255 characters for paths
  set( DOXYGEN_SHORT_NAMES YES )
endif()

set(
  DOXYGEN_EXCLUDE_PATTERNS

  */test/*
  */implementation/* )

set(
  DOXYGEN_STRIP_FROM_PATH

  ${helper_SOURCE_DIR}/lib
  ${helper_BINARY_DIR}/lib
  ${helper_SOURCE_DIR}/app
  ${tftp_SOURCE_DIR}/lib
  ${tftp_BINARY_DIR}/lib
  ${tftp_SOURCE_DIR}/app )

set(
  DOXYGEN_INCLUDE_PATH

  ${Boost_INCLUDE_DIRS}
  ${helper_SOURCE_DIR}/lib
  ${helper_BINARY_DIR}/lib
  ${tftp_SOURCE_DIR}/lib
  ${tftp_BINARY_DIR}/lib )

doxygen_add_docs(
  tftp_api_doc

  ../Mainpage.md
  ${HELPER_DOC_PATHS}
  ${TFTP_DOC_PATHS} )

add_dependencies( doc tftp_api_doc )

# add index.html redirection
set( INDEX_HTML_OUTPUT_DIR html )
configure_file(
  ../doxygen_index.html.in
  index.html
  @ONLY )

install(
  DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/html/
  DESTINATION ${CMAKE_INSTALL_DOCDIR}/tftp_api_doc
  COMPONENT development
  OPTIONAL
  MESSAGE_NEVER )

set( INDEX_HTML_OUTPUT_DIR tftp_api_doc )
configure_file(
  ../doxygen_index.html.in
  tftp_api_doc.html
  @ONLY )

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/tftp_api_doc.html
  DESTINATION ${CMAKE_INSTALL_DOCDIR}
  COMPONENT development
  OPTIONAL )
