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

#ifndef CCO_SRC_MEMORY_H_INCLUDED
#define CCO_SRC_MEMORY_H_INCLUDED

#include "api.h"
#include "compiler.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#  include <malloc.h>
#endif

always_inline CCO_PRIVATE void*
cco_alloc(size_t size)
{
    return malloc(size);
    // TODO use a custom allocator for static sizes
}

always_inline CCO_PRIVATE void*
cco_aligned_alloc(size_t size, size_t alignment)
{
#if defined(_WIN32) || defined(_WIN64)
    return _aligned_malloc(size, alignment);
#elif __unix__
    void* ptr;
    if(posix_memalign(&ptr, alignment, size) != 0) {
        ptr = NULL;
    }
    for(size_t i = 0; i != size; ++i) {
        ((char*)ptr)[i] = 0;
    }
    // printf("allocated %p with alignment %zu and size %zu (posix_memalign)\n", ptr, alignment, size);
    return ptr;
    // TODO use a custom allocator for static sizes
#else
#  error "Unsupported platform (aligned malloc required)"
#endif
}

always_inline CCO_PRIVATE void
cco_free(void* ptr)
{
    free(ptr);
    // TODO use a custom allocator for static sizes
}

always_inline CCO_PRIVATE void
cco_aligned_free(void* ptr)
{
#if defined(_WIN32) || defined(_WIN64)
    _aligned_free(ptr);
#elif __unix__
    free(ptr);
#else
#  error "Unsupported platform (aligned free required)"
#endif
    // TODO use a custom allocator for static sizes
}

#endif