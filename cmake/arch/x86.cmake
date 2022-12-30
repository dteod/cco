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

include(${CMAKE_CURRENT_LIST_DIR}/../DefaultSetting.cmake)

set(CMAKE_SYSTEM_PROCESSOR x86)

default_arch_setting(cco_x86 ENABLE_EFLAGS_REGISTER_EXCHANGE 1)
default_arch_setting(cco_x86 ENABLE_FPU_MMX_REGISTERS_EXCHANGE 1)
default_arch_setting(cco_x86 ENABLE_SSE_REGISTERS_EXCHANGE 1)
default_arch_setting(cco_x86 ENABLE_SEGMENT_REGISTERS_EXCHANGE 0)
default_arch_setting(cco_x86 ENABLE_DEBUG_REGISTERS_EXCHANGE 0)
default_arch_setting(cco_x86 ENABLE_CONTROL_REGISTERS_EXCHANGE 0)

default_arch_setting(cco_x86 EFLAGS_REGISTER_DEFAULT_EXCHANGE 1)
default_arch_setting(cco_x86 FPU_MMX_REGISTERS_DEFAULT_EXCHANGE 1)
default_arch_setting(cco_x86 SSE_REGISTERS_DEFAULT_EXCHANGE 1)
default_arch_setting(cco_x86 SEGMENT_REGISTERS_DEFAULT_EXCHANGE 0)
default_arch_setting(cco_x86 DEBUG_REGISTERS_DEFAULT_EXCHANGE 0)
default_arch_setting(cco_x86 CONTROL_REGISTERS_DEFAULT_EXCHANGE 0)

add_library(cco_arch_${CMAKE_SYSTEM_PROCESSOR} INTERFACE)
add_library(cco::arch ALIAS cco_arch_${CMAKE_SYSTEM_PROCESSOR})

get_property(CCO_${CMAKE_SYSTEM_PROCESSOR}_COMPILE_DEFINITIONS GLOBAL PROPERTY cco_${CMAKE_SYSTEM_PROCESSOR}_COMPILE_SETTINGS)
foreach(DEF ${CCO_COMPILE_DEFINITIONS})
    target_compile_definitions(${LIBNAME} PUBLIC -DCCO_${CMAKE_SYSTEM_PROCESSOR}_${DEF})
endforeach()