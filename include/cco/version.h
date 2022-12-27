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
 * @file version.h
 * 
 * @brief Library version struct definition and object declaration.
 * 
 * @details Avoid including this header directly.
 */

#ifndef CCO_VERSION_H_INCLUDED
#define CCO_VERSION_H_INCLUDED

#ifndef CCO_H_INCLUDED
#  error "#include <cco.h> instead of this file directly"
#endif

typedef struct {
    int major;
    int minor;
    int patch;
} cco_version;

CCO_API const cco_version cco_lib_version;

#endif
