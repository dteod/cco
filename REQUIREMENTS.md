# Requirements

Requirements are divided into three categories: general, architecture-specific and OS-specific. The general requirements are meant to be met by all platforms, while the architecture-specific and OS-specific ones are meant to be met by the respective platforms. The requirements are numbered in a way that makes it easy to refer to them in the code.

## General requirements

|ID|Requirement|Description|
|---|:---|:---|
|CCO-1|Language implementation| The library shall be implemented in C99; low-level switching routines shall be implemented in architecture-specific assembly language. |
|CCO-2|Build system| The library shall be built using CMake and exported in another CMake project as-is in a way that does not pollute the importing project's namespace. |
|CCO-3|Opaque data type| The library shall provide an opaque data type for the coroutine. |
|CCO-4|Publicly accessible functions (API)| The library shall provide an API to access the coroutine opaque data type according to the rest of this specification. |
|CCO-5|Architecture-specific settings| Upon creation, there shall be a way to configure architecture-specific settings that regulate how the context switch will be performed, for example to switch more registers than the bare minimum used by a specific application and commonly ignored. The architecture-specific settings shall be described in a table for each architecture and the list of settings shall be documented. |
|CCO-6|Compile-time settings| The library shall provide compile-time settings to enable/disable architecture-specific settings or other aspects of the library. |
|CCO-7|Coroutine creation API| The library shall provide a function to create a coroutine. |
|CCO-8|Coroutine destruction API| The library shall provide a function to destroy a coroutine. |
|CCO-9|Coroutine start API| The library shall provide a function to start a coroutine on a user-defined payload function. It shall be usable from both the main execution context and a coroutine. |
|CCO-10|Coroutine yield API| The library shall provide a function to yield control back from a coroutine with a returning value. |
|CCO-11|Coroutine suspend API| The library shall provide a function to suspend a coroutine, returning to the calling context. |
|CCO-12|Coroutine resume API| The library shall provide a function to resume a suspended coroutine. |
|CCO-13|Coroutine return API| The library shall provide a function to return from a suspended coroutine and unschedule it. It shall be called from a coroutine automatically when its payload function will return. |
|CCO-14|Current coroutine API| The library shall provide a function to retrieve the current coroutine. It shall return NULL when in the main execution context. |
|CCO-15|Coroutine await API| The library shall provide a function to await for a specific action to finish. The awaiting mechanism shall be implemented as follows: <ol><li>the await function shall use two callbacks to determine how the awaiting mechanism shall be controlled:<ul><li>a `ready` callback to determine wether the coroutine is ready to resume execution</li><li>an `on_suspend` callback to exploit an extern, user-defined coroutine scheduling mechanism to resume the coroutine</li></ul>Both of the functions shall have the type defined as `bool(*)(coroutine*, void*)`; the framework shall pass the coroutine and the user-defined argument to the callbacks.</li><li>the await function shall open a loop in which the `ready` callback is invoked to check readiness to resume execution; depending on the return value:<ul><li>if `true` is returned, the coroutine will be resumed (this ensures synchronization of synchronous operations)</li><li>if `false` is returned, the `on_suspend` callback is invoked; here, depending on the return value returned:<ul><li>if `true` is returned, the loop is broken and the coroutine is suspended;</li><li>if `false` is returned, the loop restarts.</li></ul></li></ul></li><li>Whenever the coroutine is resumed, it shall return to the actual execution context (the first statement after the await function).</li></ol> |
|CCO-16|Coroutine status| The library shall provide a tag to describe the status of a coroutine. The tags shall distinguish between an unscheduled coroutine (created but never started), a suspended coroutine (started but not running at the time of the call), and a running coroutine (started and with an active process working on it). |
|CCO-17|Coroutine status API| The library shall provide a function to retrieve the status of a coroutine. The call shall be thread-safe. |
|CCO-18|Coroutine stack size| The library shall provide a function to set the stack size of a coroutine. |
|CCO-19|Coroutine stack size API| The library shall provide a function to retrieve the stack size of a coroutine. |
|CCO-20|Coroutine stack usage API| The library shall provide a function to retrieve the stack currently used from the coroutine; it shall return correct results when the coroutine is currently suspended, otherwise it is not required to return successfully. |
|CCO-21|Library error handling| The library shall provide a way to retrieve the last error that occurred similar to the C standard `errno`.|
|CCO-22|Support for multiple architectures|The library shall support the following architectures (if the corresponding OS and compiler supports it):<ul><li>x86</li><li>x86_64</li> <li>ARM</li> <li>AArch64</li> <li>MIPS</li> <li>MIPS64</li> <li>PowerPC</li> <li>PowerPC64</li> <li>RISC-V</li> <li>SPARC</li><li>SPARC64</li><li>Xtensa</li></ul>|
|CCO-23|Support for multiple operating systems|The library shall support the following operating systems (if it supports the corresponding architecture):<ul><li>Linux</li><li>macOS</li><li>Windows</li><!-- <li>FreeBSD</li><li>NetBSD</li><li>OpenBSD</li><li>DragonFlyBSD</li><li>Solaris</li><li>Haiku</li><li>AROS</li><li>AmigaOS</li><li>MorphOS</li><li>ReactOS</li><li>Minix</li><li>Plan 9</li><li>Android</li><li>iOS</li><li>tvOS</li><li>watchOS</li><li>Fuchsia</li></ul> -->|
|CCO-24|Support for multiple compilers|The library shall support the following compilers (depending on the target executable format and OS/architecture support):<ul><li>GCC</li><li>Clang</li><li>MSVC</li><li>Intel C/C++ Compiler (ICC)</li></ul>|
|CCO-25|Test suite|The library shall provide a test suite to ensure the correct functioning of the library. The test suite shall contain a test case/script for each requirement marked as verifiable by test (TBD), unless otherwise specified.|

## Architecture-specific requirements

### x86-specific requirements
|ID|Requirement|Description|
|---|:---|:---|
|CCO-x86-1|Calling convention|The context switching function shall be implemented using a calling convention that is coherent with the functions used to invoke it.|
|CCO-x86-2|Base context switch registers|An x86-based implementation of cco shall switch switch the following general purpose registers:<ul><li>ebx</li><li>edi</li><li>esi</li><li>esp</li><li>ebp</li><li>eip</li></ul>_Note_: the meaning of "switch" refers to the possibility of storing the state of the CPU, to be restored _as-is_, as if no call was made to the switching function. Registers not listed in this nor in the following requirements are not considered part of the CPU state to save and restore.|
|CCO-x86-3|EFLAGS optional switch|The library shall provide a compile-setting to enable or disable the saving and restoring of the EFLAGS register, to be enabled with the x86-specific runtime settings.|
|CCO-x86-4|FPU optional switch|The library shall provide a compile-setting to enable or disable the saving and restoring of the FPU control registers and the x87 FPU state, to be enabled with the x86-specific runtime settings. _Note_: the MMX state, when available on the target processor, shares CPU resources with the x87 FPU, this feature may therefore be used to store the MMX state.|
|CCO-x86-5|SSE optional switch|The library shall provide a compile-setting to enable or disable the saving and restoring of the SSE registers, to be enabled with the x86-specific runtime settings.|
|CCO-x86-6|Segment registers optional switch|The library shall provide a compile-setting to enable or disable the saving and restoring of the segment registers, to be enabled with the x86-specific runtime settings.|
|CCO-x86-7|Debug registers optional switch|The library shall provide a compile-setting to enable or disable the saving and restoring of the debug registers, to be enabled with the x86-specific runtime settings.|
|CCO-x86-8|Control registers optional switch|The library shall provide a compile-setting to enable or disable the saving and restoring of the control registers, to be enabled with the x86-specific runtime settings.|



### x86_64-specific requirements
### ARM-specific requirements
### AArch64-specific requirements
### SPARC-specific requirements
### SPARC64-specific requirements
### Xtensa-specific requirements

## OS-specific requirements

### Linux-specific requirements
### Windows-specific requirements
### macOS-specific requirements
### Bare metal-specific requirements
