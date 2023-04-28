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

#include <chrono>
#include <random>
#include <thread>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

static constexpr unsigned int CCO_DEFAULT_STACK_SIZE = 4096 * 4;

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

    int argument = 0;
    REQUIRE(cco_coroutine_start(
        coroutine,
        [](void* argument) -> void {
            int* value = reinterpret_cast<int*>(argument);
            cco_suspend();
            *value = 1;
            cco_suspend();
            *value = 2;
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

    cco_coroutine_destroy(coroutine);
}

TEST_CASE("Test 5: Return a value from the coroutine stack", "[cco]")
{
    cco_coroutine* coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    REQUIRE(coroutine != NULL);

    int* ret = nullptr;
    REQUIRE(cco_coroutine_start(
        coroutine,
        [](void*) -> void {
            int value = 42;
            cco_return(&value);
        },
        NULL
    ));
    REQUIRE(cco_coroutine_get_state(coroutine) == CCO_COROUTINE_STATE_UNSCHEDULED);
    ret = reinterpret_cast<int*>(cco_coroutine_get_return_value(coroutine));
    REQUIRE(*ret == 42);

    cco_coroutine_destroy(coroutine);
}

TEST_CASE("Test 6: Yield values from a coroutine", "[cco]")
{
    cco_coroutine* coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    REQUIRE(coroutine != NULL);

    int argument = 4;
    REQUIRE(cco_coroutine_start(
        coroutine,
        [](void* argument) -> void {
            int* arg   = reinterpret_cast<int*>(argument);
            int  value = 10 + *arg;
            cco_yield(&value);

            double value2 = 3.14 + *arg * 2;
            cco_yield(&value2);
        },
        &argument
    ));
    REQUIRE(cco_coroutine_get_state(coroutine) == CCO_COROUTINE_STATE_SUSPENDED);
    {
        int* yielded = reinterpret_cast<int*>(cco_coroutine_get_return_value(coroutine));
        REQUIRE(*yielded == 14);
    }
    argument = 5;
    cco_resume(coroutine);
    {
        double* yielded = reinterpret_cast<double*>(cco_coroutine_get_return_value(coroutine));
        REQUIRE(*yielded == 3.14 + 5 * 2);
    }
    REQUIRE(cco_coroutine_get_state(coroutine) == CCO_COROUTINE_STATE_SUSPENDED);
    cco_resume(coroutine);
    REQUIRE(cco_coroutine_get_state(coroutine) == CCO_COROUTINE_STATE_UNSCHEDULED);
    REQUIRE(cco_coroutine_get_return_value(coroutine) == nullptr);

    cco_coroutine_destroy(coroutine);
}

TEST_CASE("Test 7: Error when trying to resume an invalid coroutine", "[cco]")
{
    cco_resume(nullptr);
    REQUIRE(cco_errno == CCO_ERROR_INVALID_ARGUMENT);
}

TEST_CASE("Test 8: Error when trying to resume a coroutine that is not suspended", "[cco]")
{
    cco_coroutine* coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    REQUIRE(coroutine != NULL);
    cco_resume(coroutine);
    REQUIRE(cco_errno == CCO_ERROR_NOT_SUSPENDED);
    cco_coroutine_destroy(coroutine);
}

TEST_CASE("Test 9: Error when trying to resume a coroutine that is already running", "[cco]")
{
    cco_coroutine* coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    cco_error      err       = CCO_OK;
    REQUIRE(coroutine != NULL);
    REQUIRE(cco_coroutine_start(
        coroutine,
        [](void* err) {
            cco_resume(cco_this_coroutine());
            *reinterpret_cast<cco_error*>(err) = cco_errno;
        },
        &err
    ));
    REQUIRE(cco_errno == CCO_OK);
    REQUIRE(err == CCO_ERROR_NOT_SUSPENDED);
    cco_coroutine_destroy(coroutine);
}

TEST_CASE("Test 10: Error when trying to resume a coroutine that is already finished", "[cco]")
{
    cco_coroutine* coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    REQUIRE(coroutine != NULL);
    REQUIRE(cco_coroutine_start(
        coroutine,
        [](void*) {
            int value = 42;
            cco_return(&value);
        },
        NULL
    ));
    REQUIRE(cco_coroutine_get_state(coroutine) == CCO_COROUTINE_STATE_UNSCHEDULED);
    cco_resume(coroutine);
    REQUIRE(cco_errno == CCO_ERROR_NOT_SUSPENDED);
    cco_coroutine_destroy(coroutine);
}

TEST_CASE("Test 11: Error when trying to resume the main context from a coroutine", "[cco]")
{
    cco_coroutine* coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    cco_error      err       = CCO_OK;
    REQUIRE(coroutine != NULL);

    struct Args {
        cco_coroutine* main;
        cco_error*     err;
    } args = {cco_this_coroutine(), &err};

    REQUIRE(cco_coroutine_start(
        coroutine,
        [](void* arg) {
            Args* args = reinterpret_cast<Args*>(arg);
            cco_resume(args->main);
            *reinterpret_cast<cco_error*>(args->err) = cco_errno;
        },
        &args
    ));
    REQUIRE(err == CCO_ERROR_INVALID_ARGUMENT);
    cco_coroutine_destroy(coroutine);
}

TEST_CASE("Test 12: cco_this_coroutine() in the main context returns NULL", "[cco]")
{
    REQUIRE(cco_this_coroutine() == NULL);
}

TEST_CASE("Test 13: cco_this_coroutine() in a coroutine returns the coroutine", "[cco]")
{
    cco_coroutine* coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    REQUIRE(coroutine != NULL);
    REQUIRE(cco_coroutine_start(
        coroutine, [](void* coroutine) { REQUIRE(cco_this_coroutine() == coroutine); }, coroutine
    ));
    cco_coroutine_destroy(coroutine);
}

TEST_CASE("Test 14: Error in retrieving main context's execution state", "[cco]")
{
    REQUIRE(cco_coroutine_get_state(cco_this_coroutine()) == CCO_COROUTINE_STATE_NONE);
    REQUIRE(cco_errno == CCO_ERROR_INVALID_ARGUMENT);
}

TEST_CASE("Test 15: Error in retrieving main context's return value", "[cco]")
{
    REQUIRE(cco_coroutine_get_return_value(cco_this_coroutine()) == NULL);
    REQUIRE(cco_errno == CCO_ERROR_INVALID_ARGUMENT);
}

TEST_CASE("Test 16: Error in retrieving main context's stack size", "[cco]")
{
    REQUIRE(cco_coroutine_get_stack_size(cco_this_coroutine()) == 0);
    REQUIRE(cco_errno == CCO_ERROR_INVALID_ARGUMENT);
}

TEST_CASE("Test 17: Retrieving coroutine's stack size", "[cco]")
{
    cco_coroutine* coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    REQUIRE(coroutine != NULL);
    REQUIRE(cco_coroutine_get_stack_size(coroutine) == CCO_DEFAULT_STACK_SIZE);
    cco_coroutine_destroy(coroutine);
}

TEST_CASE("Test 18: Retrieving coroutine's stack usage", "[cco]")
{
    cco_coroutine* coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    REQUIRE(coroutine != NULL);
    cco_coroutine_start(
        coroutine,
        [](void*) {
            volatile int32_t                       v = 0;
            volatile int32_t                       values[32];
            std::mt19937                           engine;
            std::uniform_int_distribution<int32_t> dist;
            for(int i = 0; i != sizeof(values) / sizeof(int32_t); ++i) {
                values[i] = dist(engine);
            }
            for(int i = 0; i != sizeof(values) / sizeof(int32_t); ++i) {
                v = v + values[i];
            }
            cco_suspend();
        },
        NULL
    );

    REQUIRE(
        cco_coroutine_get_stack_usage(coroutine)
        > (sizeof(int32_t) * 32 + sizeof(int32_t) + sizeof(std::mt19937) + sizeof(std::uniform_int_distribution<int32_t>))
    );
    cco_coroutine_destroy(coroutine);
}

TEST_CASE("Test 19: Destroy a suspended coroutine", "[cco]")
{
    cco_coroutine* coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    REQUIRE(coroutine != NULL);
    cco_coroutine_start(
        coroutine, [](void*) { cco_suspend(); }, NULL
    );
    cco_coroutine_destroy(coroutine);
    REQUIRE(cco_errno == CCO_OK);
}

TEST_CASE("Test 20: Error destroying the current coroutine", "[cco]")
{
    cco_coroutine* coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    cco_error      err;
    REQUIRE(coroutine != NULL);
    REQUIRE(cco_coroutine_start(
        coroutine,
        [](void* err) {
            cco_coroutine_destroy(cco_this_coroutine());
            *reinterpret_cast<cco_error*>(err) = cco_errno;
        },
        &err
    ));
    REQUIRE(err == CCO_ERROR_INVALID_CONTEXT);
    cco_coroutine_destroy(coroutine);
}

TEST_CASE("Test 21: Awaiting a ready/synchronous operation does not suspend", "[cco]")
{
    cco_coroutine*              coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    std::vector<cco_coroutine*> coroutines;
    int                         result = 0;

    struct Args {
        std::vector<cco_coroutine*>* coroutines;
        int*                         result;
    } args = {&coroutines, &result};

    REQUIRE(coroutine != NULL);

    cco_coroutine_start(
        coroutine,
        [](void* args_) {
            cco_suspend();
            Args* args = reinterpret_cast<Args*>(args_);
            cco_await_with(
                cco_await_ready,
                [](cco_coroutine*, void* arg) {
                    std::vector<cco_coroutine*>* coroutines = reinterpret_cast<std::vector<cco_coroutine*>*>(arg);
                    coroutines->push_back(cco_this_coroutine());
                    return true;
                },
                args->coroutines
            );
            *reinterpret_cast<int*>(args->result) = 1;
        },
        &args
    );
    REQUIRE(result == 0);
    cco_resume(coroutine);

    REQUIRE(coroutines.size() == 0);
    // cco_resume(coroutines.back()) not needed. The coroutine ends immediately after the await operation without suspending.

    REQUIRE(result == 1);
    cco_coroutine_destroy(coroutine);
}

TEST_CASE("Test 22: Awaiting for 3 seconds on a result", "[cco]")
{
    cco_coroutine*              coroutine = cco_coroutine_create(CCO_DEFAULT_STACK_SIZE, NULL);
    std::vector<cco_coroutine*> coroutines;
    int                         result = 0;

    struct Args {
        std::vector<cco_coroutine*>* coroutines;
        int*                         result;
    } args = {&coroutines, &result};

    REQUIRE(coroutine != NULL);

    cco_coroutine_start(
        coroutine,
        [](void* args_) {
            cco_suspend();
            Args* args = reinterpret_cast<Args*>(args_);
            cco_await_with(
                cco_await_not_ready,
                [](cco_coroutine*, void* arg) {
                    std::vector<cco_coroutine*>* coroutines = reinterpret_cast<std::vector<cco_coroutine*>*>(arg);
                    coroutines->push_back(cco_this_coroutine());
                    return true;
                },
                args->coroutines
            );
            *reinterpret_cast<int*>(args->result) = 1;
        },
        &args
    );
    REQUIRE(result == 0);
    cco_resume(coroutine);

    // Suppose that the coroutine is scheduled for execution after 3 seconds in a scheduler
    std::this_thread::sleep_for(std::chrono::seconds(3));
    auto c = coroutines.back();
    coroutines.pop_back();
    cco_resume(c);

    REQUIRE(result == 1);
    cco_coroutine_destroy(coroutine);
}