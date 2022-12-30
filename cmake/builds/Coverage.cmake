# cco - coroutine library for C
# Copyright (C) 2021-2022 Domenico Teodonio
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This file is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

if(C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS_COVERAGE "-O3 -fprofile-arcs -fprofile-abs-path -ftest-coverage " CACHE STRING "Flags used by the C compiler in coverage builds." FORCE)
    set(CMAKE_CXX_FLAGS_COVERAGE "-O3 -fprofile-arcs -fprofile-abs-path -ftest-coverage" CACHE STRING "Flags used by the C++ compiler in coverage builds." FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "-fgcov" CACHE STRING "Flags used for linking binaries in coverage builds." FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE "-fgcov" CACHE STRING "Flags used by the shared libraries linker in coverage builds." FORCE)
elseif(C_COMPILER_ID STREQUAL "Clang" OR C_COMPILER_ID STREQUAL "AppleClang")
    set(CMAKE_C_FLAGS_COVERAGE "${CMAKE_C_FLAGS_RELEASE} -fprofile-instr-generate" CACHE STRING "Flags used by the C compiler during Coverage builds." FORCE)
elseif(C_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_C_FLAGS_COVERAGE "${CMAKE_C_FLAGS_RELEASE} -prof-gen" CACHE STRING "Flags used by the C compiler during Coverage builds." FORCE)
elseif(C_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS_COVERAGE "${CMAKE_C_FLAGS_RELEASE} /Wall /Wabi /fsanitize=coverage" CACHE STRING "Flags used by the C compiler during Coverage builds." FORCE)
endif()
mark_as_advanced(
  CMAKE_CXX_FLAGS_COVERAGE
  CMAKE_C_FLAGS_COVERAGE
  CMAKE_EXE_LINKER_FLAGS_COVERAGE
  CMAKE_SHARED_LINKER_FLAGS_COVERAGE)