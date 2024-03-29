# cco - coroutine library for C
# Copyright (C) 2022 Domenico Teodonio at dteod@protonmail.com

# cco is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# cco is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with cco.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.24)

execute_process(COMMAND bash -c "source ${CMAKE_CURRENT_SOURCE_DIR}/scripts/version-parse.sh && echo -n $VERSION" OUTPUT_VARIABLE CCO_VERSION)
execute_process(COMMAND bash -c "source ${CMAKE_CURRENT_SOURCE_DIR}/scripts/version-parse.sh && echo -n $VERSION_FULL" OUTPUT_VARIABLE CCO_VERSION_FULL)
execute_process(COMMAND bash -c "source ${CMAKE_CURRENT_SOURCE_DIR}/scripts/version-parse.sh && echo -n $VERSION_MAJOR" OUTPUT_VARIABLE CCO_VERSION_MAJOR)
execute_process(COMMAND bash -c "source ${CMAKE_CURRENT_SOURCE_DIR}/scripts/version-parse.sh && echo -n $VERSION_MINOR" OUTPUT_VARIABLE CCO_VERSION_MINOR)
execute_process(COMMAND bash -c "source ${CMAKE_CURRENT_SOURCE_DIR}/scripts/version-parse.sh && echo -n $VERSION_PATCH" OUTPUT_VARIABLE CCO_VERSION_PATCH)
execute_process(COMMAND bash -c "source ${CMAKE_CURRENT_SOURCE_DIR}/scripts/version-parse.sh && echo -n $VERSION_PRERELEASE" OUTPUT_VARIABLE CCO_VERSION_PRERELEASE)
execute_process(COMMAND bash -c "source ${CMAKE_CURRENT_SOURCE_DIR}/scripts/version-parse.sh && echo -n $VERSION_METADATA" OUTPUT_VARIABLE CCO_VERSION_METADATA)

if(DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
else()
    # We are stuck in a loop: the CMAKE_HOST_SYSTEM_PROCESSOR and CMAKE_SYSTEM_PROCESSOR variables will be
    # set only after a call to project() and we need to know the processor architecture in advance to select the correct toolchain
    # file based on the host architecture. Here are the possible solutions:
    #   1. Call project() to get the processor architecture and then call project() again with the correct toolchain file;
    #   2. Use OS-specific commands to get the processor architecture and then call project() with the correct toolchain file.
    # We choose the second solution because it gives us more control over where and how the library will be compiled.
    if(WIN32)
        execute_process(COMMAND set PROCESSOR_ARCHITECTURE OUTPUT_VARIABLE PROCESSOR_ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(PROCESSOR_ARCHITECTURE STREQUAL "AMD64")
            set(CMAKE_HOST_SYSTEM_PROCESSOR "x86_64")
        elseif(PROCESSOR_ARCHITECTURE STREQUAL "x86")
            set(CMAKE_HOST_SYSTEM_PROCESSOR "x86")
        elseif(PROCESSOR_ARCHITECTURE STREQUAL "ARM")
            set(CMAKE_HOST_SYSTEM_PROCESSOR "arm")
        elseif(PROCESSOR_ARCHITECTURE STREQUAL "ARM64")
            set(CMAKE_HOST_SYSTEM_PROCESSOR "aarch64")
        else()
            message(FATAL_ERROR "Unsupported Visual Studio platform: ${CMAKE_VS_PLATFORM_NAME}")
        endif()
        find_program(MSVC_EXECUTABLE cl)
        if(MSVC_EXECUTABLE)
            set(CMAKE_DEFAULT_C_COMPILER MSVC)
        else()
            find_program(CLANG_CL_EXECUTABLE clang-cl)
            if(CLANG_CL_EXECUTABLE)
                set(CMAKE_DEFAULT_C_COMPILER clang-cl)
            else()
                find_program(CLANG_EXECUTABLE clang)
                if(CLANG_EXECUTABLE)
                    set(CMAKE_DEFAULT_C_COMPILER clang)
                else()
                    find_program(GCC_EXECUTABLE gcc)
                    if(GCC_EXECUTABLE)
                        set(CMAKE_DEFAULT_C_COMPILER gcc)
                    endif()
                    unset(GCC_EXECUTABLE)
                endif()
                unset(CLANG_EXECUTABLE)
            endif()
            unset(CLANG_CL_EXECUTABLE)
        endif()
        unset(MSVC_EXECUTABLE)
        unset(PROCESSOR_ARCHITECTURE)
    elseif(UNIX)
        execute_process(COMMAND uname -m OUTPUT_VARIABLE HOST_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(HOST_ARCH STREQUAL "x86_64")
            set(CMAKE_HOST_SYSTEM_PROCESSOR "x86_64")
        elseif(HOST_ARCH STREQUAL "i386" 
            OR HOST_ARCH STREQUAL "i486" 
            OR HOST_ARCH STREQUAL "i586" 
            OR HOST_ARCH STREQUAL "i686")
            set(CMAKE_HOST_SYSTEM_PROCESSOR "x86")
        elseif(HOST_ARCH STREQUAL "armv7l"
            OR HOST_ARCH STREQUAL "armv7b"
            OR HOST_ARCH STREQUAL "armv7a"
            OR HOST_ARCH STREQUAL "armv7r"
            OR HOST_ARCH STREQUAL "armv7m"
            OR HOST_ARCH STREQUAL "armv8l" 
            OR HOST_ARCH STREQUAL "armv8b"
            OR HOST_ARCH STREQUAL "armv8a"
            OR HOST_ARCH STREQUAL "armv8r"
            OR HOST_ARCH STREQUAL "armv8m")
            set(CMAKE_HOST_SYSTEM_PROCESSOR "arm")
        elseif(PROCESSOR_ARCHITECTURE STREQUAL "aarch64")
            set(CMAKE_HOST_SYSTEM_PROCESSOR "aarch64")
        elseif(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchains/${HOST_ARCH}-${CMAKE_HOST_SYSTEM_NAME}.cmake)
            message(FATAL_ERROR "Unsupported platform: ${HOST_ARCH}")
        endif()
        find_program(GCC_EXECUTABLE gcc)
        if(GCC_EXECUTABLE)
            set(CMAKE_DEFAULT_C_COMPILER gcc)
        else()
            find_program(CLANG_EXECUTABLE clang)
            if(CLANG_EXECUTABLE)
                set(CMAKE_DEFAULT_C_COMPILER clang)
            endif()
            unset(CLANG_EXECUTABLE)
        endif()
        unset(GCC_EXECUTABLE)
        unset(HOST_ARCH)
    else()
        message(WARNING "Unsupported host operating system '${CMAKE_HOST_SYSTEM_NAME}' while determining architecture, falling back to x86_64")
        set(CMAKE_HOST_SYSTEM_PROCESSOR "x86_64")
    endif()

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchains/${CMAKE_HOST_SYSTEM_PROCESSOR}-${CMAKE_HOST_SYSTEM_NAME}.cmake)
        message(STATUS "Toolchain file not defined, using host-default: ${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchains/${CMAKE_HOST_SYSTEM_PROCESSOR}-${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_DEFAULT_C_COMPILER}.cmake")
        set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchains/${CMAKE_HOST_SYSTEM_PROCESSOR}-${CMAKE_HOST_SYSTEM_NAME}.cmake)
    else()
        message(FATAL_ERROR "Toolchain file not defined and host-default toolchain file not found: ${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchains/${CMAKE_HOST_SYSTEM_PROCESSOR}-${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_DEFAULT_C_COMPILER}.cmake")
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/DefaultSetting.cmake)
    endif()
endif()

project(cco VERSION ${CCO_VERSION_MAJOR}.${CCO_VERSION_MINOR}.${CCO_VERSION_PATCH}
    DESCRIPTION "C library for coroutines"
    LANGUAGES C CXX
)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/builds/Coverage.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/builds/Profile.cmake)

message(STATUS "C Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Target platform: ${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_NAME}")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel Coverage Profile." FORCE)
    message(STATUS "Build type not defined, using default: ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
endif()

option(CMAKE_FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)

if(${CMAKE_FORCE_COLORED_OUTPUT})
    if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif()

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif()
endif()

get_property(TARGET_SUPPORTS_SHARED_LIBS GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS)

if(CMAKE_BUILD_TYPE STREQUAL DEBUG)
    default_compile_setting(cco DEBUG 1)
else()
    default_compile_setting(cco DEBUG 0)
endif()

default_compile_setting(cco STATIC_MALLOC_N_PAGES 0)
default_compile_setting(cco STATIC_MALLOC_PAGE_SIZE 4096)
default_compile_setting(cco STATIC_MALLOC_THREAD_SAFE 1)
default_compile_setting(cco STATIC_MALLOC_THREAD_SAFE_LOCK_FREE 1)
default_compile_setting(cco STATIC_MALLOC_THREAD_SAFE_LOCK_FREE_SPIN_COUNT 1000)
default_compile_setting(cco STATIC_MALLOC_THREAD_SAFE_LOCK_FREE_SPIN_EXPONENTIAL_BACKOFF 1)

default_compile_setting(cco ENABLE_THROW 0)

default_compile_setting(cco CATCH_SIGNALS 0)
default_compile_setting(cco CATCH_SIGNALS_ALL 1)
default_compile_setting(cco CATCH_SIGNALS_SIGABRT 1)
default_compile_setting(cco CATCH_SIGNALS_SIGFPE 1)
default_compile_setting(cco CATCH_SIGNALS_SIGILL 1)
default_compile_setting(cco CATCH_SIGNALS_SIGINT 1)
default_compile_setting(cco CATCH_SIGNALS_SIGSEGV 1)
default_compile_setting(cco CATCH_SIGNALS_SIGTERM 1)
default_compile_setting(cco CATCH_SIGNALS_SIGTRAP 1)
default_compile_setting(cco CATCH_SIGNALS_SIGXCPU 1)
default_compile_setting(cco CATCH_SIGNALS_SIGXFSZ 1)

default_compile_setting(cco CATCH_CPP_EXCEPTIONS 0)
default_compile_setting(cco CATCH_CPP_EXCEPTIONS_ALL 0)
default_compile_setting(cco CATCH_CPP_EXCEPTIONS_BAD_ALLOC 0)
default_compile_setting(cco CATCH_CPP_EXCEPTIONS_BAD_CAST 0)
default_compile_setting(cco CATCH_CPP_EXCEPTIONS_BAD_TYPEID 0)
default_compile_setting(cco CATCH_CPP_EXCEPTIONS_DOMAIN_ERROR 0)
default_compile_setting(cco CATCH_CPP_EXCEPTIONS_INVALID_ARGUMENT 0)
default_compile_setting(cco CATCH_CPP_EXCEPTIONS_LENGTH_ERROR 0)
default_compile_setting(cco CATCH_CPP_EXCEPTIONS_LOGIC_ERROR 0)
default_compile_setting(cco CATCH_CPP_EXCEPTIONS_OUT_OF_RANGE 0)
default_compile_setting(cco CATCH_CPP_EXCEPTIONS_RUNTIME_ERROR 0)
default_compile_setting(cco CATCH_CPP_EXCEPTIONS_UNDERFLOW_ERROR 0)
default_compile_setting(cco CATCH_CPP_EXCEPTIONS_UNCAUGHT_EXCEPTION 0)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/TargetHardCompilation.cmake)

target_sources(cco_arch_${CMAKE_SYSTEM_PROCESSOR} INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include/
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/cco/arch/${CMAKE_SYSTEM_PROCESSOR}.h
)
set_target_properties(cco_arch_${CMAKE_SYSTEM_PROCESSOR} PROPERTIES EXPORT_NAME arch)
get_property(CCO_${CMAKE_SYSTEM_PROCESSOR}_COMPILE_DEFINITIONS GLOBAL PROPERTY cco_${CMAKE_SYSTEM_PROCESSOR}_COMPILE_SETTINGS)
foreach(DEF ${CCO_COMPILE_DEFINITIONS})
    target_compile_definitions(${LIBNAME} PUBLIC -DCCO_${CMAKE_SYSTEM_PROCESSOR}_${DEF})
endforeach()

function(add_cco_library LIBTYPE)
    string(TOLOWER ${LIBTYPE} LIBVARIANT)
    string(PREPEND LIBNAME "cco_${LIBVARIANT}")
    add_library(${LIBNAME} ${LIBTYPE}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/coroutine.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/version.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/errno.c
    )
    add_library(cco::${LIBVARIANT} ALIAS ${LIBNAME})
    target_sources(${LIBNAME} PUBLIC FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include/
        FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cco.h
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cco/api.h
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cco/arch.h
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cco/coroutine.h
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cco/errno.h
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cco/version.h
    )
    target_include_directories(${LIBNAME} 
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_compile_features(${LIBNAME} PRIVATE c_std_11)

    if(LIBTYPE STREQUAL "STATIC")
        set(STATIC_VALUE 1)
        target_compile_definitions(${LIBNAME} PRIVATE CCO_SHARED=0)
    else()
        set(STATIC_VALUE 0)
        target_compile_definitions(${LIBNAME} PRIVATE CCO_SHARED=1)
    endif()
    target_compile_definitions(
        ${LIBNAME} PRIVATE
        -DCCO_INTERNAL=1
        -DCCO_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        -DCCO_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        -DCCO_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    )
    target_compile_definitions(
        ${LIBNAME} PUBLIC
        -DCCO_TARGET_ARCH=CCO_ARCH_${CMAKE_SYSTEM_PROCESSOR}
    )
    get_property(CCO_COMPILE_DEFINITIONS GLOBAL PROPERTY cco_${CMAKE_SYSTEM_PROCESSOR}_COMPILE_SETTINGS)
    foreach(DEF ${CCO_COMPILE_DEFINITIONS})
        target_compile_definitions(${LIBNAME} PRIVATE -DCCO_${CMAKE_SYSTEM_PROCESSOR}_${DEF})
    endforeach()

    if(WIN32 AND STATIC_VALUE)
        set_target_properties(${LIBNAME} PROPERTIES OUTPUT_NAME "cco-static")
    else()
        set_target_properties(${LIBNAME} PROPERTIES OUTPUT_NAME "cco")
    endif()

    target_hard_compilation(${LIBNAME} PRIVATE)
    target_link_libraries(${LIBNAME} PRIVATE cco_arch_${CMAKE_SYSTEM_PROCESSOR})

    set_target_properties(${LIBNAME} PROPERTIES EXPORT_NAME ${LIBVARIANT})
    install(TARGETS ${LIBNAME}
        EXPORT cco-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        PUBLIC_HEADER DESTINATION include/cco
        FILE_SET HEADERS
    )
endfunction()

add_cco_library(STATIC)
if(TARGET_SUPPORTS_SHARED_LIBS)
    add_cco_library(SHARED)
endif()
install(TARGETS cco_arch_${CMAKE_SYSTEM_PROCESSOR}
    EXPORT cco-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    PUBLIC_HEADER DESTINATION include/cco
    FILE_SET HEADERS
)

add_custom_target(cco_tests)
function(create_cco_test TESTNAME)
    get_filename_component(TEST_BASE_NAME "${TESTNAME}" NAME_WE)
    get_filename_component(TEST_EXTENSION "${TESTNAME}" EXT)
    add_executable(cco_${TEST_BASE_NAME}_test EXCLUDE_FROM_ALL ${CMAKE_CURRENT_SOURCE_DIR}/test/${TESTNAME})
    add_dependencies(cco_tests cco_${TEST_BASE_NAME}_test)
    target_link_libraries(cco_${TEST_BASE_NAME}_test PRIVATE cco_static cco_arch_${CMAKE_SYSTEM_PROCESSOR})
    target_include_directories(cco_${TEST_BASE_NAME}_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test)
    if(TEST_BASE_NAME STREQUAL ${CMAKE_SYSTEM_PROCESSOR} AND TEST_EXTENSION STREQUAL ".c")
        target_include_directories(cco_${TEST_BASE_NAME}_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    elseif(TEST_EXTENSION STREQUAL ".cpp")
        target_compile_features(cco_${TEST_BASE_NAME}_test PRIVATE cxx_std_20)
    endif()
    target_hard_compilation(cco_${TEST_BASE_NAME}_test PRIVATE)
    add_test(NAME cco_${TEST_BASE_NAME}_test COMMAND cco_${TEST_BASE_NAME}_test)
endfunction()

include(CTest)
create_cco_test(example.c)
create_cco_test(black_box.cpp)
create_cco_test(${CMAKE_SYSTEM_PROCESSOR}.c)

set(CPACK_PACKAGE_NAME "cco-${CMAKE_SYSTEM_PROCESSOR}")
include(CPack)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cco-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cco-config.cmake
    INSTALL_DESTINATION share/cmake/cco
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cco-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(EXPORT cco-targets
    FILE cco-targets.cmake
    NAMESPACE cco::
    DESTINATION share/cmake/cco
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cco-config.cmake 
    ${CMAKE_CURRENT_BINARY_DIR}/cco-config-version.cmake 
    DESTINATION share/cmake/cco
)
