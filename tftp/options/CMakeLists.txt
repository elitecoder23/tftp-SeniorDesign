# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Thomas Vogt, thomas@thomas-vogt.de

cmake_minimum_required( VERSION 3.13)

target_sources(
  tftp

  PRIVATE
    IntegerOption.hpp
    Option.hpp
    Option.cpp
    OptionList.hpp
    OptionList.cpp
    Options.hpp
    StringOption.hpp
    StringOption.cpp)

target_sources(
  tftp_test

  PRIVATE
    IntegerOptionTest.cpp
    OptionTest.cpp
    OptionListTest.cpp
    StringOptionTest.cpp)
