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

/**
 * @file arch.h
 * 
 * @brief Compiler-agnostic architecture macro definitions.
 * 
 * @details Avoid including this header directly.
 */

#ifndef CCO_ARCH_H_INCLUDED
#define CCO_ARCH_H_INCLUDED

#ifndef CCO_H_INCLUDED
#  error "#include <cco.h> instead of this file directly"
#endif

/** Compiler-agnostic architecture definition */
#define CCO_ARCH_x86     1  /**< x86 architecture */
#define CCO_ARCH_x86_64  2  /**< x86_64 architecture */
#define CCO_ARCH_IA64    3  /**< Itanium architecture */
#define CCO_ARCH_ARM     4  /**< ARM architecture */
#define CCO_ARCH_ARM64   5  /**< ARM64 architecture */
#define CCO_ARCH_MIPS    6  /**< MIPS architecture */
#define CCO_ARCH_MIPS64  7  /**< MIPS64 architecture */
#define CCO_ARCH_PPC     8  /**< PowerPC architecture */
#define CCO_ARCH_PPC64   9  /**< PowerPC64 architecture */
#define CCO_ARCH_RISCV   10 /**< RISC-V architecture*/
#define CCO_ARCH_RISCV64 11 /**< RISC-V 64-bit architecture*/
#define CCO_ARCH_SPARC   12 /**< SPARC architecture */
#define CCO_ARCH_SPARC64 13 /**< SPARC 64-bit architecture */

/* If the architecture to compile for is not set from the build system, the library will try to detect it automatically. */
#ifndef CCO_TARGET_ARCH
/** Compiler-agnostic architecture definition */
#  if defined(__GNUC__) || defined(__clang__) || defined(__ICC) || defined(__INTEL_COMPILER)
#    if defined(__i386__)
#      define CCO_TARGET_ARCH        CCO_ARCH_X86
#    elif defined(__x86_64__)
#      define CCO_TARGET_ARCH        CCO_ARCH_X86_64
#    elif defined(__arm__)
#      define CCO_TARGET_ARCH        CCO_ARCH_ARM
#    elif defined(__ia64__)
#      define CCO_TARGET_ARCH        CCO_ARCH_IA64
#    elif defined(__aarch64__)
#      define CCO_TARGET_ARCH        CCO_ARCH_ARM64
#    elif defined(__mips__)
#      define CCO_TARGET_ARCH        CCO_ARCH_MIPS
#    elif defined(__mips64) || defined(__mips64__)
#      define CCO_TARGET_ARCH        CCO_ARCH_MIPS64
#    elif defined(__powerpc__)
#      define CCO_TARGET_ARCH        CCO_ARCH_PPC
#    elif defined(__powerpc64__)
#      define CCO_TARGET_ARCH        CCO_ARCH_PPC64
#    elif defined(__riscv)
#      define CCO_TARGET_ARCH        CCO_ARCH_RISCV
#    elif defined(__riscv64)
#      define CCO_TARGET_ARCH        CCO_ARCH_RISCV64
#    elif defined(__sparc__)
#      define CCO_TARGET_ARCH        CCO_ARCH_SPARC
#    elif defined(__sparc64__)
#      define CCO_TARGET_ARCH        CCO_ARCH_SPARC64
#    else
#      error Architecture not supported
#    endif
#  elif defined(_MSC_VER)
#    define stdcall __stdcall
#    if defined(_M_IX86)
#      define CCO_TARGET_ARCH CCO_ARCH_X86
#    elif defined(_M_X64) || defined(_M_AMD64)
#      define CCO_TARGET_ARCH CCO_ARCH_X86_64
#    elif defined(_M_ARM)
#      define CCO_TARGET_ARCH CCO_ARCH_ARM
#    elif defined(_M_ARM64)
#      define CCO_TARGET_ARCH CCO_ARCH_ARM64
#    else
#      error Architecture not supported
#    endif
#  else
#    error Compiler not supported
#  endif
#endif

#if CCO_TARGET_ARCH == CCO_ARCH_x86
#  define CCO_TARGET_ARCH_STRING "x86"
#  define CCO_TARGET_ARCH_HEADER "cco/arch/x86.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_x86_64
#  define CCO_TARGET_ARCH_STRING "x86_64"
#  define CCO_TARGET_ARCH_HEADER "cco/arch/x86_64.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_IA64
#  define CCO_TARGET_ARCH_STRING "ia64"
#  define CCO_TARGET_ARCH_HEADER "cco/arch/ia64.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_ARM
#  define CCO_TARGET_ARCH_STRING "arm"
#  define CCO_TARGET_ARCH_HEADER "cco/arch/arm.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_ARM64
#  define CCO_TARGET_ARCH_STRING "aarch64"
#  define CCO_TARGET_ARCH_HEADER "cco/arch/aarch64.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_MIPS
#  define CCO_TARGET_ARCH_STRING "mips"
#  define CCO_TARGET_ARCH_HEADER "cco/arch/mips.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_MIPS64
#  define CCO_TARGET_ARCH_STRING "mips64"
#  define CCO_TARGET_ARCH_HEADER "cco/arch/mips64.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_PPC
#  define CCO_TARGET_ARCH_STRING "ppc"
#  define CCO_TARGET_ARCH_HEADER "cco/arch/ppc.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_PPC64
#  define CCO_TARGET_ARCH_STRING "ppc64"
#  define CCO_TARGET_ARCH_HEADER "cco/arch/ppc64.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_RISCV
#  define CCO_TARGET_ARCH_STRING "riscv"
#  define CCO_TARGET_ARCH_HEADER "cco/arch/riscv.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_RISCV64
#  define CCO_TARGET_ARCH_STRING "riscv64"
#  define CCO_TARGET_ARCH_HEADER "cco/arch/riscv64.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_SPARC
#  define CCO_TARGET_ARCH_STRING "sparc"
#  define CCO_TARGET_ARCH_HEADER "cco/arch/sparc.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_SPARC64
#  define CCO_TARGET_ARCH_STRING "sparc64"
#  define CCO_TARGET_ARCH_HEADER "cco/arch/sparc64.h"
#else
#  error Architecture not supported
#endif

#endif
