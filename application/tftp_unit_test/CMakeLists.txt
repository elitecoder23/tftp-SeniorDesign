# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# 
# $Date$
# $Revision$
# Thomas Vogt, Thomas@Thomas-Vogt.de

cmake_minimum_required( VERSION 3.3)

find_package( Boost REQUIRED COMPONENTS unit_test_framework)

add_executable( tftp_unit_test tftp_unit_test.cpp)

target_link_libraries( 
  tftp_unit_test 
  
  tftp 
  helper
  
  ${Boost_LIBRARIES}
  ${BOOST_THREAD_LIBRARY})
