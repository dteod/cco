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

#include "api.h"
#include "errno.h"

#ifdef __cplusplus
extern "C" {
#endif

CCO_PRIVATE thread_local cco_error  cco_errno_instance = CCO_OK;
CCO_PRIVATE thread_local cco_error* cco_errno_loc;

void ctor
cco_errno_init(void)
{
    cco_errno_loc = &cco_errno_instance;
}

CCO_API_INTERNAL const cco_error*
cco_errno_ptr(void)
{
    return cco_errno_loc;
}

cco_error*
cco_errno_location(void)
{
    return cco_errno_loc;
}

CCO_API_INTERNAL const char*
cco_strerror(cco_error error)
{
    switch(error) {
    case CCO_OK: return "no error";
    case CCO_ERROR_NO_MEMORY: return "memory allocation failed";
    case CCO_ERROR_INVALID_CONTEXT: return "invalid context";
    case CCO_ERROR_INVALID_ARGUMENT: return "invalid argument";
    case CCO_ERROR_SCHEDULED: return "coroutine was scheduled";
    case CCO_ERROR_UNSCHEDULED: return "coroutine was not scheduled";
    case CCO_ERROR_NOT_SUSPENDED: return "coroutine was not suspended";
    case CCO_ERROR_NOT_RUNNING: return "coroutine was not running";
    default: return "unknown error";
    }
}

#ifdef __cplusplus
}
#endif
