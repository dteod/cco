/*
 *   cco - coroutine library for C
 *   Copyright (C) 2022 Domenico Teodonio at dteod@protonmail.com
 *
 *   cco is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   cco is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with cco.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CCO_ARCH_x86_H_INCLUDED
#define CCO_ARCH_x86_H_INCLUDED

/**
 * @brief Settings for 32-bit x86 architectures.
 * 
 */
typedef unsigned int cco_x86_settings;

#if CCO_x86_ENABLE_EFLAGS_REGISTER_EXCHANGE
#  define CCO_SETTINGS_x86_EXCHANGE_EFLAGS_REGISTER ((cco_x86_settings)(1 << 0))
#endif

#if CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE
#  define CCO_SETTINGS_x86_EXCHANGE_FPU_MMX_REGISTERS ((cco_x86_settings)(1 << 1))
#endif

#if CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE
#  define CCO_SETTINGS_x86_EXCHANGE_SSE_REGISTERS ((cco_x86_settings)(1 << 2))
#endif

#if CCO_x86_ENABLE_SEGMENT_REGISTERS_EXCHANGE
#  define CCO_SETTINGS_x86_EXCHANGE_SEGMENT_REGISTERS ((cco_x86_settings)(1 << 3))
#endif

#if CCO_x86_ENABLE_DEBUG_REGISTERS_EXCHANGE
#  define CCO_SETTINGS_x86_EXCHANGE_DEBUG_REGISTERS ((cco_x86_settings)(1 << 4))
#endif

#if CCO_x86_ENABLE_CONTROL_REGISTERS_EXCHANGE
#  define CCO_SETTINGS_x86_EXCHANGE_CONTROL_REGISTERS ((cco_x86_settings)(1 << 5))
#endif

typedef cco_x86_settings cco_architecture_specific_settings;

#endif