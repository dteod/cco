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

#ifndef CCO_SRC_API_H_INCLUDED
#define CCO_SRC_API_H_INCLUDED

#include "cco.h"

#if CCO_SHARED
#  ifdef _MSC_VER
#    if CCO_INTERNAL
#      define CCO_API_INTERNAL __declspec(dllexport)
#    else
#      define CCO_API_INTERNAL __declspec(dllimport)
#    endif
#  elif __GNUC__ >= 4
#    define CCO_API_INTERNAL __attribute__((visibility("default")))
#  endif
#else
#  define CCO_API_INTERNAL
#endif
#define CCO_PRIVATE static

#endif
