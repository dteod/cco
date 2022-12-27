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

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86)

set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

default_arch_setting(CSWITCH EXCHANGE_GENERAL_PURPOSE_REGISTERS 1)
default_arch_setting(CSWITCH EXCHANGE_DEBUG_REGISTERS 0)
default_arch_setting(CSWITCH EXCHANGE_CONTROL_REGISTERS 0)
default_arch_setting(CSWITCH EXCHANGE_SEGMENT_REGISTERS 0)
default_arch_setting(CSWITCH EXCHANGE_FPU_AND_MMX_REGISTERS 0)
default_arch_setting(CSWITCH USE_HAS_FXSR 0)

add_library(cco_arch_${CMAKE_SYSTEM_PROCESSOR} INTERFACE)
add_library(cco::arch ALIAS cco_arch_${CMAKE_SYSTEM_PROCESSOR})
target_compile_definitions(cco_arch_${CMAKE_SYSTEM_PROCESSOR} INTERFACE
    -DCCO_CSWITCH_x86_EXCHANGE_GENERAL_PURPOSE_REGISTERS=${CCO_CSWITCH_x86_EXCHANGE_GENERAL_PURPOSE_REGISTERS}
    -DCCO_CSWITCH_x86_EXCHANGE_DEBUG_REGISTERS=${CCO_CSWITCH_x86_EXCHANGE_DEBUG_REGISTERS}
    -DCCO_CSWITCH_x86_EXCHANGE_CONTROL_REGISTERS=${CCO_CSWITCH_x86_EXCHANGE_CONTROL_REGISTERS}
    -DCCO_CSWITCH_x86_EXCHANGE_SEGMENT_REGISTERS=${CCO_CSWITCH_x86_EXCHANGE_SEGMENT_REGISTERS}
    -DCCO_CSWITCH_x86_EXCHANGE_FPU_AND_MMX_REGISTERS=${CCO_CSWITCH_x86_EXCHANGE_FPU_AND_MMX_REGISTERS}
    -DCCO_CSWITCH_x86_USE_HAS_FXSR=${CCO_CSWITCH_x86_USE_HAS_FXSR}
)
target_compile_options(cco_arch_${CMAKE_SYSTEM_PROCESSOR} INTERFACE -m32)
target_link_options(cco_arch_${CMAKE_SYSTEM_PROCESSOR} INTERFACE -m32)