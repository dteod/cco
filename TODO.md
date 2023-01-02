# TODO.md

This file contains general, architecture-specific, and OS-specific features to add to the library before it is considered stable and ready for production use.


General features
----------------
 - [x] Add a default returning function that can be used to return from a coroutine without having to specify a return value. This is useful for coroutines that do not return a value.
 - [ ] Some specialized programs may require a wider CPU context while performing their tasks and coroutines may switch only a part of the context they are working with. Extend the context switching capabilities and enable them both with compile-time settings or with runtime ones, for example for x86_64 provide a setting like CCO_X86_64_ENABLE_DEBUG_REGS to enable switching of DR0-DR7 or CCO_X86_64_ENABLE_CONTROL_REGS to enable switching of CR0-CR4 in addition to the general purpose, FPU and SSE/AVX/AVX-512.
    - [ ] Depending on the capabilities of the CPU, the library may or may not encompass switching of certain registers. An example is for AVX/AVX-512 which may not be available on all AMD64 architectures. In order for this to be performed correctly, a runtime check is to be added and the value can be set statically by a constructor function to be run before main.
    - [ ] Enforce alignment for instructions operating on memory that require to be properly aligned. This involves XSAVE/XRSTOR (64-bit alignment) and FXSAVE/FXRSTOR (16-bit alignment) instructions.
 - [ ] Remind to use the right calling convention for the architecture/OS in use. For example, 64-bit x86 has two calling conventions, which are the Microsoft x64 calling convention and the SystemV amd64 calling convention, which default to Windows and other OSes respectively. That should be overridden with a macro.
 - [ ] Since the memory allocation task in this library may deeply require aligned storage depending on various requirements, is is better to design its upper levels relying on a custom memory allocator. Support for static memory allocation is also required.
 - [ ] The library shall be tested on each platform it was meant to be deployed on with emulated hardware; qemu can be used to accomplish this for cross-architecture testing, while docker can be used for cross-OS testing. Testing shall encompass all compilers used to compile and assemble the library and at least one practical use case with a deterministic way to assert the library is working properly. A debugger can be used to assess the library's thread safety with timestamps.
