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

#ifndef CCO_ARCH_X86_H
#define CCO_ARCH_X86_H

typedef unsigned char cco_x86_settings;

#define CCO_SETTINGS_x86_STORE_CONTROL_REGISTERS (1 << 0)
#define CCO_SETTINGS_x86_STORE_DEBUG_REGISTERS   (1 << 1)
#define CCO_SETTINGS_x86_STORE_SEGMENT_REGISTERS (1 << 2)

typedef cco_x86_settings cco_architecture_specific_settings;

#endif