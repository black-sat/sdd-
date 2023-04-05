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

set(
  SDDPP_SRC
  src/sdd.cpp
)

add_library(sdd++ SHARED ${SDDPP_SRC})

target_link_libraries(sdd++ PUBLIC sdd)

target_compile_features(sdd++ PUBLIC cxx_std_20)
target_enable_warnings(sdd++)

target_include_directories(sdd++ PUBLIC  
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
)


install(TARGETS sdd++ sdd
  EXPORT sdd++Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

install(
  DIRECTORY include/sdd++ 
  DESTINATION include
  PATTERN "*.in" EXCLUDE
)

install(EXPORT sdd++Targets
  FILE sdd++Targets.cmake
  NAMESPACE sdd++::
  DESTINATION lib/cmake/sdd++
)

include(CMakePackageConfigHelpers)

set(INCLUDE_INSTALL_DIR include/)
set(LIBRARY_INSTALL_DIR lib/)

configure_package_config_file(sdd++Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/sdd++Config.cmake
  INSTALL_DESTINATION lib/cmake/sdd++
  PATH_VARS INCLUDE_INSTALL_DIR LIBRARY_INSTALL_DIR)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/sdd++ConfigVersion.cmake
  COMPATIBILITY SameMajorVersion )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sdd++Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/sdd++ConfigVersion.cmake
        DESTINATION lib/cmake/sdd++ )