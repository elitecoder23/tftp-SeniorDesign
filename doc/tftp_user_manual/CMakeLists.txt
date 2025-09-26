# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.31 )

find_program( ASCIIDOCTOR_EXE NAMES asciidoctor DOC "Ascii Doctor executable" )

if( NOT ASCIIDOCTOR_EXE )
  return()
endif()

add_custom_target(
  tftp_user_manual
  COMMAND
    ${ASCIIDOCTOR_EXE}
    --backend html5
    --source-dir ${CMAKE_CURRENT_SOURCE_DIR}
    --destination-dir ${CMAKE_CURRENT_BINARY_DIR}
    --attribute source-highlighter=pygments
    ${CMAKE_CURRENT_SOURCE_DIR}/tftp_user_manual.adoc
  COMMENT "Generating tftp_user_manual.html" )

add_dependencies( doc tftp_user_manual )

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/tftp_user_manual.html
  TYPE DOC
  COMPONENT runtime
  OPTIONAL )
