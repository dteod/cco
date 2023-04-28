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
 * @file coroutine.h
 * 
 * @brief Coroutine opaque struct, enums and management functions.
 * 
 * @details Avoid including this header directly.
 */

#ifndef CCO_COROUTINE_H_INCLUDED
#define CCO_COROUTINE_H_INCLUDED

#ifndef CCO_H_INCLUDED
#  error "#include <cco.h> instead of this file directly"
#endif

/**
 * @brief Opaque struct used to manage a coroutine.
 * 
 * @details A coroutine is a detached execution context with its own stack space that can be suspended and resumed.
 * They can be used to generate multiple independent execution flows from a single thread and the application itself,
 * running in the thread, can switch between them by performing a user-space context switch. This mechanism allows
 * to avoid the overhead of creating and destroying threads, and it is particularly useful for implementing
 * cooperative multitasking efficiently. 
 * 
 * While coroutines are often associated to threads, they are orthogonal concepts. A thread is a unit of execution
 * that contends with other threads to obtain a CPU time slice, while a coroutine is a unit of execution running in its
 * own context that yields control to other coroutines cooperatively. Whenever execution on a coroutine is to be blocked
 * (e.g. waiting for I/O, or for a mutex to be released), the blocking coroutine will never actually block the thread, it
 * will just yield control to another coroutine. The thread will be blocked only when there are no more coroutines free
 * to run.
 * 
 * Proficient usage of coroutines allows to implement a cooperative multitasking scheduler with a very small footprint,
 * and it is particularly useful for embedded systems where the number of threads is limited by both the available memory
 * and the number of hardware threads.
 * 
 * Notable tasks and constructs that can be implemented using coroutines are:
 *  - Asynchronous I/O
 *  - Pipeline processing
 *  - Event loops and message passing interfaces
 *  - Infinite generators
 * 
 * Each coroutine is first created with a call to cco_coroutine_create(), and then it is scheduled for execution with a
 * call to cco_coroutine_start(). The coroutine will run until it reaches a suspension point, at which point it will
 * yield control to the caller. The caller can then decide to resume the coroutine again, or to destroy it with a call to
 * cco_coroutine_destroy(). The coroutine can also be destroyed by itself, by calling cco_coroutine_destroy() from within
 * the coroutine itself.
 * 
 * @note Coroutines are not threads. They are not bound to a specific thread and they can be resumed on any thread.
 * 
 * @warning The above note does not mean that a coroutine can be resumed concurrently on different threads. Doing so
 * will make the coroutine's execution context inconsistent, and it will result in undefined behavior. It is responsibility 
 * of the caller to manage the coroutine lifecycle in a thread-safe manner and to manage its execution.
 */
typedef struct cco_coroutine cco_coroutine;

/**
 * @brief Coroutine callback type alias.
 * 
 * @details This type alias is used to represent the coroutine's function to call when started. When a coroutine is created, 
 * it is passed a pointer to a function that will be executed in the coroutine.
 * The function takes a single void* argument, which is passed to the cco_coroutine_start function
 * together with the callback to start.
 */
typedef void (*cco_coroutine_callback)(void* argument);

/**
 * @brief Creates a new coroutine using compile-time architecture-specific settings.
 * 
 * @details This function creates a new coroutine with the given stack size and the pointer to the function to execute
 * using the context switch settings defined at compile-time, and returns a pointer to the newly created coroutine.
 * Architecture-specific settings can be actually passed to this function, in order for the underlying
 * context switch implementation to be able to use them. These usually include any architecture-specific registers
 * that must be preserved across context switches (e.g. segment registers on x86). Use NULL to use default settings defined
 * at compile-time.
 *  
 * @note The coroutine is created in a terminated state, in order for it to be started. Resuming it is undefined behavior.
 * 
 * @param stack_size The size of the stack to allocate for the coroutine.
 * @param settings A pointer to a cco_coroutine_settings struct containing the coroutine settings.
 * @return cco_coroutine* A pointer to the newly created coroutine, NULL on error.
 * 
 * @retval CCO_OK
 * @retval CCO_ERROR_NO_MEMORY
 */
CCO_API cco_coroutine* cco_coroutine_create(size_t stack_size, const cco_architecture_specific_settings* settings);

/**
 * @brief Destroys the given coroutine.
 * 
 * @details This function destroys the given coroutine, freeing all the resources associated with it.
 * This includes both the stack space and the coroutine itself.
 * 
 * @warning Memory leaks will occur if the coroutine is the unique owner of a resource that was not freed.
 * 
 * @warning The coroutine must be in a suspended state. If it is not, the behavior is undefined.
 * 
 * @param coroutine A pointer to the coroutine to destroy.
 * 
 * @retval CCO_OK
 * @retval CCO_ERROR_INVALID_ARGUMENT
 * @retval CCO_ERROR_INVALID_CONTEXT
 */
CCO_API void cco_coroutine_destroy(cco_coroutine* coroutine);

/**
 * @brief Starts the execution of the given coroutine.
 * 
 * @details This function starts the execution of @p coroutine, passing @p argument to the @p function
 * that will be executed. The coroutine will be executed in the context of the current thread, and control
 * will be yielded back to the caller when the coroutine reaches a suspension point.
 * 
 * @note The coroutine shall be in the unscheduled state.
 * 
 * @note A true return value from this function means that the coroutine has yielded or returned, but it does not
 * mean that the coroutine has terminated. Depending on its execution path, it may be possible to resume
 * the coroutine again: check cco_coroutine_get_state() to see if the coroutine can be resumed or not.
 * 
 * @param coroutine A pointer to the coroutine to start.
 * @param function The function to execute in the coroutine.
 * @param argument The argument to pass to the function.
 * @return bool Whether the coroutine was successfully started; true means the coroutine yielded or returned.
 * 
 * @retval CCO_OK
 * @retval CCO_ERROR_INVALID_ARGUMENT
 * @retval CCO_ERROR_SCHEDULED
 */
CCO_API bool cco_coroutine_start(cco_coroutine* coroutine, cco_coroutine_callback function, void* argument);

/**
 * @brief Returns a pointer to the currently running coroutine.
 * 
 * @details This function returns a pointer to the currently running coroutine. If the context in which this function
 * was invoked is the main execution context (i.e. the context in which the program was started), it returns NULL.
 * 
 * @return cco_coroutine* Pointer to the currently running coroutine (NULL if not called from a coroutine).
 * 
 * @retval CCO_OK
 */
CCO_API cco_coroutine* cco_this_coroutine(void);

/**
 * @brief Unschedules the current coroutine returning control to the caller and stores the given value.
 * 
 * @details This function unschedules the current coroutine, returning control to the caller; the
 * given value will be stored in the coroutine, and it can be retrieved using cco_coroutine_get_return_value().
 * It is safe to call this function from a non-coroutine context, and it will actually return to the calling context.
 * It is also safe to return a value pointing to the stack of the coroutine that is being returned,
 * as the coroutine will be destroyed only after this function returns. Calling cco_coroutine_start()
 * on a coroutine that already returned will set the return value to NULL again.
 * 
 * @param value A pointer to the value to return.
 * 
 * @retval CCO_OK
 * @retval CCO_ERROR_INVALID_CONTEXT
 */
CCO_API void cco_return(void* value);

/**
 * @brief Suspends the execution of the current coroutine.
 * 
 * @details This function suspends the execution of the current coroutine, and returns control to the calling context.
 * It can be resumed again using cco_resume(). 
 * 
 * @note This function shall be called from a coroutine. If it is called from the main context, it will be a no-op 
 * and errno will be set to CCO_ERROR_INVALID_CONTEXT.
 * 
 * @retval CCO_OK
 * @retval CCO_ERROR_INVALID_CONTEXT
 */
CCO_API void cco_suspend(void);

/**
 * @brief Resumes the execution of the given coroutine.
 * 
 * @details This function resumes the execution of @p coroutine, switching the execution context to the one stored
 * in the coroutine. A cco_return() call in the coroutine will return control to the caller, and the coroutine will
 * be unscheduled. Same is for a cco_suspend() call, except that the coroutine will be suspended instead of being unscheduled.
 * 
 * @param coroutine A pointer to the coroutine to resume.
 */
CCO_API void cco_resume(cco_coroutine* coroutine);

/**
 * @brief Yields the execution of the current coroutine.
 * 
 * @details This function yields the execution of the current coroutine, and returns control to the calling context,
 * while storing the given value in the coroutine. It can be resumed again using cco_resume(). The value can be retrieved
 * using cco_coroutine_get_return_value().
 * 
 * @note This function must be called from a coroutine. If it is called from the main context, the behavior is undefined.
 * 
 * @param value The value to return to the caller.
 */
CCO_API void cco_yield(void* value);

/**
 * @brief Alias for a callback to be called by the await mechanism.
 * 
 * @details The mechanism used to handle the await operation is similar to C++20 coroutines.
 * As soon as the await operation starts, a loop is opened: first, the @p ready callback is called
 * to handle a short-circuit; if it returns false, @p on_suspend is called, otherwise the coroutine
 * is resumed from the point where it was suspended.
 * During the execution of @p on_suspend the coroutine can be considered as already suspended, and it can be scheduled
 * for execution externally. If the on_suspend callback returns false, the loop is restarted and @p ready
 * is called again. This mechanism, although providing an overhead in case of short-circuit operations or
 * bad @p on_suspend implementations, allows to implement a wide range of awaitable operations.
 * 
 * @example The following example shows how to implement a simple awaitable operation that waits for a given amount of time.
 * @code {.c}
 * #include <time.h>
 * #include <cco.h>
 * 
 * struct event_loop* g_loop; // consider this as a thread running on its own
 * event_loop* loop_start(size_t max_events);
 * void loop_destroy(event_loop* loop);
 * bool schedule_event(struct event_loop* loop, cco_coroutine* coroutine, struct timespec* timestamp);
 *  
 * static bool awaitable_timer_ready(cco_coroutine* coroutine, void* argument) {
 *     (void)coroutine;
 *     struct timespec* timeout = (struct timespec*)argument;
 *     struct timespec now;
 *     clock_gettime(CLOCK_MONOTONIC, &now);
 *     return now.tv_sec > timeout->tv_sec || (now.tv_sec == timeout->tv_sec && now.tv_nsec >= timeout->tv_nsec);
 * }
 * 
 * static bool awaitable_timer_on_suspend(cco_coroutine* coroutine, void* argument) {
 *     return schedule_event(loop, cco_this_coroutine(), (struct timespec*) argument);
 * }
 * 
 * void callback() {
 *    printf("Hello, world!");
 * }
 * 
 * void cmain() {
 *     cco_await_with(awaitable_timer_ready, awaitable_timer_on_suspend, &(struct timespec){ .tv_sec = 3, .tv_nsec = 0 });
 *     callback();
 * }
 * 
 * int main() {
 *     g_loop = loop_start(128);
 *     cco_coroutine* c = cco_coroutine_create(128);
 *     cco_coroutine_start(c, cmain);
 *     // the coroutine was scheduled for execution, it will execute after three seconds
 *     // in the thread running in the event loop, and it yielded control to main. 
 * 
 *     struct timespec timeout = { .tv_sec = 5, .tv_nsec = 0 };
 *     nanosleep(&timeout, NULL);
 *     // While we are waiting on the main thread, the event loop will run the coroutine after 3 seconds.
 * 
 *     loop_destroy(g_loop);
 *     cco_coroutine_destroy(c);
 *     return 0;
 * }
 * @endcode
 */
typedef bool (*cco_await_callback)(cco_coroutine* coroutine, void* argument);

/**
 * @brief Registers the given callbacks as the default awaitable callbacks.
 * 
 * @note The callbacks are registered on the current coroutine and overwrite the previous ones.
 * 
 * @param ready The callback to call to check if the await operations can be executed synchronously.
 * @param on_suspend The callback to call to schedule the await operations asynchronously.
 */
CCO_API void cco_register_awaitable(cco_await_callback ready, cco_await_callback on_suspend);

/**
 * @brief Default awaitable `ready' callback that always executes the await operation asynchronously.
 * 
 * @note This is the callback registered by default as the ready callback.
 * 
 * @note Avoid using this callback as the on_suspend callback, they're semantically different:
 *       the on_suspend callback shall be used to schedule the coroutine for execution, while the ready
 *       callback shall be used to check if the await operation can be executed synchronously.
 */
CCO_API const cco_await_callback cco_await_not_ready;

/**
 * @brief Awaitable `ready' callback that always execute the await operation synchronously.
 * 
 * @note Avoid using this callback as the on_suspend callback, they're semantically different:
 *       the on_suspend callback shall be used to schedule the coroutine for execution, while the ready
 *       callback shall be used to check if the await operation can be executed synchronously.
 */
CCO_API const cco_await_callback cco_await_ready;

/**
 * @brief Awaits for the given awaitable operation to complete, either synchronously or asynchronously.
 * 
 * @details This function will execute the await mechanism as described in cco_await_callback.
 * Refer to the documentation of cco_await_callback for more information.
 * 
 * @note This function will use the default awaitable callbacks registered with cco_register_awaitable().
 * By default, the default callbacks are set to NULL, which means that the await operation will always
 * be executed synchronously.
 * 
 * @note This function must be called from a coroutine. If it is called from the main context, the behavior is undefined.
 * 
 * @param arg The argument to pass to the callbacks.
 */
CCO_API void cco_await(void* arg);

/**
 * @brief Awaits for the given awaitable operation to complete, either synchronously or asynchronously (explicit callbacks).
 * 
 * @details This function will execute the await mechanism as described in cco_await_callback.
 * Refer to the documentation of cco_await_callback for more information.
 * 
 * @note This function must be called from a coroutine. If it is called from the main context, the behavior is undefined.
 * 
 * @param ready The callback to call to check if the operation is ready.
 * @param on_suspend The callback to call if the operation is not ready.
 * @param arg The argument to pass to the callbacks.
 */
CCO_API void cco_await_with(cco_await_callback ready, cco_await_callback on_suspend, void* arg);

/** @brief Coroutine execution state. */
typedef enum {
    CCO_COROUTINE_STATE_NONE,        /**< Not a valid coroutine. */
    CCO_COROUTINE_STATE_UNSCHEDULED, /**< Created, but it has not been scheduled for execution yet. */
    CCO_COROUTINE_STATE_SUSPENDED,   /**< It has a stack and an execution context, but it's not currently running. */
    CCO_COROUTINE_STATE_RUNNING,     /**< It is currently running. */
} cco_coroutine_state;

CCO_API const char* const cco_coroutine_state_strings[4];

/**
 * @brief Returns the coroutine's execution state.
 * 
 * @details This function returns the coroutine's execution state. The execution state is a value that indicates
 * whether the coroutine is currently running, suspended, or has not been scheduled for execution yet.
 * 
 * @note If called from the main execution context, it will always return CCO_COROUTINE_STATE_NONE.
 * 
 * @param coroutine A pointer to the coroutine to get the execution state from.
 * @return cco_coroutine_execution_state The coroutine's execution state.
 */
CCO_API cco_coroutine_state cco_coroutine_get_state(const cco_coroutine* coroutine);

/**
 * @brief Returns the stack size of the given coroutine.
 * 
 * @details This function returns the stack size of the given coroutine.
 * 
 * @param coroutine A pointer to the coroutine.
 * @return size_t The stack size of the given coroutine.
 */
CCO_API size_t cco_coroutine_get_stack_size(const cco_coroutine* coroutine);

/**
 * @brief Returns the amount of stack space currently used by the given coroutine.
 * 
 * @details This function returns the amount of stack space used by the given coroutine. The returned value is
 * only meaningful if the coroutine is not running, otherwise it is not guaranteed to be accurate.
 * 
 * @param coroutine A pointer to the coroutine.
 * @return size_t The amount of stack space used by the given coroutine (0 if the coroutine is invalid)
 */
CCO_API size_t cco_coroutine_get_stack_usage(const cco_coroutine* coroutine);

/**
 * @brief Retrieved the value returned by the coroutine.
 * 
 * @details This function returns the value returned by the coroutine. It is set to NULL by default, so if the coroutine
 * has not returned a value, this function will return NULL.
 * 
 * @param coroutine A pointer to the coroutine to retrieve the return value from.
 * @return void* The value returned by the coroutine.
 */
CCO_API void* cco_coroutine_get_return_value(const cco_coroutine* coroutine);

#endif
