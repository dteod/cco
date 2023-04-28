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
 * @file errno.h
 * 
 * @brief Error codes definitions and library errno.
 * 
 * @details Avoid including this header directly.
 */

#ifndef CCO_ERRNO_H_INCLUDED
#define CCO_ERRNO_H_INCLUDED

#ifndef CCO_H_INCLUDED
#  error "#include <cco.h> instead of this file directly"
#endif

/** Error codes */
typedef enum {
    CCO_OK,                     /**< No error */
    CCO_ERROR_NO_MEMORY,        /**< Out of memory */
    CCO_ERROR_INVALID_CONTEXT,  /**< Coroutine function called from a context which is not a coroutine */
    CCO_ERROR_INVALID_ARGUMENT, /**< Invalid argument */
    CCO_ERROR_SCHEDULED,        /**< Coroutine is already scheduled */
    CCO_ERROR_UNSCHEDULED,      /**< Coroutine is not scheduled */
    CCO_ERROR_NOT_SUSPENDED,    /**< Coroutine is not suspended */
    CCO_ERROR_NOT_RUNNING,      /**< Coroutine is not running */
} cco_error;

/** Error code pointer of the current thread */
CCO_API const cco_error* cco_errno_ptr(void);

/** Error code of the current thread */
#define cco_errno (*cco_errno_ptr())

CCO_API const char* cco_strerror(cco_error error);

#endif
