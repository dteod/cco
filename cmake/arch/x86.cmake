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

default_arch_setting(cco_arch_${CMAKE_SYSTEM_PROCESSOR} ENABLE_EFLAGS_REGISTER_EXCHANGE 0)
default_arch_setting(cco_arch_${CMAKE_SYSTEM_PROCESSOR} ENABLE_FPU_MMX_REGISTERS_EXCHANGE 0)
default_arch_setting(cco_arch_${CMAKE_SYSTEM_PROCESSOR} ENABLE_SSE_REGISTERS_EXCHANGE 0)
default_arch_setting(cco_arch_${CMAKE_SYSTEM_PROCESSOR} ENABLE_SEGMENT_REGISTERS_EXCHANGE 0)
default_arch_setting(cco_arch_${CMAKE_SYSTEM_PROCESSOR} ENABLE_DEBUG_REGISTERS_EXCHANGE 0)
default_arch_setting(cco_arch_${CMAKE_SYSTEM_PROCESSOR} ENABLE_CONTROL_REGISTERS_EXCHANGE 0)

default_arch_setting(cco_arch_${CMAKE_SYSTEM_PROCESSOR} EFLAGS_REGISTER_DEFAULT_EXCHANGE 0)
default_arch_setting(cco_arch_${CMAKE_SYSTEM_PROCESSOR} FPU_MMX_REGISTERS_DEFAULT_EXCHANGE 0)
default_arch_setting(cco_arch_${CMAKE_SYSTEM_PROCESSOR} SSE_REGISTERS_DEFAULT_EXCHANGE 0)
default_arch_setting(cco_arch_${CMAKE_SYSTEM_PROCESSOR} SEGMENT_REGISTERS_DEFAULT_EXCHANGE 0)
default_arch_setting(cco_arch_${CMAKE_SYSTEM_PROCESSOR} DEBUG_REGISTERS_DEFAULT_EXCHANGE 0)
default_arch_setting(cco_arch_${CMAKE_SYSTEM_PROCESSOR} CONTROL_REGISTERS_DEFAULT_EXCHANGE 0)

add_library(cco_arch_${CMAKE_SYSTEM_PROCESSOR} INTERFACE)
add_library(cco::arch ALIAS cco_arch_${CMAKE_SYSTEM_PROCESSOR})
