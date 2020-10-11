# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Thomas Vogt, thomas@thomas-vogt.de

cmake_minimum_required( VERSION 3.18 )

find_package( Doxygen )
find_package( Boost )

if( NOT TARGET doc )
  add_custom_target( doc COMMENT "Generate Documentation" )
endif( NOT TARGET doc )

if( DOXYGEN_FOUND )
  set( DOXYGEN_PROJECT_NAME "${tftp_DESCRIPTION}" )
  set( DOXYGEN_PROJECT_BRIEF "${tftp_DESCRIPTION}" )
  set( DOXYGEN_BUILTIN_STL_SUPPORT YES )
  set( DOXYGEN_EXTRACT_PRIVATE YES )
  set( DOXYGEN_EXTRACT_PACKAGE YES )
  set( DOXYGEN_QUIET YES )
  set( DOXYGEN_WARN_NO_PARAMDOC YES )
  set( DOXYGEN_HTML_DYNAMIC_SECTIONS YES )

  set(
    DOXYGEN_EXCLUDE_PATTERNS

    *Test.*
    */implementation/* )

  set(
    DOXYGEN_STRIP_FROM_PATH

    ${helper_SOURCE_DIR}
    ${tftp_SOURCE_DIR}
    ${tftp_SOURCE_DIR}/application )

  set(
    DOXYGEN_INCLUDE_PATH

    ${Boost_INCLUDE_DIRS}
    ${helper_SOURCE_DIR}
    ${tftp_SOURCE_DIR} )

  doxygen_add_docs(
    tftp_api_doc

    Mainpage.md
    ${helper_SOURCE_DIR}/helper

    ${tftp_SOURCE_DIR}/tftp
    ${tftp_SOURCE_DIR}/application/tftp_client
    ${tftp_SOURCE_DIR}/application/tftp_server
    ${tftp_SOURCE_DIR}/application/tftp_unit_test )

  add_dependencies( doc tftp_api_doc )

  # add index.html redirection
  configure_file(
    ${cmake_common_SOURCE_DIR}/doxygen_index.html.in
    index.html
    @ONLY )

  install(
    DIRECTORY
      ${CMAKE_CURRENT_BINARY_DIR}/html
    COMPONENT documentation
    DESTINATION ${CMAKE_INSTALL_DOCDIR}/tftp_api_doc
    OPTIONAL )

  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/index.html
    COMPONENT documentation
    DESTINATION ${CMAKE_INSTALL_DOCDIR}/tftp_api_doc
    OPTIONAL )

#  file(
#    GENERATE
#    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/CMakeGraphVizOptions.cmake
#    CONTENT "\
#cmake_minimum_required( VERSION 3.18 )
#set( GRAPHVIZ_EXTERNAL_LIBS FALSE)
#set( GRAPHVIZ_GENERATE_PER_TARGET FALSE)
#set( GRAPHVIZ_GENERATE_DEPENDERS FALSE)")

#  add_custom_target(
#    doc_dep
#    ${CMAKE_COMMAND} --graphviz=${CMAKE_CURRENT_BINARY_DIR}/doc_dep.dot ${CMAKE_CURRENT_SOURCE_DIR}
#    COMMAND ${DOXYGEN_DOT_EXECUTABLE} -Tpdf -O ${CMAKE_CURRENT_BINARY_DIR}/doc_dep.dot)

  #add_dependencies( doc doc_dep)
endif()
