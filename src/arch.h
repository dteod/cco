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
 * @brief Umbrella header file for architecture specific headers.
 */

#ifndef CCO_SRC_ARCH_H_INCLUDED
#define CCO_SRC_ARCH_H_INCLUDED

/** @cond */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/** @endcond */

#include "compiler.h"

#if CCO_TARGET_ARCH == CCO_ARCH_X86
#  include "arch/x86.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_X86_64
#  include "arch/x86_64.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_ARM
#  include "arch/arm.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_IA64
#  include "arch/ia64.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_ARM64
#  include "arch/aarch64.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_MIPS
#  include "arch/mips.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_MIPS64
#  include "arch/mips64.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_PPC
#  include "arch/ppc.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_PPC64
#  include "arch/ppc64.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_RISCV
#  include "arch/riscv.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_RISCV64
#  include "arch/riscv64.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_SPARC
#  include "arch/sparc.h"
#elif CCO_TARGET_ARCH == CCO_ARCH_SPARC64
#  include "arch/sparc64.h"
#else
#  error Architecture not supported
#endif

#endif