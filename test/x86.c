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
 * @file test/x86.h
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

void
hello(void* arg)
{
    printf("Hello from %s\n", (const char*)arg);
}

void
black_box_test()
{
    cco_coroutine* coroutine = cco_coroutine_create(1024);
    hello("black_box_test (in)");
    cco_coroutine_start(coroutine, hello, "coroutine");
    hello("black_box_test (out)");
    cco_coroutine_destroy(coroutine);
}

int
main()
{
    hello("main (enter)");
    black_box_test();
    hello("main (exit)");
}