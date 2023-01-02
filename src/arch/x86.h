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

/** Compiler-agnostic macro to define a fastcall calling convention. */
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
 * @details This structure started as a struct to be used to store the context of a coroutine, but it was later
 * chosen to enlarge it at runtime according to the compile-time and runtime settings of the coroutine. It is used as a
 * buffer to store the context of a coroutine.
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
    /* FPU/MMX registers. MMX is fully contained in FPU.
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
CCO_PRIVATE thread_local _Alignas(16) uint8_t cco_main_context
    [sizeof(cco_cpu_context)
#if CCO_x86_ENABLE_EFLAGS_REGISTER_EXCHANGE
     + sizeof(uint32_t)
#endif
#if CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE && CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE
#  if !CCO_x86_ENABLE_EFLAGS_REGISTER_EXCHANGE
     + sizeof(uint32_t)
#  endif
     + sizeof(uint8_t[512])
#elif CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE
     + sizeof(uint8_t[(16 + 16 + 16 + 48 + 48 + 16 + 8 * 80) / 8])
#elif CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE
     + sizeof(uint8_t[(32 + 8 * 128) / 8])
#endif
#if CCO_x86_ENABLE_SEGMENT_REGISTERS_EXCHANGE
     + sizeof(uint16_t[6])
#endif
#if CCO_x86_ENABLE_DEBUG_REGISTERS_EXCHANGE
     + sizeof(uint32_t[6])
#endif
#if CCO_x86_ENABLE_CONTROL_REGISTERS_EXCHANGE
     + sizeof(uint32_t[5])
#endif
] = {0};

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

#define CCO_x86_BARE_CSWITCH                                                                                                     \
  (!(CCO_x86_ENABLE_EFLAGS_REGISTER_EXCHANGE || CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE                                        \
     || CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE || CCO_x86_ENABLE_SEGMENT_REGISTERS_EXCHANGE                                       \
     || CCO_x86_ENABLE_DEBUG_REGISTERS_EXCHANGE || CCO_x86_ENABLE_CONTROL_REGISTERS_EXCHANGE))

#define CCO_x86_ELIGIBLE_FOR_FXSR (CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE && CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE)

CCO_PRIVATE always_inline size_t
cco_get_cpu_context_size(const cco_architecture_specific_settings* settings)
{
    // clang-format off
#if CCO_x86_BARE_CSWITCH
    // It was not needed to wrap this in a conditional block, but is not a problem either.
    (void) settings;
#endif
    return sizeof(struct cco_cpu_context)
#if CCO_x86_ENABLE_EFLAGS_REGISTER_EXCHANGE
#   if CCO_x86_ELIGIBLE_FOR_FXSR
    // this branch accounts for the fact that FXSR requires 16-byte alignment; with eflags we have 32 bytes and it's good
    // If we don't have EFLAGS but we have FXSR, we pad 4 bytes anyway
    + ((*settings & CCO_SETTINGS_x86_EXCHANGE_EFLAGS_REGISTER || 
        *settings & (CCO_SETTINGS_x86_EXCHANGE_FPU_MMX_REGISTERS | CCO_SETTINGS_x86_EXCHANGE_SSE_REGISTERS)) ? sizeof(uint32_t) : 0)
#   else
    + ((*settings & CCO_SETTINGS_x86_EXCHANGE_EFLAGS_REGISTER) ? sizeof(uint32_t) : 0)
#   endif
#endif
#if CCO_x86_ELIGIBLE_FOR_FXSR
#   if !CCO_x86_ENABLE_EFLAGS_REGISTER_EXCHANGE
    // this is to pad 4 bytes and reach 32 bytes alignment for FXSR if we did not enter in the previous #if branch
    + ( // first branch is for FPU/MMX and SSE combined (fxsr), second for FPU/MMX only, and third for SSE only
    ((*settings & (CCO_SETTINGS_x86_EXCHANGE_FPU_MMX_REGISTERS | CCO_SETTINGS_x86_EXCHANGE_SSE_REGISTERS)) ? sizeof(uint32_t) + sizeof(uint8_t[512]) :
    ((*settings & CCO_SETTINGS_x86_EXCHANGE_FPU_MMX_REGISTERS) ? sizeof(uint8_t[(16+16+16+48+48+16+8*80)/8]) :
    ((*settings & CCO_SETTINGS_x86_EXCHANGE_SSE_REGISTERS) ? sizeof(uint8_t[(32+8*128)/8]) : 0
    ))))
#   else
    + ( // first branch is for FPU/MMX and SSE combined (fxsr), second for FPU/MMX only, and third for SSE only
    ((*settings & (CCO_SETTINGS_x86_EXCHANGE_FPU_MMX_REGISTERS | CCO_SETTINGS_x86_EXCHANGE_SSE_REGISTERS)) ? sizeof(uint8_t[512]) :
    ((*settings & CCO_SETTINGS_x86_EXCHANGE_FPU_MMX_REGISTERS) ? sizeof(uint8_t[(16+16+16+48+48+16+8*80)/8]) :
    ((*settings & CCO_SETTINGS_x86_EXCHANGE_SSE_REGISTERS) ? sizeof(uint8_t[(32+8*128)/8]) : 0
    ))))
#   endif
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

#if CCO_x86_ELIGIBLE_FOR_FXSR

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
    return (edx & (1 << 24)) != 0;
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

#  if !CCO_x86_BARE_CSWITCH
    asm volatile("pushl 4(%ecx)");
#  endif
    asm volatile("movl (%ecx), %ecx");     /* %ecx = prev->context;*/
    asm volatile("movl %ebx, 0x00(%ecx)"); /* prev->context.ebx = %ebx; */
#  if !CCO_x86_BARE_CSWITCH
    asm volatile("popl %ebx");
#  endif
    asm volatile("movl %esi, 0x04(%ecx)");                /* prev->context.esi = %esi; */
    asm volatile("movl %edi, 0x08(%ecx)");                /* prev->context.edi = %edi; */
    asm volatile("movl %ebp, 0x0c(%ecx)");                /* prev->context.ebp = %ebp; */
    asm volatile("movl %esp, 0x10(%ecx)");                /* prev->context.esp = %esp; */
    asm volatile("popl %edi \n\t movl %edi, 0x14(%ecx)"); /* prev->context.eip = %eip; */
#  if !CCO_x86_BARE_CSWITCH
    asm volatile("addl $28, %ecx"); /* %ecx = (&prev->context.eip + 1); */
#    define ASMSTR(X)  ASMSTR2(X)
#    define ASMSTR2(X) #X
#  endif

#  if CCO_x86_ENABLE_EFLAGS_REGISTER_EXCHANGE
#    define EXCHANGE_EFLAGS_REGISTER (1 << 0)
    _Static_assert(
        CCO_SETTINGS_x86_EXCHANGE_EFLAGS_REGISTER == EXCHANGE_EFLAGS_REGISTER,
        "The value of CCO_SETTINGS_x86_EXCHANGE_EFLAGS_REGISTER differs from the value of EXCHANGE_EFLAGS_REGISTER"
    );
    asm volatile("test $" ASMSTR(EXCHANGE_EFLAGS_REGISTER) ", %ebx"); /* if (settings & EXCHANGE_EFLAGS_REGISTER) */
    asm volatile("jz .Lno_store_flags_registers");                    /* then */
    asm volatile("pushf");                                            /* push %eflags; */
    asm volatile("popl %esi");                                        /* %ebx = %eflags; */
    asm volatile("movl %esi, 0x00(%ecx)");                            /* prev->context.eflags = %eflags; */
    asm volatile("addl $4, %ecx");                                    /* %ecx = (&prev->context.eflags + 1); */
    asm volatile(".Lno_store_flags_registers:");                      /* else */
#  endif

#  if CCO_x86_ELIGIBLE_FOR_FXSR
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
    /* if(settings_make_fxsr_is_eligible(%ebx)) */
    asm volatile("test $(" ASMSTR(EXCHANGE_FPU_MMX_REGISTERS) "|" ASMSTR(EXCHANGE_SSE_REGISTERS) "), %ebx");
    asm volatile("jz .Lno_store_fpu_mmx_sse_registers"); /* then */
#    if !CCO_x86_ENABLE_EFLAGS_REGISTER_EXCHANGE
    asm volatile("addl $4, %ecx"); /* %ecx = &prev->context.fxsr; // 16-byte aligned*/
#    else
    asm volatile("test $" ASMSTR(EXCHANGE_EFLAGS_REGISTER) ", %ebx"); /* if (settings & EXCHANGE_EFLAGS_REGISTER) */
    asm volatile("jnz .Lno_pad_fxsr_storage");                        /* then */
    asm volatile("addl $4, %ecx");                                    /* %ecx = &prev->context.fxsr; // 16-byte aligned */
    asm volatile(".Lno_pad_fxsr_storage:");                           /* else */
#    endif
    asm volatile("pushl %edx");
    if(cco_x86_has_fxsr) {               /* if (has_fxsr) */
        asm volatile("fxsave (%ecx)");   /* fxsave(prev->context.fxsr); */
        asm volatile("addl $512, %ecx"); /* %ecx = (&prev->context.eflags + 1); */
    }
    else {
        // TODO: store FPU/MMX and SSE registers one by one
    }
    asm volatile("popl %edx");
    asm volatile(".Lno_store_fpu_mmx_sse_registers:");
#  elif CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE
#    define EXCHANGE_FPU_MMX_REGISTERS (1 << 1)
    _Static_assert(
        CCO_SETTINGS_x86_EXCHANGE_FPU_MMX_REGISTERS == EXCHANGE_FPU_MMX_REGISTERS,
        "The value of CCO_SETTINGS_x86_EXCHANGE_FPU_MMX_REGISTERS differs from the value of EXCHANGE_FPU_MMX_REGISTERS"
    );
    asm volatile("test $(" ASMSTR(EXCHANGE_FPU_MMX_REGISTERS) "), %ebx"); /* if(settings & exchange_fpu_mmx) */
    asm volatile("jz .Lno_store_fpu_mmx_registers");                      /* then */
    // TODO: store FPU/MMX registers one by one
    asm volatile(".Lno_store_fpu_mmx_registers:"); /* else */
#  elif CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE
#    define EXCHANGE_SSE_REGISTERS (1 << 2)
    _Static_assert(
        CCO_SETTINGS_x86_EXCHANGE_SSE_REGISTERS == EXCHANGE_SSE_REGISTERS,
        "The value of CCO_SETTINGS_x86_EXCHANGE_SSE_REGISTERS differs from the value of EXCHANGE_SSE_REGISTERS"
    );
    asm volatile("test %(" ASMSTR(EXCHANGE_SSE_REGISTERS) "), %ebx"); /* if(settings & exchange_sse) */
    asm volatile("jz .Lno_store_sse_registers");                      /* then */
    // TODO: store SSE registers one by one
    // asm volatile("addl $128, %ecx");
    asm volatile(".Lno_store_sse_registers:"); /* else */
#  endif

#  if CCO_x86_ENABLE_SEGMENT_REGISTERS_EXCHANGE
#    define EXCHANGE_SEGMENT_REGISTERS (1 << 3)
    _Static_assert(
        CCO_SETTINGS_x86_EXCHANGE_SEGMENT_REGISTERS == EXCHANGE_SEGMENT_REGISTERS,
        "The value of CCO_SETTINGS_x86_EXCHANGE_SEGMENT_REGISTERS differs from the value of EXCHANGE_SEGMENT_REGISTERS"
    );
    /* if(settings & CCO_SETTINGS_x86_EXCHANGE_SEGMENT_REGISTERS) { */
    asm volatile("test $" ASMSTR(EXCHANGE_SEGMENT_REGISTERS) ", %ebx"); /* if(settings & exchange_segment) */
    asm volatile("jz .Lno_store_segment_registers");                    /* then */
    asm volatile("movw %cs, 0x00(%ecx)");                               /* prev->context.cs = %cs; */
    asm volatile("movw %ds, 0x02(%ecx)");                               /* prev->context.ds = %ds; */
    asm volatile("movw %ss, 0x04(%ecx)");                               /* prev->context.ss = %ss; */
    asm volatile("movw %es, 0x06(%ecx)");                               /* prev->context.es = %es; */
    asm volatile("movw %fs, 0x08(%ecx)");                               /* prev->context.fs = %fs; */
    asm volatile("movw %gs, 0x0a(%ecx)");                               /* prev->context.gs = %gs; */
    asm volatile("addl $12, %ecx");                                     /* %ecx = (&prev->context.gs + 1); */
    asm volatile(".Lno_store_segment_registers:");                      /* else */
#  endif

#  if CCO_x86_ENABLE_DEBUG_REGISTERS_EXCHANGE
#    define EXCHANGE_DEBUG_REGISTERS (1 << 4)
    _Static_assert(
        CCO_SETTINGS_x86_EXCHANGE_DEBUG_REGISTERS == EXCHANGE_DEBUG_REGISTERS,
        "The value of CCO_SETTINGS_x86_EXCHANGE_DEBUG_REGISTERS differs from the value of EXCHANGE_DEBUG_REGISTERS"
    );
    asm volatile("test $" ASMSTR(EXCHANGE_DEBUG_REGISTERS) ", %ebx"); /* if(settings & exchange_debug) */
    asm volatile("jz .Lno_store_debug_registers");                    /* then */
    asm volatile("movl %dr0, 0x00(%ecx)");                            /* prev->context.dr0 = %dr0; */
    asm volatile("movl %dr1, 0x04(%ecx)");                            /* prev->context.dr1 = %dr1; */
    asm volatile("movl %dr2, 0x08(%ecx)");                            /* prev->context.dr2 = %dr2; */
    asm volatile("movl %dr3, 0x0c(%ecx)");                            /* prev->context.dr3 = %dr3; */
    asm volatile("movl %dr6, 0x10(%ecx)");                            /* prev->context.dr6 = %dr6; */
    asm volatile("movl %dr7, 0x14(%ecx)");                            /* prev->context.dr7 = %dr7; */
    asm volatile("addl $24, %ecx");                                   /* %ecx = (&prev->context.dr7 + 1); */
    asm volatile(".Lno_store_debug_registers:");                      /* else */
#  endif

#  if CCO_x86_ENABLE_CONTROL_REGISTERS_EXCHANGE
#    define EXCHANGE_CONTROL_REGISTERS (1 << 5)
    _Static_assert(
        CCO_SETTINGS_x86_EXCHANGE_CONTROL_REGISTERS == EXCHANGE_CONTROL_REGISTERS,
        "The value of CCO_SETTINGS_x86_EXCHANGE_CONTROL_REGISTERS differs from the value of EXCHANGE_CONTROL_REGISTERS"
    );
    asm volatile("test $" ASMSTR(EXCHANGE_CONTROL_REGISTERS) ", %ebx"); /* if(settings & exchange_control) */
    asm volatile("jz .Lno_store_control_registers");                    /* then */
    asm volatile("movl %cr0, 0x00(%ecx)");                              /* prev->context.cr0 = %cr0; */
    asm volatile("movl %cr2, 0x04(%ecx)");                              /* prev->context.cr2 = %cr2; */
    asm volatile("movl %cr3, 0x08(%ecx)");                              /* prev->context.cr3 = %cr3; */
    asm volatile("movl %cr4, 0x0c(%ecx)");                              /* prev->context.cr4 = %cr4; */
    asm volatile("movl %cr8, 0x10(%ecx)");                              /* prev->context.cr8 = %cr8; */
    asm volatile(".Lno_store_control_registers:");                      /* else */
#  endif

#  if !CCO_x86_BARE_CSWITCH
    asm volatile("movl 4(%edx), %ecx"); /* %ecx = next->settings; */
#  endif
    // TODO consider using pusha/popa
    asm volatile("movl (%edx), %edx");                               /* %edx = next->context; */
    asm volatile("movl 0x10(%edx), %esp");                           /* %esp = next->context.esp; */
    asm volatile("movl 0x14(%edx), %edi \n\t movl %edi, 0x0(%esp)"); /* *%esp = next->context.eip; // for ret */
    asm volatile("movl 0x0c(%edx), %ebp");                           /* %ebp = next->context.ebp; */
    asm volatile("movl 0x08(%edx), %edi");                           /* %edi = next->context.edi; */
    asm volatile("movl 0x04(%edx), %esi");                           /* %esi = next->context.esi; */
    asm volatile("movl 0x00(%edx), %ebx");                           /* %ebx = next->context.ebx; */
#  if !CCO_x86_BARE_CSWITCH
    asm volatile("addl $28, %edx"); /* %edx = (&next->context.eip + 1); */
#  endif

#  if CCO_x86_ENABLE_EFLAGS_REGISTER_EXCHANGE
    asm volatile("test $" ASMSTR(EXCHANGE_EFLAGS_REGISTER) ", %ecx"); /* if (settings & EXCHANGE_EFLAGS_REGISTER) */
    asm volatile("jz .Lno_restore_flags_registers");                  /* then */
    asm volatile("push (%edx)");                                      /* push(next->context.eflags); */
    asm volatile("popf");                                             /* %eflags = pop(); */
    asm volatile("addl $4, %edx");                                    /* %edx = (&next->context.eflags + 1); */
    asm volatile(".Lno_restore_flags_registers:");                    /* else */
#  endif

#  if CCO_x86_ELIGIBLE_FOR_FXSR
    asm volatile("test $(" ASMSTR(EXCHANGE_FPU_MMX_REGISTERS) "|" ASMSTR(EXCHANGE_SSE_REGISTERS) "), %ecx");
    asm volatile("jz .Lno_restore_fpu_mmx_sse_registers");
#    if !CCO_x86_ENABLE_EFLAGS_REGISTER_EXCHANGE
    asm volatile("addl $4, %edx"); /* %edx = &next->context.fxsr; // 16-byte aligned */
#    else
    asm volatile("test $" ASMSTR(EXCHANGE_EFLAGS_REGISTER) ", %ecx"); /* if (settings & EXCHANGE_EFLAGS_REGISTER) */
    asm volatile("jnz .Lno_pad_fxsr_restore");                        /* then */
    asm volatile("addl $4, %edx");                                    /* %edx = &next->context.fxsr; // 16-byte aligned */
    asm volatile(".Lno_pad_fxsr_restore:");                           /* else */
#    endif
    if(cco_x86_has_fxsr) {
        asm volatile("fxrstor (%edx)");
        asm volatile("addl $512, %edx");
    }
    else {
        // TODO: store FPU/MMX and SSE registers one by one
    }
    asm volatile(".Lno_restore_fpu_mmx_sse_registers:");
#  elif CCO_x86_ENABLE_FPU_MMX_REGISTERS_EXCHANGE
    asm volatile("test $(" ASMSTR(EXCHANGE_FPU_MMX_REGISTERS) "), %ecx");
    asm volatile("jz .Lno_restore_fpu_mmx_registers");
    asm volatile(".Lno_restore_fpu_mmx_registers:");
#  elif CCO_x86_ENABLE_SSE_REGISTERS_EXCHANGE
    asm volatile("test %(" ASMSTR(EXCHANGE_SSE_REGISTERS) "), %ecx");
    asm volatile("jz .Lno_restore_sse_registers");
    asm volatile(".Lno_restore_sse_registers:");
#  endif

#  if CCO_x86_ENABLE_SEGMENT_REGISTERS_EXCHANGE
    asm volatile("test $" ASMSTR(EXCHANGE_SEGMENT_REGISTERS) ", %ecx");
    asm volatile("jz .Lno_restore_segment_registers");
    asm volatile("movw %cs, 0x00(%edx)"); /* prev->context.cs = %cs; */
    asm volatile("movw %ds, 0x02(%edx)"); /* prev->context.ds = %ds; */
    asm volatile("movw %ss, 0x04(%edx)"); /* prev->context.ss = %ss; */
    asm volatile("movw %es, 0x06(%edx)"); /* prev->context.es = %es; */
    asm volatile("movw %fs, 0x08(%edx)"); /* prev->context.fs = %fs; */
    asm volatile("movw %gs, 0x0a(%edx)"); /* prev->context.gs = %gs; */
    asm volatile("addl $12, %edx");       /* %ecx = (&prev->context.gs + 1); */
    asm volatile(".Lno_restore_segment_registers:");
#  endif

#  if CCO_x86_ENABLE_DEBUG_REGISTERS_EXCHANGE
    asm volatile("test $" ASMSTR(EXCHANGE_DEBUG_REGISTERS) ", %ecx");
    asm volatile("jz .Lno_restore_debug_registers");
    asm volatile("movl %dr0, 0x00(%edx)"); /* prev->context.dr0 = %dr0; */
    asm volatile("movl %dr1, 0x04(%edx)"); /* prev->context.dr1 = %dr1; */
    asm volatile("movl %dr2, 0x08(%edx)"); /* prev->context.dr2 = %dr2; */
    asm volatile("movl %dr3, 0x0c(%edx)"); /* prev->context.dr3 = %dr3; */
    asm volatile("movl %dr6, 0x10(%edx)"); /* prev->context.dr6 = %dr6; */
    asm volatile("movl %dr7, 0x14(%edx)"); /* prev->context.dr7 = %dr7; */
    asm volatile("addl $24, %edx");        /* %ecx = (&prev->context.dr7 + 1); */
    asm volatile(".Lno_restore_debug_registers:");
#  endif

#  if CCO_x86_ENABLE_CONTROL_REGISTERS_EXCHANGE
    asm volatile("test $" ASMSTR(EXCHANGE_CONTROL_REGISTERS) ", %ecx");
    asm volatile("jz .Lno_restore_control_registers");
    asm volatile("movl %cr0, 0x00(%edx)"); /* prev->context.cr0 = %cr0; */
    asm volatile("movl %cr2, 0x04(%edx)"); /* prev->context.cr2 = %cr2; */
    asm volatile("movl %cr3, 0x08(%edx)"); /* prev->context.cr3 = %cr3; */
    asm volatile("movl %cr4, 0x0c(%edx)"); /* prev->context.cr4 = %cr4; */
    asm volatile("movl %cr8, 0x10(%edx)"); /* prev->context.cr8 = %cr8; */
    asm volatile(".Lno_restore_control_registers:");
#  endif
    asm volatile("ret");
#elif defined(_MSC_VER)
#elif defined(__ICC) || defined(__INTEL_COMPILER)
#else
#  error Unsupported compiler
#endif
}

#if CCO_x86_ELIGIBLE_FOR_FXSR
CCO_PRIVATE void ctor
cco_cpu_context_init()
{
    cco_x86_has_fxsr = cco_x86_retrieve_has_fxsr();
}
#endif

CCO_PRIVATE always_inline uint8_t*
cco_current_stack_pointer()
{
#if defined(__GNUC__) || defined(__clang__)
    uint8_t* sp;
    asm volatile("mov %%esp, %0" : "=r"(sp));
    return sp;
#elif defined(_MSC_VER)
    return (uint8_t*)_AddressOfReturnAddress();
#elif defined(__ICC) || defined(__INTEL_COMPILER)
    return (uint8_t*)__builtin_frame_address(0);
#else
#  error Unsupported compiler
#endif
}

CCO_PRIVATE always_inline uint8_t*
cco_get_stack_pointer(const cco_coroutine* coroutine)
{
    return (uint8_t*)coroutine->context->esp;
}

#endif