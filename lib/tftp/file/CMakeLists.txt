# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Thomas Vogt, thomas@thomas-vogt.de

cmake_minimum_required( VERSION 3.25 )

target_sources(
  tftp

  PUBLIC
    FILE_SET HEADERS
      FILES
        File.hpp
        MemoryFile.hpp
        NullSinkFile.hpp
        StreamFile.hpp
        TftpFile.hpp

  PRIVATE
    MemoryFile.cpp
    StreamFile.cpp
    NullSinkFile.cpp )

#target_sources( tftp_test PRIVATE)
