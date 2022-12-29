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
 * @file test/black_box.cpp
 * 
 * @brief Black box testing for the cco library.
 * 
 * @details Test file to be run with Catch2.
 * Contains several tests useful to verify the correct behavior of the library
 * from a user's point of view.
 * 
 * 
 * @note Not yet ready to be considered a full test.
 */

#include <cco.h>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

static constexpr unsigned int CCO_DEFAULT_STACK_SIZE = 1024 * 1024;

TEST_CASE("Test 1: Create a valid coroutine", "[cco]")
{
    cco_coroutine* coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    REQUIRE(coroutine != NULL);
    REQUIRE(cco_coroutine_get_state(coroutine) == CCO_COROUTINE_STATE_UNSCHEDULED);
    cco_coroutine_destroy(coroutine);
}

TEST_CASE("Test 2: Schedule a coroutine for execution", "[cco]")
{
    cco_coroutine* coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    int            argument  = 0;
    REQUIRE(coroutine != NULL);
    REQUIRE(cco_coroutine_start(
        coroutine, [](void* argument) { *(int*)argument = 3; }, &argument
    ));
    REQUIRE(cco_coroutine_get_state(coroutine) == CCO_COROUTINE_STATE_UNSCHEDULED);
    REQUIRE(argument == 3);
    cco_coroutine_destroy(coroutine);
}

TEST_CASE("Test 3: Suspend a coroutine", "[cco]")
{
    cco_coroutine* coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    REQUIRE(coroutine != NULL);
    REQUIRE(cco_coroutine_start(
        coroutine, [](void*) { cco_suspend(); }, NULL
    ));
    REQUIRE(cco_coroutine_get_state(coroutine) == CCO_COROUTINE_STATE_SUSPENDED);
    cco_coroutine_destroy(coroutine);
}

TEST_CASE("Test 4: Resume a coroutine after yielding control", "[cco]")
{
    cco_coroutine* coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    REQUIRE(coroutine != NULL);

    int  argument = 0;
    int* ret      = nullptr;

    REQUIRE(cco_coroutine_start(
        coroutine,
        [](void* argument) -> void {
            int* value = reinterpret_cast<int*>(argument);
            cco_suspend();
            *value = 1;
            cco_suspend();
            *value = 2;
            cco_return(value);
        },
        &argument
    ));
    REQUIRE(argument == 0);
    REQUIRE(cco_coroutine_get_state(coroutine) == CCO_COROUTINE_STATE_SUSPENDED);

    cco_resume(coroutine);
    REQUIRE(argument == 1);
    REQUIRE(cco_coroutine_get_state(coroutine) == CCO_COROUTINE_STATE_SUSPENDED);

    cco_resume(coroutine);
    REQUIRE(argument == 2);
    REQUIRE(cco_coroutine_get_state(coroutine) == CCO_COROUTINE_STATE_UNSCHEDULED);

    ret = reinterpret_cast<int*>(cco_coroutine_get_return_value(coroutine));
    REQUIRE(&argument == ret);

    cco_coroutine_destroy(coroutine);
}
