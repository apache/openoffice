/**************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *************************************************************/

// AArch64 (Apple Silicon, AAPCS64) outgoing-call trampoline for the C++-UNO
// bridge.  Loads the argument registers from caller-prepared arrays, copies
// any overflow arguments to the outgoing stack, performs the indirect call,
// and stores the integer and FP/SIMD return registers back.
//
// See AAPCS64_BRIDGE_SPEC.md.  Mach-O assembler syntax; symbols are prefixed
// with an underscore per the Darwin C ABI.
//
// extern "C" void callVirtualFunction(
//     sal_uInt64  pFunction,     // x0: target C++ virtual method
//     sal_uInt64  pIndirectRet,  // x1: value for x8 (indirect result ptr), 0 if none
//     sal_uInt64 *pGPR,          // x2: 8 words  -> x0..x7
//     double     *pFPR,          // x3: 8 doubles -> d0..d7
//     unsigned char *pStack,     // x4: packed overflow-argument bytes
//     sal_uInt32  nStackBytes,   // x5: byte count
//     sal_uInt64 *pGPRReturn,    // x6: [out] x0,x1
//     double     *pFPRReturn);   // x7: [out] d0..d3 (HFA up to 4 elements)

    .text
    .globl _callVirtualFunction
    .p2align 2
_callVirtualFunction:
    .cfi_startproc
    // prologue: save fp/lr and the callee-saved registers we use
    stp     x29, x30, [sp, #-16]!
    stp     x19, x20, [sp, #-16]!
    stp     x21, x22, [sp, #-16]!
    stp     x23, x24, [sp, #-16]!
    mov     x29, sp
    .cfi_def_cfa x29, 64
    .cfi_offset x29, -16
    .cfi_offset x30, -8
    .cfi_offset x19, -32
    .cfi_offset x20, -24
    .cfi_offset x21, -48
    .cfi_offset x22, -40
    .cfi_offset x23, -64
    .cfi_offset x24, -56

    // stash inputs that must survive the call into callee-saved registers
    mov     x19, x0                     // pFunction
    mov     x20, x2                     // pGPR
    mov     x21, x3                     // pFPR
    mov     x22, x6                     // pGPRReturn
    mov     x23, x7                     // pFPRReturn
    mov     x24, x1                     // x8 indirect-result value

    // allocate and copy the outgoing overflow stack arguments.
    // Round the packed byte area up to preserve 16-byte SP alignment.
    add     x9, x5, #15
    bic     x9, x9, #15
    sub     sp, sp, x9
    mov     x10, #0
Lcvf_copy:
    cmp     x10, x5
    b.ge    Lcvf_copied
    ldrb    w11, [x4, x10]
    strb    w11, [sp, x10]
    add     x10, x10, #1
    b       Lcvf_copy
Lcvf_copied:

    // load the FP/SIMD argument registers d0..d7
    ldp     d0, d1, [x21, #0]
    ldp     d2, d3, [x21, #16]
    ldp     d4, d5, [x21, #32]
    ldp     d6, d7, [x21, #48]

    // load the GP argument registers x0..x7 and the x8 indirect-result reg
    mov     x8, x24
    ldp     x6, x7, [x20, #48]
    ldp     x4, x5, [x20, #32]
    ldp     x2, x3, [x20, #16]
    ldp     x0, x1, [x20, #0]

    // perform the virtual call
    blr     x19

    // store the return registers
    str     x0, [x22, #0]
    str     x1, [x22, #8]
    str     d0, [x23, #0]
    str     d1, [x23, #8]
    str     d2, [x23, #16]
    str     d3, [x23, #24]

    // epilogue
    mov     sp, x29
    ldp     x23, x24, [sp], #16
    ldp     x21, x22, [sp], #16
    ldp     x19, x20, [sp], #16
    ldp     x29, x30, [sp], #16
    ret
    .cfi_endproc

// ---------------------------------------------------------------------------
// privateSnippetExecutor: the incoming (cpp2uno) register-spill executor.
//
// Reached by a BR from a per-vtable-slot code snippet (see codeSnippet() in
// cpp2uno.cxx) with:
//   x16 = (nVtableOffset << 32) | nFunctionIndex  (low bit 0x80000000 flags a
//         hidden/indirect return)
//   x0..x7, d0..d7, x8 = the original incoming arguments (untouched)
//   sp  = the caller's stack-argument area (overflow)
//   x30 = return address back into the original C++ caller
//
// It spills the argument registers to a save area and calls cpp_vtable_call,
// then loads the return value back into x0/x1 and d0/d1 (or d0 for fp).
//
//   typelib_TypeClass cpp_vtable_call(
//       sal_Int32 nFunctionIndex, sal_Int32 nVtableOffset,
//       void** gpreg, void** fpreg, void** ovrflw,
//       void* pIndirectReturn, sal_uInt64* pRegisterReturn);
//
// Frame (176 bytes): [0]=x29,x30  [16..79]=x0..x7  [80..143]=d0..d7
//                    [144..175]=return buffer (up to four HFA doubles).
    .globl _privateSnippetExecutor
    .p2align 2
_privateSnippetExecutor:
    .cfi_startproc
    mov     x17, sp                     // x17 = ovrflw (incoming stack args)
    stp     x29, x30, [sp, #-176]!
    mov     x29, sp
    .cfi_def_cfa x29, 176
    .cfi_offset x29, -176
    .cfi_offset x30, -168

    stp     x0, x1, [sp, #16]           // save GP argument registers x0..x7
    stp     x2, x3, [sp, #32]
    stp     x4, x5, [sp, #48]
    stp     x6, x7, [sp, #64]

    stp     d0, d1, [sp, #80]           // save FP/SIMD argument registers d0..d7
    stp     d2, d3, [sp, #96]
    stp     d4, d5, [sp, #112]
    stp     d6, d7, [sp, #128]

    mov     w0, w16                     // nFunctionIndex (low 32 bits)
    lsr     x1, x16, #32                // nVtableOffset (high 32 bits)
    add     x2, sp, #16                 // gpreg
    add     x3, sp, #80                 // fpreg
    mov     x4, x17                     // ovrflw
    mov     x5, x8                      // pIndirectReturn (x8 indirect-result reg)
    add     x6, sp, #144                // pRegisterReturn (32-byte buffer)
    bl      _cpp_vtable_call

    cmp     w0, #0x100                   // RETURN_KIND_HFA_FLOAT
    b.eq    Lpse_hfa_float
    cmp     w0, #0x101                   // RETURN_KIND_HFA_DOUBLE
    b.eq    Lpse_hfa_double
    cmp     w0, #10                     // typelib_TypeClass_FLOAT
    b.eq    Lpse_float
    cmp     w0, #11                     // typelib_TypeClass_DOUBLE
    b.eq    Lpse_float
    cmp     w0, #3                      // typelib_TypeClass_BYTE
    b.eq    Lpse_signed_byte
    cmp     w0, #4                      // typelib_TypeClass_SHORT
    b.eq    Lpse_signed_short
    cmp     w0, #1                      // typelib_TypeClass_CHAR
    b.eq    Lpse_unsigned_short
    cmp     w0, #5                      // typelib_TypeClass_UNSIGNED_SHORT
    b.eq    Lpse_unsigned_short
    cmp     w0, #2                      // typelib_TypeClass_BOOLEAN
    b.eq    Lpse_unsigned_byte
    cmp     w0, #6                      // typelib_TypeClass_LONG
    b.eq    Lpse_word
    cmp     w0, #7                      // typelib_TypeClass_UNSIGNED_LONG
    b.eq    Lpse_word
    cmp     w0, #15                     // typelib_TypeClass_ENUM
    b.eq    Lpse_word
    // Integer / pointer / <=16B aggregate.
    ldp     x0, x1, [sp, #144]
    b       Lpse_done
Lpse_float:
    ldr     d0, [sp, #144]
    b       Lpse_done
Lpse_hfa_float:
    ldr     s0, [sp, #144]
    ldr     s1, [sp, #148]
    ldr     s2, [sp, #152]
    ldr     s3, [sp, #156]
    b       Lpse_done
Lpse_hfa_double:
    ldp     d0, d1, [sp, #144]
    ldp     d2, d3, [sp, #160]
    b       Lpse_done
Lpse_signed_byte:
    ldrsb   w0, [sp, #144]
    b       Lpse_done
Lpse_unsigned_byte:
    ldrb    w0, [sp, #144]
    b       Lpse_done
Lpse_signed_short:
    ldrsh   w0, [sp, #144]
    b       Lpse_done
Lpse_unsigned_short:
    ldrh    w0, [sp, #144]
    b       Lpse_done
Lpse_word:
    ldr     w0, [sp, #144]
Lpse_done:
    mov     sp, x29
    ldp     x29, x30, [sp], #176
    ret
    .cfi_endproc
