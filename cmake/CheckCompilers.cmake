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


function(check_compilers C_COMPILER CXX_COMPILER)
    find_program(C_COMPILER_EXECUTABLE ${C_COMPILER})
    if(C_COMPILER_EXECUTABLE)
    set(CMAKE_REQUIRED_FLAGS "-std=c99")
        file(WRITE ${CMAKE_BINARY_DIR}/try_compile_test.c "int main() { return 0; }")
        try_compile(C_AVAILABLE
            ${CMAKE_BINARY_DIR}
            ${CMAKE_BINARY_DIR}/try_compile_test.c
            OUTPUT_VARIABLE OUTPUT
            CMAKE_FLAGS "-DCMAKE_C_COMPILER=${C_COMPILER_EXECUTABLE}"
            COPY_FILE ${CMAKE_BINARY_DIR}/try_compile_test.c
            TEST_TARGET_NAME test_target_c
            TEST_COMMAND test_target_c
            TEST_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            TEST_NAME try_compile_test_c
            TEST_DEFAULT
        )
        if(C_AVAILABLE)
            set(CMAKE_C_COMPILER ${C_COMPILER} CACHE STRING "C compiler" FORCE)
            # message(STATUS "C compiler: ${C_COMPILER}")
        else()
            message(WARNING "C compiler ${C_COMPILER} not suitable: ${OUTPUT}")
            return()
        endif()
    else()
        message(WARNING "C compiler ${C_COMPILER} not found")
        return()
    endif()

    find_program(CXX_COMPILER_EXECUTABLE ${CXX_COMPILER})
    if(CXX_COMPILER_EXECUTABLE)
        set(CMAKE_REQUIRED_FLAGS "-std=c++11")
        file(WRITE ${CMAKE_BINARY_DIR}/try_compile_test.cpp "int main() { return 0; }")
        try_compile(CXX_AVAILABLE
            ${CMAKE_BINARY_DIR}
            ${CMAKE_BINARY_DIR}/try_compile_test.cpp
            OUTPUT_VARIABLE OUTPUT
            CMAKE_FLAGS "-DCMAKE_CXX_COMPILER=${CXX_COMPILER_EXECUTABLE}"
            COPY_FILE ${CMAKE_BINARY_DIR}/try_compile_test.cpp
            TEST_TARGET_NAME test_target_cpp
            TEST_COMMAND test_target_cpp
            TEST_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            TEST_NAME try_compile_test
            TEST_DEFAULT
        )
        if(CXX_AVAILABLE)
            set(CMAKE_CXX_COMPILER ${CXX_COMPILER} CACHE STRING "C++ compiler" FORCE)
            # message(STATUS "C++ compiler: ${CXX_COMPILER}")
        else()
            message(WARNING "C++ compiler ${CXX_COMPILER} not suitable: ${OUTPUT}")
        endif()
    else()
        message(WARNING "C++ compiler ${CXX_COMPILER} not found")
    endif()
endfunction()
