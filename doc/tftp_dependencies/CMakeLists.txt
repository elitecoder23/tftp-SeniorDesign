# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.31 )

find_package( Doxygen COMPONENTS dot )

if( NOT PROJECT_IS_TOP_LEVEL OR NOT TARGET Doxygen::dot OR NOT EXISTS ${CMAKE_BINARY_DIR}/dependencies.dot )
  return()
endif()

add_custom_command(
  OUTPUT dependencies.pdf
  COMMAND $<TARGET_FILE:Doxygen::dot> -Tpdf -o dependencies.pdf ${CMAKE_BINARY_DIR}/dependencies.dot
  DEPENDS ${CMAKE_BINARY_DIR}/dependencies.dot
  COMMENT "Generate TFTP Dependencies" )

add_custom_target(
  tftp_dependency_graph
  DEPENDS dependencies.pdf )

add_dependencies( doc tftp_dependency_graph )

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/dependencies.pdf
  TYPE DOC
  COMPONENT development
  OPTIONAL )
