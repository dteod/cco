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

#ifndef CCO_SRC_ARCH_x86_H_INCLUDED
#define CCO_SRC_ARCH_x86_H_INCLUDED

#ifndef CCO_COROUTINE_IMPLEMENTATION
#  error "This file was designed to be included from coroutine.c directly"
#endif

/** Compiler-agnostic macro to define a cdecl calling convention. */
#if defined(__GNUC__) || defined(__clang__)
#  define fastcall __attribute__((fastcall))
#elif defined(_MSC_VER)
#  define fastcall __fastcall
#elif defined(__ICC) || defined(__INTEL_COMPILER)
#  define fastcall __fastcall
#else
#  error Unsupported compiler
#endif

/** Compiler-agnostic cpuid wrapper, defined as a macro */
#if defined(__GNUC__) || defined(__clang__)
#  include <cpuid.h>
#  define cco_cpuid(idx, eax, ebx, ecx, edx) __cpuid(idx, eax, ebx, ecx, edx);
#elif defined(_MSC_VER)
#  include <intrin.h>
#  define cco_cpuid(idx, eax, ebx, ecx, edx) __cpuid((int[4]){eax, ebx, ecx, edx}, idx);
#elif defined(__ICC) || defined(__INTEL_COMPILER)
#  include <immintrin.h>
#  define cco_cpuid(idx, eax, ebx, ecx, edx) _cpuid_count(idx, 0, eax, ebx, ecx, edx);
#else
#  error Compiler not supported
#endif

#pragma pack(push, 1)

/**
 * @brief Basic x86 registers struct.
 * 
 * @details This structure contains the CPU state for the x86 architecture.
 * It contains the general purpose registers, the status flag and instruction pointer,
 * the FPU registers, the floating-point control registers, and the x87/MMX/XMM registers if available.
 * 
 * @note XMM registers are included in the fxsave area, a runtime check for fxsave support is performed in cswitch.
 *
 */
struct cco_cpu_context {
#if CCO_CSWITCH_x86_EXCHANGE_GENERAL_PURPOSE_REGISTERS
    void* eax;
    void* ebx;
#endif
    /* ecx and edx are used to store the function parameters in fastcall ABI, so they are not exchanged
    void*   ecx;
    void*   edx;
    */
#if CCO_CSWITCH_x86_EXCHANGE_GENERAL_PURPOSE_REGISTERS
    void* esi;
    void* edi;
    void* ebp;
#endif
    void* esp;
    void* eip;
#if CCO_CSWITCH_x86_EXCHANGE_FLAGS_REGISTER
    void* eflags;
#endif
#if CCO_CSWITCH_x86_EXCHANGE_FPU_AND_MMX_REGISTERS
    uint8_t fxsr[512];
#endif
};

#pragma pack(pop)

/**
 * @brief The size of the CPU context structure, aligned to the next power of 2.
 * 
 * @details This variable is initialized in cco_init().
 */
CCO_PRIVATE uint32_t cco_x86_cpu_context_size;

#if CCO_CSWITCH_x86_EXCHANGE_FPU_AND_MMX_REGISTERS && CCO_CSWITCH_x86_USE_HAS_FXSR
/**
 * @brief Whether the CPU supports fxsave. Initialized in cco_init() and cached, used in cco_cswitch().
 * 
 * @details Defined in context.S.
 */
CCO_PRIVATE bool cco_x86_has_fxsr;

/**
 * @brief Whether the CPU supports fxsave. Initialized in cco_init() and cached.
 * 
 * @note This function creates 4 local variables, so it should be called only once at startup.
 * Specialized assembly is used in cco_cswitch() to avoid the overhead of this function.
 * 
 * @return always_inline 
 */
CCO_PRIVATE always_inline bool
cco_x86_supports_fxsr()
{
    uint32_t eax, ebx, ecx, edx;
    cco_cpuid(1, eax, ebx, ecx, edx);
    return (ecx & (1 << 24)) != 0;
}
#endif

/**
 * @brief Prepares the CPU context for the first call to cco_cswitch().
 * 
 * @details This function contains x86-specific code to prepare the CPU context for the first call to cco_cswitch().
 * In particular, it sets the instruction pointer to the entry point of the coroutine 
 * 
 * @param ctx The context to prepare
 */
CCO_PRIVATE always_inline void
cco_prepare_coroutine(cco_coroutine* coroutine)
{
    cco_cpu_context* ctx = coroutine->context;
    ctx->eip             = (void*)(uintptr_t)cco_coroutine_entry_point;
    /* Remind that
        1.  The stack grows downwards (push -> decrement, pop -> increment)
        2.  The argument in cco_coroutine_entry_point is passed on the stack.
        3.  We cannot store the variable in stack + size, or it will overflow.
    */
    ctx->esp          = coroutine->stack + coroutine->stack_size - sizeof(ctx->eip);
    *(void**)ctx->esp = coroutine;

    /* 
        We have to emulate a call to cco_coroutine_entry_point; to do that, the caller would
        push the return address on the stack, and then jump to the entry point.
        Remind that the stack is increased first and read after when popping
        and stored first and decreased after when pushing. So to account for storage of the return address,
        we have to decrement the stack pointer by 2 * sizeof(ctx->esp) (the size of the return address).
        The assembly will do the following to retrieve the argument from the stack:
            movl 0x8(%esp), %eax
    */
    ctx->esp = (char*)ctx->esp - 2 * sizeof(ctx->esp);
}

/**
 * @brief Stores the current CPU context in @p prev and loads the one stored in @p next.
 * 
 * @details This function will implement con a compiler-agnostic way the context switch function for the x86 architecture.
 * It will store the current CPU context in @p prev and load the one stored in @p next.
 * According to the configuration specified at compile time, it will exchange the general purpose registers,
 * the stack pointer and instruction pointer, the FPU registers, the floating-point control registers,
 * the x87/MMX/XMM registers (if available), and the flags register. In particular:
 * - the stack pointer and instruction pointer are always exchanged, because they correspond to the context by itself;
 * - the general purpose registers are exchanged if @ref CCO_CSWITCH_x86_EXCHANGE_GENERAL_PURPOSE_REGISTERS is set to 1;
 * - the FPU registers, the floating-point control registers, and the x87/MMX/XMM registers are exchanged only
 * - the flags register is exchanged if @ref CCO_CSWITCH_x86_EXCHANGE_FLAGS_REGISTER is set to 1;
 *   if @ref CCO_CSWITCH_x86_EXCHANGE_FPU_AND_MMX_REGISTERS is set to 1;
 *   - in particular, if CCO_CSWITCH_x86_USE_HAS_FXSR is set, the cpuid check for fxsave/fxrstor is performed once at startup,
 *     and later usages of such instructions are checked against the cached value; these instructions are not used if
 *     the CPU does not support them (pretty rare nowadays).
 * 
 * @param prev the current CPU context to store (and suspend)
 * @param next the CPU context to load (and resume)
 */
CCO_PRIVATE no_inline void fastcall
cco_cswitch(cco_cpu_context* restrict prev, cco_cpu_context* restrict next)
{
#if defined(__GNUC__) || defined(__clang__)
#  if CCO_CSWITCH_x86_EXCHANGE_GENERAL_PURPOSE_REGISTERS
    __asm__ volatile("pushl %eax");
    __asm__ volatile("movl %0, %%eax" ::"m"(prev));
    __asm__ volatile("movl %ebx, 0x4(%eax)");
    __asm__ volatile("movl %esi, 0x8(%eax)");
    __asm__ volatile("movl %edi, 0xc(%eax)");
    __asm__ volatile("movl %ebp, 0x10(%eax)");
    __asm__ volatile("movl %eax, %ebx");
    __asm__ volatile("popl %eax");
    __asm__ volatile("movl %eax, (%ebx)");
#  endif
#  if CCO_CSWITCH_x86_EXCHANGE_FLAGS_REGISTER
    __asm__ volatile("pushf");
    __asm__ volatile("popl %0" : "=r"(prev->eflags));
#  endif
#  if CCO_CSWITCH_x86_EXCHANGE_FPU_AND_MMX_REGISTERS
#    if CCO_CSWITCH_x86_USE_HAS_FXSR
    if(cco_x86_has_fxsr) {
        __asm__ volatile("fxsave %0" ::"m"(prev->fxsr) : "memory");
        __asm__ volatile("fxrstor %0" ::"m"(next->fxsr) : "memory");
    }
#    else
    __asm__ volatile(
        "cpuid\n\t"
        "test $(1<<24), %%ecx\n\t"
        "jz .Lno_fxsr\n\t"
        "fxsave %[prev]\n\t"
        "fxrstor %[next]\n"
        ".Lno_fxsr:\n\t"
        : /* no output */
        : [prev] "m"(prev->fxsr), [next] "m"(next->fxsr)
        : "memory"
    );
#    endif
#  endif
    void* ip = __builtin_return_address(0);
    __asm__ volatile("movl %%esp, %0" ::"m"(prev->esp));
    __asm__ volatile("movl %[ip], %%ebx\n\tmovl %%ebx, %[eip]" ::[ip] "m"(ip), [eip] "m"(prev->eip));
    __asm__ volatile("movl %0, %%esp" ::"m"(next->esp));
    __asm__ volatile("movl %0, %%ebx\n\tmovl %%ebx, (%%esp)" ::"m"(next->eip));
#  if CCO_CSWITCH_x86_EXCHANGE_GENERAL_PURPOSE_REGISTERS
    __asm__ volatile("movl %0, %%eax\n\t" ::"m"(next));
    __asm__ volatile("movl 0x4(%eax),%ebx");
    __asm__ volatile("movl 0x8(%eax),%esi");
    __asm__ volatile("movl 0xc(%eax),%edi");
    __asm__ volatile("movl 0x10(%eax),%ebp");
    __asm__ volatile("movl (%eax), %eax");
#  endif
    __asm__ volatile("ret");
#elif defined(_MSC_VER)
#  if CCO_CSWITCH_x86_EXCHANGE_GENERAL_PURPOSE_REGISTERS
    // code for eax, ebx, esi, edi, ebp
#  endif
#  if CCO_CSWITCH_x86_EXCHANGE_FLAGS_REGISTER
    // code for eflags
#  endif
#  if CCO_CSWITCH_x86_EXCHANGE_FPU_AND_MMX_REGISTERS
#    if CCO_CSWITCH_x86_USE_HAS_FXSR
    // code using cco_x86_has_fxsr
#    else
    // code using x86_supports_fxsr()
#    endif
#  endif
    // code for esp/eip
#elif defined(__ICC) || defined(__INTEL_COMPILER)
#  if CCO_CSWITCH_x86_EXCHANGE_GENERAL_PURPOSE_REGISTERS
    // code for eax, ebx, esi, edi, ebp
#  endif
#  if CCO_CSWITCH_x86_EXCHANGE_FLAGS_REGISTER
    // code for eflags
#  endif
#  if CCO_CSWITCH_x86_EXCHANGE_FPU_AND_MMX_REGISTERS
#    if CCO_CSWITCH_x86_USE_HAS_FXSR
    // code using cco_x86_has_fxsr
#    else
    // code using x86_supports_fxsr()
#    endif
#  endif
    // code for esp/eip
#else
#  error Unsupported compiler
#endif
}

/**
 * @brief This function calculates the next power of 2 of a given number.
 * 
 * @param s the number to calculate the next power of 2
 * @return size_t the next power of 2
 */
CCO_PRIVATE always_inline size_t
cco_next_power_of_2(size_t s)
{
    size_t p = 1;
    while(p < s) {
        p <<= 1;
    }
    return p;
}

/**
 * @brief This function calculates the size of the CPU context structure, aligned to the next power of 2.
 * 
 * @return size_t the size of the CPU context structure
 */
CCO_PRIVATE always_inline uint32_t
cco_calculate_x86_cpu_context_size()
{
#if CCO_x86_STORE_FPU_AND_MMX_REGISTERS
#  if CCO_x86_USE_HAS_FXSR
    return cco_next_power_of_2(sizeof(cco_cpu_context) - (cco_x86_has_fxsr ? 0 : sizeof(((cco_cpu_context*)(NULL))->fxsr)));
#  else
    return cco_next_power_of_2(sizeof(cco_cpu_context) - (x86_supports_fxsr() ? 0 : sizeof(((cco_cpu_context*)(NULL))->fxsr)));
#  endif
#else
    return cco_next_power_of_2(sizeof(cco_cpu_context));
#endif
}

CCO_PRIVATE void ctor
cco_cpu_context_init()
{
#if CCO_x86_STORE_FPU_AND_MMX_REGISTERS && CCO_x86_USE_HAS_FXSR
    cco_x86_has_fxsr = cco_x86_supports_fxsr();
#endif
    cco_x86_cpu_context_size = cco_calculate_x86_cpu_context_size();
}

#endif