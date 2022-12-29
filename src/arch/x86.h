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
    /*  eax is caller-saved, while ecx and edx are used to store the function parameters in fastcall ABI,
        so we do not need to save or restore them.
    void*   eax;
    void*   ecx;
    void*   edx;
    */
    void* ebx;
    void* esi;
    void* edi;
    void* ebp;
    void* esp;
    void* eip;

    /*  All of the following registers are to be enabled via register-specific compile-time settings.
        This and the following comments expose the registers as documentation only; the size of the struct to be allocated
        will be calculated at runtime according to the x86-specific runtime settings of the chosen coroutine, and
        only using the registers enabled by the compile-time settings. The cco_cswitch function (responsible of
        storing and restoring the context) will also use the runtime settings, to determine which registers to save and restore.
    */

    /* Eflags register.
    uint32_t eflags;
    */

    // union fpu_mmx_sse {
    // struct no_sse {
    /* FPU/MMX registers. MMX is fully contained in MMX.
            uint16_t fpu_status;                                // 16-bit
            uint16_t fpu_control;                               // 16-bit
            uint16_t fpu_tag_word;                              // 16-bit
            struct { uint16_t high, uint32_t low; } fpu_ip;     // 48-bit
            struct { uint16_t high, uint32_t low; } fpu_dp;     // 48-bit                  
            uint16_t fpu_opcode;                                // 11-bit
            struct { uint16_t low; uint32_t high; } st[8];      // 8 registers of 80-bit each
            */

    /* SSE registers.
            struct { uint64_t high; uint64_t low; } xmm[8];     // 8 registers of 128-bit each
            uint32_t mxcsr;                                     // 32-bit
            */
    //  } no_sse; // end struct no_sse
    /* If all of FPU, MMX and SSE registers are enabled, the fxsave area is used to store the context.
            uint8_t fxsave[512];
            */
    // }; // end union fpu_mmx_sse

    /* Segment registers.
    uint16_t cs;
    uint16_t ds;
    uint16_t es;
    uint16_t fs;
    uint16_t gs;
    uint16_t ss;
    */

    /* Debug registers.
    uint32_t dr0;
    uint32_t dr1;
    uint32_t dr2;
    uint32_t dr3;
    uint32_t dr6;
    uint32_t dr7;
    */

    /* Control registers.
    uint32_t cr0;
    uint32_t cr2;
    uint32_t cr3;
    uint32_t cr4;
    uint32_t cr8;
    */
};

#pragma pack(pop)

#define CCO_DEFAULT_ARCHITECTURE_SPECIFIC_SETTINGS() &cco_default_x86_settings_instance;

CCO_PRIVATE cco_x86_settings cco_default_x86_settings_instance = 0
#if CCO_x86_ENABLE_EFLAGS_REGISTER_EXCHANGE && CCO_x86_EFLAGS_REGISTER_DEFAULT_EXCHANGE
                                                                 | CCO_SETTINGS_x86_EXCHANGE_EFLAGS_REGISTER
#endif
#if CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE && CCO_x86_FPU_MMX_REGISTERS_DEFAULT_EXCHANGE
                                                                 | CCO_SETTINGS_x86_EXCHANGE_FPU_MMX_REGISTERS
#endif
#if CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE && CCO_x86_SSE_REGISTERS_DEFAULT_EXCHANGE
                                                                 | CCO_SETTINGS_x86_EXCHANGE_SSE_REGISTERS
#endif
#if CCO_x86_ENABLE_SEGMENT_REGISTERS_EXCHANGE && CCO_x86_SEGMENT_REGISTERS_DEFAULT_EXCHANGE
                                                                 | CCO_SETTINGS_x86_EXCHANGE_SEGMENT_REGISTERS
#endif
#if CCO_x86_ENABLE_DEBUG_REGISTERS_EXCHANGE && CCO_x86_DEBUG_REGISTERS_DEFAULT_EXCHANGE
                                                                 | CCO_SETTINGS_x86_EXCHANGE_DEBUG_REGISTERS
#endif
#if CCO_x86_ENABLE_CONTROL_REGISTERS_EXCHANGE && CCO_x86_CONTROL_REGISTERS_DEFAULT_EXCHANGE
                                                                 | CCO_SETTINGS_x86_EXCHANGE_CONTROL_REGISTERS
#endif
    ;

CCO_PRIVATE always_inline size_t
cco_get_cpu_context_size(const cco_architecture_specific_settings* settings)
{
    // clang-format off
    return sizeof(struct cco_cpu_context)
#if CCO_x86_ENABLE_EFLAGS_REGISTER_EXCHANGE
    + ((*settings & CCO_SETTINGS_x86_EXCHANGE_EFLAGS_REGISTER) ? sizeof(uint32_t) : 0)
#endif
#if CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE && CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE
    + ( // first branch is for FPU/MMX and SSE combined (fxsr), second for FPU/MMX only, and third for SSE only
    ((*settings & (CCO_SETTINGS_x86_EXCHANGE_FPU_MMX_REGISTERS | CCO_SETTINGS_x86_EXCHANGE_SSE_REGISTERS)) ? sizeof(uint8_t[512]) :
    ((*settings & CCO_SETTINGS_x86_EXCHANGE_FPU_MMX_REGISTERS) ? sizeof(uint8_t[(16+16+16+48+48+16+8*80)/8]) :
    ((*settings & CCO_SETTINGS_x86_EXCHANGE_SSE_REGISTERS) ? sizeof(uint8_t[(32+8*128)/8]) : 0
    ))))
#elif CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE
    + ((*settings & CCO_SETTINGS_x86_EXCHANGE_FPU_MMX_REGISTERS) ? sizeof(uint8_t[(16+16+16+48+48+16+8*80)/8]) : 0)
#elif CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE
    + ((*settings & CCO_SETTINGS_x86_EXCHANGE_SSE_REGISTERS) ? sizeof(uint8_t[(32+8*128)/8]) : 0)
#endif
#if CCO_x86_ENABLE_SEGMENT_REGISTERS_EXCHANGE
    + (*setting & CCO_SETTINGS_x86_EXCHANGE_SEGMENT_REGISTERS) ? sizeof(uint16_t[6]) : 0
#endif
#if CCO_x86_ENABLE_DEBUG_REGISTERS_EXCHANGE
    + (*setting & CCO_SETTINGS_x86_EXCHANGE_DEBUG_REGISTERS) ? sizeof(uint32_t[6]) : 0
#endif
#if CCO_x86_ENABLE_CONTROL_REGISTERS_EXCHANGE
    + (*setting & CCO_SETTINGS_x86_EXCHANGE_CONTROL_REGISTERS) ? sizeof(uint32_t[5]) : 0
#endif
    ;
    // clang-format on
}

#if CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE && CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE

/** @brief Whether the CPU supports fxsave. Initialized in cco_init() and cached. */
CCO_PRIVATE volatile bool cco_x86_has_fxsr;

/**
 * @brief Whether the CPU supports fxsave. Initialized in cco_init() and cached.
 * 
 * @note This function creates 4 local variables, so it should be called only once at startup.
 * Specialized assembly is used in cco_cswitch() to avoid the overhead of this function.
 * 
 * @return bool whether the CPU supports fxsave or not 
 */
CCO_PRIVATE always_inline bool
cco_x86_retrieve_has_fxsr()
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
    ctx->ebp = (char*)ctx->esp + sizeof(ctx->esp);
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
 * - the general purpose registers are exchanged if @ref CCO_x86_EXCHANGE_GENERAL_PURPOSE_REGISTERS is set to 1;
 * - the FPU registers, the floating-point control registers, and the x87/MMX/XMM registers are exchanged only
 * - the flags register is exchanged if @ref CCO_x86_EXCHANGE_FLAGS_REGISTER is set to 1;
 *   if @ref CCO_x86_EXCHANGE_FPU_AND_MMX_REGISTERS is set to 1;
 *   - in particular, if CCO_x86_USE_HAS_FXSR is set, the cpuid check for fxsave/fxrstor is performed once at startup,
 *     and later usages of such instructions are checked against the cached value; these instructions are not used if
 *     the CPU does not support them (pretty rare nowadays).
 * 
 * @param prev the current CPU context to store (and suspend)
 * @param next the CPU context to load (and resume)
 */
CCO_PRIVATE no_inline fastcall naked void
cco_cswitch(cco_coroutine* restrict prev, cco_coroutine* restrict next)
{
    // We mark the arguments as unused to avoid warnings, but we actually use them in the assembly code.
    // ecx contains the address of prev, edx contains the address of next.
    (void)prev;
    (void)next;

    // In the first implementation of this function I was using prev/next to actually access their fields, but
    // 1. I obtained suboptimal code (twice the number of instructions for each register stored)
    // 2. The code could not work when using -O3 (most likely because the function was not naked, though).
    // Eventually I wrote the function in C for if control statements, mixed with pure assembly code by hand.
#if defined(__GNUC__) || defined(__clang__)

#  if CCO_x86_ENABLE_EFLAGS_REGISTER_EXCHANGE || CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE                                       \
      || CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE || CCO_x86_ENABLE_SEGMENT_REGISTERS_EXCHANGE                                      \
      || CCO_x86_ENABLE_DEBUG_REGISTERS_EXCHANGE || CCO_x86_ENABLE_CONTROL_REGISTERS_EXCHANGE
    __asm__ volatile("pushl 4(%ecx)");
#  endif
    __asm__ volatile("movl (%ecx), %ecx");     /* %ecx = prev->context;*/
    __asm__ volatile("movl %ebx, 0x00(%ecx)"); /* prev->context.ebx = %ebx; */
#  if CCO_x86_ENABLE_EFLAGS_REGISTER_EXCHANGE || CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE                                       \
      || CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE || CCO_x86_ENABLE_SEGMENT_REGISTERS_EXCHANGE                                      \
      || CCO_x86_ENABLE_DEBUG_REGISTERS_EXCHANGE || CCO_x86_ENABLE_CONTROL_REGISTERS_EXCHANGE
    __asm__ volatile("popl %ebx");
#  endif
    __asm__ volatile("movl %esi, 0x04(%ecx)");                /* prev->context.esi = %esi; */
    __asm__ volatile("movl %edi, 0x08(%ecx)");                /* prev->context.edi = %edi; */
    __asm__ volatile("movl %ebp, 0x0c(%ecx)");                /* prev->context.ebp = %ebp; */
    __asm__ volatile("movl %esp, 0x10(%ecx)");                /* prev->context.esp = %esp; */
    __asm__ volatile("popl %edi \n\t movl %edi, 0x14(%ecx)"); /* prev->context.eip = %eip; */
    __asm__ volatile("addl $28, %ecx");                       /* %ecx = (&prev->context.eip + 1); */

#  define ASMSTR(X)  ASMSTR2(X)
#  define ASMSTR2(X) #X

#  if CCO_x86_ENABLE_EFLAGS_REGISTER_EXCHANGE
#    define EXCHANGE_EFLAGS_REGISTER (1 << 0)
    _Static_assert(
        CCO_SETTINGS_x86_EXCHANGE_EFLAGS_REGISTER == EXCHANGE_EFLAGS_REGISTER,
        "The value of CCO_SETTINGS_x86_EXCHANGE_EFLAGS_REGISTER differs from the value of EXCHANGE_EFLAGS_REGISTER"
    );
    __asm__ volatile("test $" ASMSTR(EXCHANGE_EFLAGS_REGISTER) ", %ebx"); /* if (settings & EXCHANGE_EFLAGS_REGISTER) */
    __asm__ volatile("jz .Lno_exchange_flags_registers");                 /* then */
    __asm__ volatile("pushf");                                            /* push %eflags; */
    __asm__ volatile("popl %esi");                                        /* %ebx = %eflags; */
    __asm__ volatile("movl %esi, 0x00(%ecx)");                            /* prev->context.eflags = %eflags; */
    __asm__ volatile("addl $4, %ecx");                                    /* %ecx = (&prev->context.eflags + 1); */
    __asm__ volatile(".Lno_exchange_flags_registers:");                   /* else */
#  endif

#  if CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE && CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE
#    define EXCHANGE_FPU_MMX_REGISTERS (1 << 1)
    _Static_assert(
        CCO_SETTINGS_x86_EXCHANGE_FPU_MMX_REGISTERS == EXCHANGE_FPU_MMX_REGISTERS,
        "The value of CCO_SETTINGS_x86_EXCHANGE_FPU_MMX_REGISTERS differs from the value of EXCHANGE_FPU_MMX_REGISTERS"
    );
#    define EXCHANGE_SSE_REGISTERS (1 << 2)
    _Static_assert(
        CCO_SETTINGS_x86_EXCHANGE_SSE_REGISTERS == EXCHANGE_SSE_REGISTERS,
        "The value of CCO_SETTINGS_x86_EXCHANGE_SSE_REGISTERS differs from the value of EXCHANGE_SSE_REGISTERS"
    );
    __asm__ volatile("test $(" ASMSTR(EXCHANGE_FPU_MMX_REGISTERS) "|" ASMSTR(EXCHANGE_SSE_REGISTERS) "), %ebx");
    __asm__ volatile("jz .Lno_exchange_fpu_mmx_sse_registers");
    if(cco_x86_has_fxsr) {
        __asm__ volatile("fxsave (%ecx)");
        __asm__ volatile("addl $512, %ecx");
    }
    else {
        // TODO: store FPU/MMX and SSE registers one by one
    }
    __asm__ volatile(".Lno_exchange_fpu_mmx_sse_registers:");
#  elif CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE
#    define EXCHANGE_FPU_MMX_REGISTERS (1 << 1)
    _Static_assert(
        CCO_SETTINGS_x86_EXCHANGE_FPU_MMX_REGISTERS == EXCHANGE_FPU_MMX_REGISTERS,
        "The value of CCO_SETTINGS_x86_EXCHANGE_FPU_MMX_REGISTERS differs from the value of EXCHANGE_FPU_MMX_REGISTERS"
    );
    __asm__ volatile("test $(" ASMSTR(EXCHANGE_FPU_MMX_REGISTERS) "), %ebx");
    __asm__ volatile("jz .Lno_exchange_fpu_mmx_registers");
    // TODO: store FPU/MMX registers one by one
    __asm__ volatile(".Lno_exchange_fpu_mmx_registers:");
#  elif CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE
#    define EXCHANGE_SSE_REGISTERS (1 << 2)
    _Static_assert(
        CCO_SETTINGS_x86_EXCHANGE_SSE_REGISTERS == EXCHANGE_SSE_REGISTERS,
        "The value of CCO_SETTINGS_x86_EXCHANGE_SSE_REGISTERS differs from the value of EXCHANGE_SSE_REGISTERS"
    );
    __asm__ volatile("test %(" ASMSTR(EXCHANGE_SSE_REGISTERS) "), %ebx");
    __asm__ volatile("jz .Lno_exchange_sse_registers");
    // TODO: store SSE registers one by one
    // __asm__ volatile("addl $128, %ecx");
    __asm__ volatile(".Lno_exchange_sse_registers:");
#  endif

#  if CCO_x86_ENABLE_SEGMENT_REGISTERS_EXCHANGE
#    define EXCHANGE_SEGMENT_REGISTERS (1 << 3)
    _Static_assert(
        CCO_SETTINGS_x86_EXCHANGE_SEGMENT_REGISTERS == EXCHANGE_SEGMENT_REGISTERS,
        "The value of CCO_SETTINGS_x86_EXCHANGE_SEGMENT_REGISTERS differs from the value of EXCHANGE_SEGMENT_REGISTERS"
    );
    /* if(settings & CCO_SETTINGS_x86_EXCHANGE_SEGMENT_REGISTERS) { */
    __asm__ volatile("test $" ASMSTR(EXCHANGE_SEGMENT_REGISTERS) ", %ebx");
    __asm__ volatile("jz .Lno_exchange_segment_registers");
    __asm__ volatile("movw %cs, 0x00(%ecx)"); /* prev->context.cs = %cs; */
    __asm__ volatile("movw %ds, 0x02(%ecx)"); /* prev->context.ds = %ds; */
    __asm__ volatile("movw %ss, 0x04(%ecx)"); /* prev->context.ss = %ss; */
    __asm__ volatile("movw %es, 0x06(%ecx)"); /* prev->context.es = %es; */
    __asm__ volatile("movw %fs, 0x08(%ecx)"); /* prev->context.fs = %fs; */
    __asm__ volatile("movw %gs, 0x0a(%ecx)"); /* prev->context.gs = %gs; */
    __asm__ volatile("addl $12, %ecx");       /* %ecx = (&prev->context.gs + 1); */
    __asm__ volatile(".Lno_exchange_segment_registers:");
#  endif

#  if CCO_x86_ENABLE_DEBUG_REGISTERS_EXCHANGE
#    define EXCHANGE_DEBUG_REGISTERS (1 << 4)
    _Static_assert(
        CCO_SETTINGS_x86_EXCHANGE_DEBUG_REGISTERS == EXCHANGE_DEBUG_REGISTERS,
        "The value of CCO_SETTINGS_x86_EXCHANGE_DEBUG_REGISTERS differs from the value of EXCHANGE_DEBUG_REGISTERS"
    );
    __asm__ volatile("test $" ASMSTR(EXCHANGE_DEBUG_REGISTERS) ", %ebx");
    __asm__ volatile("jz .Lno_exchange_debug_registers");
    __asm__ volatile("movl %dr0, 0x00(%ecx)"); /* prev->context.dr0 = %dr0; */
    __asm__ volatile("movl %dr1, 0x04(%ecx)"); /* prev->context.dr1 = %dr1; */
    __asm__ volatile("movl %dr2, 0x08(%ecx)"); /* prev->context.dr2 = %dr2; */
    __asm__ volatile("movl %dr3, 0x0c(%ecx)"); /* prev->context.dr3 = %dr3; */
    __asm__ volatile("movl %dr6, 0x10(%ecx)"); /* prev->context.dr6 = %dr6; */
    __asm__ volatile("movl %dr7, 0x14(%ecx)"); /* prev->context.dr7 = %dr7; */
    __asm__ volatile("addl $24, %ecx");        /* %ecx = (&prev->context.dr7 + 1); */
    __asm__ volatile(".Lno_exchange_debug_registers:");
#  endif

#  if CCO_x86_ENABLE_CONTROL_REGISTERS_EXCHANGE
#    define EXCHANGE_CONTROL_REGISTERS (1 << 5)
    _Static_assert(
        CCO_SETTINGS_x86_EXCHANGE_CONTROL_REGISTERS == EXCHANGE_CONTROL_REGISTERS,
        "The value of CCO_SETTINGS_x86_EXCHANGE_CONTROL_REGISTERS differs from the value of EXCHANGE_CONTROL_REGISTERS"
    );
    __asm__ volatile("test $" ASMSTR(EXCHANGE_CONTROL_REGISTERS) ", %ebx");
    __asm__ volatile("jz .Lno_exchange_control_registers");
    __asm__ volatile("movl %cr0, 0x00(%ecx)"); /* prev->context.cr0 = %cr0; */
    __asm__ volatile("movl %cr2, 0x04(%ecx)"); /* prev->context.cr2 = %cr2; */
    __asm__ volatile("movl %cr3, 0x08(%ecx)"); /* prev->context.cr3 = %cr3; */
    __asm__ volatile("movl %cr4, 0x0c(%ecx)"); /* prev->context.cr4 = %cr4; */
    __asm__ volatile("movl %cr8, 0x10(%ecx)"); /* prev->context.cr8 = %cr8; */
    __asm__ volatile(".Lno_exchange_control_registers:");
#  endif
#  if CCO_x86_ENABLE_EFLAGS_REGISTER_EXCHANGE || CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE                                       \
      || CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE || CCO_x86_ENABLE_SEGMENT_REGISTERS_EXCHANGE                                      \
      || CCO_x86_ENABLE_DEBUG_REGISTERS_EXCHANGE || CCO_x86_ENABLE_CONTROL_REGISTERS_EXCHANGE
    __asm__ volatile("movl 4(%edx), %ebx");
#  endif
    __asm__ volatile("movl (%edx), %edx");
    __asm__ volatile("movl 0x10(%edx), %esp");
    __asm__ volatile("movl 0x14(%edx), %edi \n\t movl %edi, 0x0(%esp)");
    __asm__ volatile("movl 0x0c(%edx), %ebp");
    __asm__ volatile("movl 0x08(%edx), %edi");
    __asm__ volatile("movl 0x04(%edx), %esi");
    __asm__ volatile("movl 0x00(%edx), %ebx");
    __asm__ volatile("ret");
#elif defined(_MSC_VER)
#elif defined(__ICC) || defined(__INTEL_COMPILER)
#else
#  error Unsupported compiler
#endif
}

#if CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE && CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE
CCO_PRIVATE void ctor
cco_cpu_context_init()
{
    cco_x86_has_fxsr = cco_x86_retrieve_has_fxsr();
}
#endif

#endif