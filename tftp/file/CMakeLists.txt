# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Thomas Vogt, thomas@thomas-vogt.de

cmake_minimum_required( VERSION 3.21 )

target_sources(
  tftp

  PRIVATE
    File.hpp
    TftpFile.hpp
    MemoryFile.hpp
    MemoryFile.cpp
    StreamFile.hpp
    StreamFile.cpp
    NullSinkFile.hpp
    NullSinkFile.cpp )

#target_sources( tftp_test PRIVATE)
