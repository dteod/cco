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
 * @file api.h
 * 
 * @brief Compiler-agnostic API declaration.
 * 
 * @details Avoid including this header directly.
 */

#ifndef CCO_API_H_INCLUDED
#define CCO_API_H_INCLUDED

#ifndef CCO_H_INCLUDED
#  error "#include <cco.h> instead of this file directly"
#endif

#if CCO_SHARED
#  ifdef _MSC_VER
#    if CCO_INTERNAL
#      define CCO_API extern __declspec(dllexport)
#    else
#      define CCO_API extern __declspec(dllimport)
#    endif
#  elif __GNUC__ >= 4
#    define CCO_API extern __attribute__((visibility("default")))
#  endif
#else
#  define CCO_API extern
#endif

#endif
