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
 * @file test/example.c
 * 
 * @brief Test file for x86 architecture.
 * 
 * @details Test file to be run either on a x86 machine or on a x86 emulator such as qemu
 * to test the cco library. The test is performed by running a coroutine that prints
 * a message and then yields to the main context several times.
 * 
 * @note Not yet ready to be considered a full test, just a usage example.
 */

#include "cco.h"

#include <stdio.h>

cco_coroutine* coroutines[2];

void
hello(void* arg)
{
    printf("Hello from %s\n", (const char*)arg);
}

void
hello_inner(const char* arg, int i)
{
    printf("Hello from %s (%d)\n", arg, i);
}

void
hello_loop(void* arg)
{
    for(int i = 0; i != 10; ++i) {
        hello_inner((const char*)arg, i);
        printf("Coroutine 0 state: %s\n", cco_coroutine_state_strings[cco_coroutine_get_state(coroutines[0])]);
        printf("Coroutine 1 state: %s\n", cco_coroutine_state_strings[cco_coroutine_get_state(coroutines[1])]);
        cco_yield(&i);
    }
    printf("Goodbye from %s\n", (const char*)arg);
}

int
main(void)
{
    for(int i = 0; i != 2; ++i) {
        /*  Values below 4096 crash when trying to use printf. I suspect printf allocates some buffer on the stack,
            and using a stack that is too small causes a stack overflow.
        */
        coroutines[i] = cco_coroutine_create(4096, NULL);
    }

    cco_coroutine_start(coroutines[0], hello_loop, "coroutine 0");
    cco_coroutine_start(coroutines[1], hello_loop, "coroutine 1");

    for(int i = 0; i != 10; ++i) {
        for(int j = 0; j != 2; ++j) {
            cco_resume(coroutines[j]);
            if(cco_errno != CCO_OK) {
                printf("Error: %s (%d) in coroutine %d at iteration %d (main)\n", cco_strerror(cco_errno), cco_errno, j, i);
            }
        }
        printf("(main) Coroutine 0 state: %s\n", cco_coroutine_state_strings[cco_coroutine_get_state(coroutines[0])]);
        printf("(main) Coroutine 1 state: %s\n", cco_coroutine_state_strings[cco_coroutine_get_state(coroutines[1])]);
    }
    cco_coroutine_destroy(coroutines[0]);
    cco_coroutine_destroy(coroutines[1]);
}
