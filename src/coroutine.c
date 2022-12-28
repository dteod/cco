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
 * @file cco.h
 * 
 * @brief Umbrella header file for coroutine library, extended to support a broader CPU register set.
 * 
 * @details This file includes all the other headers of the library. Avoid including the other headers directly.
 */

#include "api.h"
#include "cco.h"
#include "compiler.h"
#include "errno.h"
#include "memory.h"

/**
 * @brief Struct describing the context of a CPU.
 * 
 * @details Forward declared and implemented in arch.h, to be included after this declaration,
 * the definition of cco_coroutine, and that of cco_coroutine_entry_point. Varies with the target processor
 * and the compile-time options currently set.
 */
typedef struct cco_cpu_context cco_cpu_context;

/**
 * @brief Entry point to every coroutine function.
 * 
 * @details Wraps the callback function of the coroutine and cco_return(NULL) to avoid returning to an invalid address.
 * 
 * @param coroutine The coroutine containing the callback to call, together with its argument.
 */
CCO_PRIVATE void cco_coroutine_entry_point(cco_coroutine* coroutine);

struct cco_coroutine {
    cco_cpu_context*       context;
    cco_coroutine*         caller;
    cco_coroutine_callback callback;
    void*                  arg;
    void*                  return_value;
    cco_coroutine_promise  promise;
    cco_coroutine_state    state;
    size_t                 stack_size;
    uint8_t*               stack;
    cco_await_callback     await_ready;
    cco_await_callback     await_on_suspend;
};

// Functions included by arch.h, to be implemented on each processor:
// CCO_PRIVATE always_inline void cco_prepare_coroutine(cco_coroutine* coroutine);
// CCO_PRIVATE always_inline void cco_cswitch(cco_cpu_context* prev, cco_cpu_context* next);

#define CCO_COROUTINE_IMPLEMENTATION
#include "arch.h"

/**
 * @brief Thread-local context of the main coroutine.
 * 
 * @details The "main coroutine" concept is not exposed to the user: it is just named as "main context"
 * in the documentation. The main context is the context of the main thread, and it is used to run the
 * main function of the program. It is not a coroutine, but it is used to run the first
 * coroutine created by the user, to store the context of the main function and to keep track of where
 * the coroutine shall yield control to when it terminates, either with cco_yield() or cco_return().
 * 
 * @note Each thread in a program may start and/or resume a coroutine, and each of these contexts
 * shall be saved. This is why the main context is thread-local.
 */
CCO_PRIVATE thread_local cco_cpu_context cco_main_context;

/**
 * @brief Thread-local storage for the main coroutine.
 * 
 * @details The main coroutine is not exposed to the user, but it is used to store the context of the
 * main function and to keep track of where the coroutine shall yield control to when it terminates.
 * Also, its mere presence is useful to understand if the current context is the main context or not,
 * by comparing the current coroutine with the main coroutine. This is actually how the library
 * handles the main coroutine: data such as the return value, the stack/stack size and the state are
 * not used, and the context is not used to switch to the main context, but only to store the context
 * of the main function.
 * 
 * @note The main coroutine is thread local for the same reason why the main context is thread-local.
 */
CCO_PRIVATE thread_local cco_coroutine cco_main_coroutine;

/**
 * @brief Thread-local pointer of the current coroutine.
 * 
 * @details The current coroutine is the coroutine that is currently running on the current thread.
 * It is thread-local because each thread may run a different coroutine at the same time.
 * The variable is loaded with the address of the main coroutine when the library is initialized,
 * and it is updated every time a coroutine is switched to.
 * 
 * @note It does not need to be atomic because it is thread-local.
 */
CCO_PRIVATE thread_local cco_coroutine* volatile cco_current_coroutine = NULL;

CCO_PRIVATE bool
cco_await_true_callback(cco_coroutine* coroutine, void* argument)
{
    (void)coroutine;
    (void)argument;
    return true;
}

CCO_PRIVATE bool
cco_await_false_callback(cco_coroutine* coroutine, void* argument)
{
    (void)coroutine;
    (void)argument;
    return false;
}

CCO_API_INTERNAL const cco_await_callback cco_await_suspend_always = cco_await_false_callback;

CCO_API_INTERNAL const cco_await_callback cco_await_suspend_never = cco_await_true_callback;

/**
 * @brief Constructor function of the library, called before main() and used to initialize the global variables.
 */
CCO_PRIVATE void ctor
cco_init()
{
    cco_main_coroutine.context = &cco_main_context;
    cco_main_coroutine.state   = CCO_COROUTINE_STATE_RUNNING;
    cco_current_coroutine      = &cco_main_coroutine;

    /*
     * The following is a hack to avoid a random crash happening on printf ran from a coroutine.
     * The crash happens because the stack is not initialized, and the printf function tries to
     * access the stack to print the string.
     */
    printf("%s", "");
}

CCO_API_INTERNAL cco_coroutine*
cco_coroutine_create(size_t stack_size)
{
    cco_coroutine* out = (cco_coroutine*)cco_alloc(sizeof(cco_coroutine));
    if(out) {
        out->stack_size = stack_size;
        out->stack      = (uint8_t*)cco_alloc(stack_size);
        if(!out->stack) {
            cco_free(out);
            out                = NULL;
            cco_errno_instance = CCO_ERROR_NO_MEMORY;
        }
        else {
            out->context = (cco_cpu_context*)cco_alloc(sizeof(cco_cpu_context));
            if(!out->context) {
                cco_free(out->stack);
                cco_free(out);
                out                = NULL;
                cco_errno_instance = CCO_ERROR_NO_MEMORY;
            }
            else {
                out->state         = CCO_COROUTINE_STATE_UNSCHEDULED;
                cco_errno_instance = CCO_OK;
            }
        }
    }
    else {
        cco_errno_instance = CCO_ERROR_NO_MEMORY;
    }
    return out;
}

CCO_API_INTERNAL void
cco_coroutine_destroy(cco_coroutine* coroutine)
{
    if(coroutine) {
        if(coroutine != &cco_main_coroutine) {
            // cco_cpu_context return_context;
            // memcpy(&return_context, coroutine->context, sizeof(cco_cpu_context));
            // cco_free(coroutine->stack);
            // TODO this will be a problem. We're pulling the rug out from under our feet:
            // to perform a context switch we are using the stack of the coroutine we're destroying.
            // We need to find a way to switch to the main context without using the stack of the
            // coroutine we're destroying. Maybe we can use the stack of the main coroutine?
            // For now I will just add a check to not destroy the current coroutine.
            if(coroutine == cco_current_coroutine) {
                cco_errno_instance = CCO_ERROR_INVALID_CONTEXT;
                return;
            }
            cco_free(coroutine->stack);
            cco_free(coroutine->context);
            cco_free(coroutine);
            cco_errno_instance = CCO_OK;
        }
        else {
            cco_errno_instance = CCO_ERROR_INVALID_ARGUMENT;
        }
    }
    else {
        cco_errno_instance = CCO_ERROR_INVALID_ARGUMENT;
    }
}

CCO_PRIVATE no_inline void
cco_coroutine_entry_point(cco_coroutine* coroutine)
{
    coroutine->state = CCO_COROUTINE_STATE_RUNNING;
    coroutine->callback(coroutine->arg);
    cco_return(NULL);
}

CCO_API_INTERNAL bool
cco_coroutine_start(cco_coroutine* coroutine, cco_coroutine_callback callback, void* arg)
{
    bool ret = false;
    if(coroutine) {
        if(coroutine->state == CCO_COROUTINE_STATE_UNSCHEDULED) {
            if(callback) {
                ret                         = true;
                cco_errno_instance          = CCO_OK;
                coroutine->callback         = callback;
                coroutine->arg              = arg;
                coroutine->caller           = cco_current_coroutine;
                coroutine->await_ready      = cco_await_suspend_always;
                coroutine->await_on_suspend = NULL;
                cco_prepare_coroutine(coroutine);
                cco_current_coroutine = coroutine;
                cco_cswitch(coroutine->caller->context, coroutine->context);
                /*
                    Notice that after cco_cswitch we will return in the context of the coroutine, hence we will
                    come back to this context only when the coroutine will yield or return (explicitly or implicitly).
                */
                cco_current_coroutine = coroutine->caller;
            }
            else {
                cco_errno_instance = CCO_ERROR_INVALID_ARGUMENT;
            }
        }
        else {
            cco_errno_instance = CCO_ERROR_SCHEDULED;
        }
    }
    else {
        cco_errno_instance = CCO_ERROR_INVALID_ARGUMENT;
    }
    return ret;
}

CCO_API_INTERNAL cco_coroutine*
cco_this_coroutine()
{
    cco_errno_instance = CCO_OK;
    return cco_current_coroutine != &cco_main_coroutine ? cco_current_coroutine : NULL;
}

CCO_API_INTERNAL void
cco_return(void* value)
{
    if(cco_current_coroutine == &cco_main_coroutine) {
        cco_errno_instance = CCO_ERROR_INVALID_CONTEXT;
    }
    else {
        cco_cpu_context* context            = cco_current_coroutine->context;
        cco_coroutine*   caller             = cco_current_coroutine->caller;
        cco_errno_instance                  = CCO_OK;
        cco_current_coroutine->return_value = value;
        cco_current_coroutine->state        = CCO_COROUTINE_STATE_UNSCHEDULED;
        cco_cswitch(context, (cco_current_coroutine = caller)->context);
    }
}

CCO_API_INTERNAL void
cco_suspend()
{
    if(cco_current_coroutine == &cco_main_coroutine) {
        cco_errno_instance = CCO_ERROR_INVALID_CONTEXT;
    }
    else {
        cco_cpu_context* context     = cco_current_coroutine->context;
        cco_coroutine*   caller      = cco_current_coroutine->caller;
        cco_errno_instance           = CCO_OK;
        cco_current_coroutine->state = CCO_COROUTINE_STATE_SUSPENDED;
        cco_cswitch(context, (cco_current_coroutine = caller)->context);
    }
}

#include <stdatomic.h>

CCO_API_INTERNAL void
cco_resume(cco_coroutine* coroutine)
{
    if(coroutine) {
        if(coroutine->state == CCO_COROUTINE_STATE_SUSPENDED) {
            cco_errno_instance = CCO_OK;
            coroutine->caller  = cco_current_coroutine;
            coroutine->state   = CCO_COROUTINE_STATE_RUNNING;
            cco_cswitch(coroutine->caller->context, (cco_current_coroutine = coroutine)->context);
        }
        else {
            cco_errno_instance = CCO_ERROR_NOT_SUSPENDED;
        }
    }
    else {
        cco_errno_instance = CCO_ERROR_INVALID_ARGUMENT;
    }
}

CCO_API_INTERNAL void
cco_yield(void* value)
{
    if(cco_current_coroutine == &cco_main_coroutine) {
        cco_errno_instance = CCO_ERROR_INVALID_CONTEXT;
    }
    else {
        cco_cpu_context* context            = cco_current_coroutine->context;
        cco_coroutine*   caller             = cco_current_coroutine->caller;
        cco_errno_instance                  = CCO_OK;
        cco_current_coroutine->return_value = value;
        cco_current_coroutine->state        = CCO_COROUTINE_STATE_SUSPENDED;
        cco_cswitch(context, (cco_current_coroutine = caller)->context);
    }
}

CCO_API_INTERNAL void
cco_register_awaitable(cco_await_callback ready, cco_await_callback on_suspend)
{
    if(cco_current_coroutine) {
        cco_current_coroutine->await_ready      = ready;
        cco_current_coroutine->await_on_suspend = on_suspend;
        cco_errno_instance                      = CCO_OK;
    }
    else {
        cco_errno_instance = CCO_ERROR_INVALID_CONTEXT;
    }
}

CCO_API_INTERNAL void
cco_await(void* arg)
{
    cco_await_with(cco_current_coroutine->await_ready, cco_current_coroutine->await_on_suspend, arg);
}

CCO_API_INTERNAL void
cco_await_with(cco_await_callback ready, cco_await_callback on_suspend, void* arg)
{
    cco_cpu_context* context = cco_current_coroutine->context;
    cco_coroutine*   caller  = cco_current_coroutine->caller;
    if(cco_current_coroutine != &cco_main_coroutine) {
        if(!(ready || on_suspend)) {
            cco_errno_instance = CCO_ERROR_INVALID_ARGUMENT;
            return;
        }
        cco_errno_instance = CCO_OK;
        while(true) {
            if(ready && ready(cco_current_coroutine, arg)) {
                return;
            }
            else {
                cco_current_coroutine->state = CCO_COROUTINE_STATE_SUSPENDED;
                if(!on_suspend || on_suspend(cco_current_coroutine, arg)) {
                    goto suspend;
                }
            }
        }
    }
    else {
        cco_errno_instance = CCO_ERROR_INVALID_CONTEXT;
    }
suspend:
    cco_cswitch(context, (cco_current_coroutine = caller)->context);
}

CCO_API_INTERNAL const char* const cco_coroutine_state_strings[] = {
    [CCO_COROUTINE_STATE_NONE]        = "none",
    [CCO_COROUTINE_STATE_RUNNING]     = "running",
    [CCO_COROUTINE_STATE_SUSPENDED]   = "suspended",
    [CCO_COROUTINE_STATE_UNSCHEDULED] = "unscheduled",
};

CCO_API_INTERNAL cco_coroutine_state
cco_coroutine_get_state(const cco_coroutine* coroutine)
{
    if(coroutine) {
        cco_errno_instance = CCO_OK;
        switch(coroutine->state) {
        case CCO_COROUTINE_STATE_RUNNING:      // fallthrough;
        case CCO_COROUTINE_STATE_SUSPENDED:    // fallthrough;
        case CCO_COROUTINE_STATE_UNSCHEDULED:  // fallthrough;
            {
                cco_errno_instance = CCO_OK;
                return coroutine->state;
            }
        default:
            {
                cco_errno_instance = CCO_ERROR_INVALID_ARGUMENT;
                return CCO_COROUTINE_STATE_NONE;
            }
        }
    }
    else {
        cco_errno_instance = CCO_ERROR_INVALID_ARGUMENT;
        return CCO_COROUTINE_STATE_NONE;
    }
}

CCO_API_INTERNAL size_t
cco_coroutine_get_stack_size(const cco_coroutine* coroutine)
{
    if(coroutine) {
        if(coroutine != &cco_main_coroutine) {
            cco_errno_instance = CCO_OK;
            return coroutine->stack_size;
        }
        else {
            // TODO check if there is a way to get the stack size of the main coroutine
            // if(coroutine == cco_current_coroutine) {
            // } else {
            // }
            cco_errno_instance = CCO_ERROR_INVALID_CONTEXT;
            return 0;
        }
    }
    else {
        cco_errno_instance = CCO_ERROR_INVALID_ARGUMENT;
        return 0;
    }
}

CCO_API_INTERNAL size_t cco_coroutine_get_stack_usage(const cco_coroutine* coroutine);

CCO_API_INTERNAL void*
cco_coroutine_get_return_value(const cco_coroutine* coroutine)
{
    if(coroutine) {
        if(coroutine != &cco_main_coroutine) {
            cco_errno_instance = CCO_OK;
            return coroutine->return_value;
        }
        else {
            cco_errno_instance = CCO_ERROR_INVALID_CONTEXT;
            return NULL;
        }
    }
    else {
        cco_errno_instance = CCO_ERROR_INVALID_ARGUMENT;
        return NULL;
    }
}

CCO_API_INTERNAL cco_coroutine_promise*
cco_coroutine_get_promise(cco_coroutine* coroutine)
{
    cco_coroutine_promise* promise = NULL;
    if(coroutine) {
        if(coroutine != &cco_main_coroutine) {
            cco_errno_instance = CCO_OK;
            promise            = &coroutine->promise;
        }
        else {
            cco_errno_instance = CCO_ERROR_INVALID_CONTEXT;
        }
    }
    else {
        cco_errno_instance = CCO_ERROR_INVALID_ARGUMENT;
    }
    return promise;
}
