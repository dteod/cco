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


# It happens that this toolchain file is included multiple times
# from CMake itself so we need to check if the target already exists
# like a header guard.
if(NOT CCO_TOOLCHAIN_INCLUDED)
    set(CCO_TOOLCHAIN_INCLUDED TRUE)
    include(${CMAKE_CURRENT_LIST_DIR}/../DefaultSetting.cmake)

    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR x86)

    default_arch_setting(cco EXCHANGE_DEBUG_REGISTERS 0)
    default_arch_setting(cco EXCHANGE_CONTROL_REGISTERS 0)
    default_arch_setting(cco EXCHANGE_SEGMENT_REGISTERS 0)
    default_arch_setting(cco EXCHANGE_FPU_AND_MMX_REGISTERS 0)
    default_arch_setting(cco USE_HAS_FXSR 0)

    if(NOT TARGET cco_arch_${CMAKE_SYSTEM_PROCESSOR})
        add_library(cco_arch_${CMAKE_SYSTEM_PROCESSOR} INTERFACE)
        add_library(cco::arch ALIAS cco_arch_${CMAKE_SYSTEM_PROCESSOR})
        target_compile_options(cco_arch_${CMAKE_SYSTEM_PROCESSOR} INTERFACE -m32)
        target_link_options(cco_arch_${CMAKE_SYSTEM_PROCESSOR} INTERFACE -m32)
    endif()
endif()