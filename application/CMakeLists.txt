# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# 
# $Date$
# $Revision$
# Thomas Vogt, Thomas@Thomas-Vogt.de

cmake_minimum_required( VERSION 3.7)

project ( "TFTP Applications")

add_subdirectory( tftp_unit_test)

add_subdirectory( tftp_client)
add_subdirectory( tftp_server)
