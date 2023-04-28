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
 * @file cco.h
 * 
 * @brief Umbrella header file for coroutine library, extended to support a broader CPU register set.
 * 
 * @details This file includes all the other headers of the library. Avoid including the other headers directly.
 */

#ifndef CCO_H_INCLUDED
#define CCO_H_INCLUDED

/** \cond */
#ifdef __cplusplus
#  include <cstddef>
#  include <cstdint>
extern "C" {
#else
#  include <stdbool.h>
#  include <stddef.h>
#  include <stdint.h>
#endif
/** \endcond */

#include "cco/api.h"
#include "cco/arch.h"
#include CCO_TARGET_ARCH_HEADER
#include "cco/coroutine.h"
#include "cco/errno.h"
#include "cco/version.h"

#ifdef __cplusplus
}
#endif

#endif
