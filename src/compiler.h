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

#ifndef CCO_SRC_COMPILER_H_INCLUDED
#define CCO_SRC_COMPILER_H_INCLUDED

#include "cco.h"

#if defined(__GNUC__) || defined(__clang__)
#  define ctor __attribute__((constructor))
#elif defined(_MSC_VER)
#  define ctor _Pragma("init_seg(lib)")
#elif defined(__INTEL_COMPILER) || defined(__ICC)
#  define ctor _Pragma("section(\"init_section\",constructor)")
#else
#  error Unsupported compiler
#endif

#if defined(__GNUC__) || defined(__clang__) || defined(__ICC) || defined(__INTEL_COMPILER)
#  define always_inline inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#  define always_inline inline __forceinline
#else
#  error Unsupported compiler
#endif

#if defined(__GNUC__) || defined(__clang__) || defined(__ICC) || defined(__INTEL_COMPILER)
#  define no_inline __attribute__((noinline))
#elif defined(_MSC_VER)
#  define no_inline __declspec(noinline)
#else
#  error Unsupported compiler
#endif

#if !defined(_MSC_VER)
#  define thread_local _Thread_local
#endif

#endif