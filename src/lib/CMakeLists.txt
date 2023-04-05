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
  SDD_SRC
  src/src/vtree_search/state.c
  src/src/vtree_search/auto.c
  src/src/vtree_search/search.c
  src/src/vtrees/static.c
  src/src/vtrees/compare.c
  src/src/vtrees/io.c
  src/src/vtrees/maps.c
  src/src/vtrees/moves.c
  src/src/vtrees/edit.c
  src/src/vtrees/vtree.c
  src/src/vtree_fragments/construction.c
  src/src/vtree_fragments/moves.c
  src/src/vtree_fragments/operations.c
  src/src/util.c
  src/src/vtree_operations/rollback.c
  src/src/vtree_operations/op_left_rotate.c
  src/src/vtree_operations/op_swap.c
  src/src/vtree_operations/split.c
  src/src/vtree_operations/limits.c
  src/src/vtree_operations/dissect.c
  src/src/vtree_operations/cartesian_product.c
  src/src/vtree_operations/op_right_rotate.c
  src/src/fnf/io.c
  src/src/verify.c
  src/src/basic/shadows.c
  src/src/basic/nodes.c
  src/src/basic/memory.c
  src/src/basic/sort.c
  src/src/basic/references.c
  src/src/basic/hash.c
  src/src/basic/count_and_size.c
  src/src/basic/replace.c
  src/src/basic/multiply.c
  src/src/basic/computed.c
  src/src/basic/partitions.c
  src/src/basic/gc.c
  src/src/manager/interface.c
  src/src/manager/variables.c
  src/src/manager/copy.c
  src/src/manager/stats.c
  src/src/manager/manager.c
  src/src/sdds/forall.c
  src/src/sdds/exists_multiple.c
  src/src/sdds/io.c
  src/src/sdds/wmc.c
  src/src/sdds/rename_vars.c
  src/src/sdds/apply.c
  src/src/sdds/copy.c
  src/src/sdds/exists_multiple_static.c
  src/src/sdds/bits.c
  src/src/sdds/size.c
  src/src/sdds/condition.c
  src/src/sdds/count.c
  src/src/sdds/cardinality.c
  src/src/sdds/model_count.c
  src/src/sdds/essential_vars.c
  src/src/sdds/exists.c
  src/src/version.c
)

add_library(sdd SHARED ${SDD_SRC})

option(SDD_ENABLE_FULL_DEBUG OFF)

if(SDD_ENABLE_FULL_DEBUG)
  target_compile_definitions(sdd PRIVATE FULL_DEBUG=1)
else()
  target_compile_definitions(sdd PRIVATE FULL_DEBUG=0)
endif()

target_compile_definitions(sdd PRIVATE SDD_VERSION="2.0")

string(TIMESTAMP TODAY "%Y-%m-%d")
target_compile_definitions(sdd PRIVATE SDD_DATE="${TODAY}")

target_include_directories(sdd PRIVATE src/include)

target_include_directories(sdd PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(
  DIRECTORY include/sdd 
  DESTINATION include
  PATTERN "*.in" EXCLUDE
)

