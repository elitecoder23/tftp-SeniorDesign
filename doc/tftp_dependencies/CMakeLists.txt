# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 4.1 )

find_package( Doxygen COMPONENTS dot )

if( NOT PROJECT_IS_TOP_LEVEL OR NOT TARGET Doxygen::dot OR NOT EXISTS ${CMAKE_BINARY_DIR}/dependencies.dot )
  return()
endif()

set( DEPENDENCY_OUTPUT_FILE "${CMAKE_PROJECT_NAME}_dependencies.pdf" )

add_custom_command(
  OUTPUT ${DEPENDENCY_OUTPUT_FILE}
  COMMAND $<TARGET_FILE:Doxygen::dot> -Tpdf -o ${DEPENDENCY_OUTPUT_FILE} ${CMAKE_BINARY_DIR}/dependencies.dot
  DEPENDS ${CMAKE_BINARY_DIR}/dependencies.dot
  COMMENT "Generating ${DEPENDENCY_OUTPUT_FILE}" )

add_custom_target(
  dependency_graph
  DEPENDS ${DEPENDENCY_OUTPUT_FILE} )

add_dependencies( doc dependency_graph )

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/${DEPENDENCY_OUTPUT_FILE}
  TYPE DOC
  COMPONENT development
  OPTIONAL )
