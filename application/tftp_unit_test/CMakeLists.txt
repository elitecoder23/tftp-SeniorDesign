# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Thomas Vogt, thomas@thomas-vogt.de

cmake_minimum_required( VERSION 3.16 )

find_package( Boost REQUIRED )

add_executable( tftp_unit_test )

target_sources( tftp_unit_test PRIVATE tftp_unit_test.cpp )

target_compile_definitions(
  tftp_unit_test

  PRIVATE
    # Disable C++17 deprecation warning for BOOST in MSVC (Version 1.67)
    $<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING> )

target_link_libraries(
  tftp_unit_test

  PRIVATE
    tftp_test
    helper_test
    Boost::boost)

add_test( NAME tftp_unit_test COMMAND tftp_unit_test )

# INSTALL
include( GNUInstallDirs )

install(
  TARGETS tftp_unit_test
  COMPONENT runtime
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
