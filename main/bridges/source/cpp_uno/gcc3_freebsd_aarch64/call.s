/*
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
 */

// AArch64 (System V / FreeBSD) outgoing-call trampoline for the C++-UNO
// bridge. Loads registers from caller-prepared arrays, copies overflow args
// to the outgoing stack, performs the indirect call and stores return regs.

	.text
	.align 2
	.globl callVirtualFunction
	.type	callVirtualFunction, @function
callVirtualFunction:
	.cfi_startproc
	// prologue: save fp/lr and the callee-saved registers we use
	stp	x29, x30, [sp, #-16]!
	stp	x19, x20, [sp, #-16]!
	stp	x21, x22, [sp, #-16]!
	stp	x23, x24, [sp, #-16]!
	mov	x29, sp
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
	mov	x19, x0			// pFunction
	mov	x20, x2			// pGPR
	mov	x21, x3			// pFPR
	mov	x22, x6			// pGPRReturn
	mov	x23, x7			// pFPRReturn
	mov	x24, x1			// x8 indirect-result value

	// allocate and copy the outgoing overflow stack arguments.
	add	x9, x5, #15
	bic	x9, x9, #15
	sub	sp, sp, x9
	mov	x10, #0
Lcvf_copy:
	cmp	x10, x5
	b.ge	Lcvf_copied
	ldrb	w11, [x4, x10]
	strb	w11, [sp, x10]
	add	x10, x10, #1
	b	Lcvf_copy
Lcvf_copied:

	// load the FP/SIMD argument registers d0..d7
	ldp	d0, d1, [x21, #0]
	ldp	d2, d3, [x21, #16]
	ldp	d4, d5, [x21, #32]
	ldp	d6, d7, [x21, #48]

	// load the GP argument registers x0..x7 and the x8 indirect-result reg
	mov	x8, x24
	ldp	x6, x7, [x20, #48]
	ldp	x4, x5, [x20, #32]
	ldp	x2, x3, [x20, #16]
	ldp	x0, x1, [x20, #0]

	// perform the virtual call
	blr	x19

	// store the return registers
	str	x0, [x22, #0]
	str	x1, [x22, #8]
	str	d0, [x23, #0]
	str	d1, [x23, #8]
	str	d2, [x23, #16]
	str	d3, [x23, #24]

	// epilogue
	mov	sp, x29
	ldp	x23, x24, [sp], #16
	ldp	x21, x22, [sp], #16
	ldp	x19, x20, [sp], #16
	ldp	x29, x30, [sp], #16
	ret
	.cfi_endproc

// ---------------------------------------------------------------------------
// privateSnippetExecutor: incoming (cpp2uno) register-spill executor.

	.globl privateSnippetExecutor
	.type	privateSnippetExecutor, @function
privateSnippetExecutor:
	.cfi_startproc
	mov	x17, sp			// x17 = ovrflw (incoming stack args)
	stp	x29, x30, [sp, #-176]!
	mov	x29, sp
	.cfi_def_cfa x29, 176
	.cfi_offset x29, -176
	.cfi_offset x30, -168

	stp	x0, x1, [sp, #16]		// save GP argument registers x0..x7
	stp	x2, x3, [sp, #32]
	stp	x4, x5, [sp, #48]
	stp	x6, x7, [sp, #64]

	stp	d0, d1, [sp, #80]		// save FP/SIMD argument registers d0..d7
	stp	d2, d3, [sp, #96]
	stp	d4, d5, [sp, #112]
	stp	d6, d7, [sp, #128]

	mov	w0, w16			// nFunctionIndex (low 32 bits)
	lsr	x1, x16, #32		// nVtableOffset (high 32 bits)
	add	x2, sp, #16		// gpreg
	add	x3, sp, #80		// fpreg
	mov	x4, x17			// ovrflw
	mov	x5, x8			// pIndirectReturn (x8 indirect-result reg)
	add	x6, sp, #144		// pRegisterReturn (32-byte buffer)
	bl	cpp_vtable_call

	cmp	w0, #0x100			// RETURN_KIND_HFA_FLOAT
	b.eq	Lpse_hfa_float
	cmp	w0, #0x101			// RETURN_KIND_HFA_DOUBLE
	b.eq	Lpse_hfa_double
	cmp	w0, #10			// typelib_TypeClass_FLOAT
	b.eq	Lpse_float
	cmp	w0, #11			// typelib_TypeClass_DOUBLE
	b.eq	Lpse_float
	cmp	w0, #3			// typelib_TypeClass_BYTE
	b.eq	Lpse_signed_byte
	cmp	w0, #4			// typelib_TypeClass_SHORT
	b.eq	Lpse_signed_short
	cmp	w0, #1			// typelib_TypeClass_VOID
	b.eq	Lpse_void

	// integer / pointer return
	ldr	x0, [x6, #0]
	ldr	x1, [x6, #8]
	ldr	d0, [x6, #16]
	ldr	d1, [x6, #24]
	b	Lpse_finish

Lpse_hfa_float:
	// HFA float: up to 4 floats in d0..d3
	ldr	d0, [x6, #0]
	ldr	d1, [x6, #8]
	ldr	d2, [x6, #16]
	ldr	d3, [x6, #24]
	b	Lpse_finish

Lpse_hfa_double:
	// HFA double: up to 4 doubles in d0..d3
	ldr	d0, [x6, #0]
	ldr	d1, [x6, #8]
	ldr	d2, [x6, #16]
	ldr	d3, [x6, #24]
	b	Lpse_finish

Lpse_float:
	// single float/double returned in d0
	ldr	d0, [x6, #16]
	b	Lpse_finish

Lpse_signed_byte:
	ldr	x0, [x6, #0]
	b	Lpse_finish

Lpse_signed_short:
	ldr	x0, [x6, #0]
	b	Lpse_finish

Lpse_void:
	mov	x0, #0
	b	Lpse_finish

Lpse_finish:
	ldp	x23, x24, [sp], #16
	ldp	x21, x22, [sp], #16
	ldp	x19, x20, [sp], #16
	ldp	x29, x30, [sp], #16
	ret
	.cfi_endproc

	.section	.note.GNU-stack,"",@progbits
