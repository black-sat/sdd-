#
# SDD++ - C++ wrapper library for libsdd 2.0
#
# (C) 2023 Nicola Gigante
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

cmake_minimum_required(VERSION 3.10...3.26)

project(
  sdd++
  LANGUAGES C CXX
  VERSION 0.1
  DESCRIPTION "SDD++ - C++ wrapper library for libsdd 2.0"
)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
  message(STATUS "Build configuration not defined. Defaulting to Release")
  set(CMAKE_BUILD_TYPE "Release")
else()
  message(STATUS "Build configuration: '${CMAKE_BUILD_TYPE}'")
endif()

#
# Fix the RPATH for the installation 
#
if(APPLE)
  set(CMAKE_MACOSX_RPATH 1)
  list(
    APPEND CMAKE_INSTALL_RPATH 
    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
  )
endif()


set(CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake"
    ${CMAKE_MODULE_PATH})

include(EnableWarnings) # Common warnings to enable or disable
include(GNUInstallDirs) # Correct and portable installation paths

add_subdirectory(src/lib)
add_subdirectory(src/lib++)