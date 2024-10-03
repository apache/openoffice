	.file	"uno_ppc64_asm.cc"
	.machine power4
	.abiversion 2
	.section	".text"
	.align 2
	.globl callVirtualMethod
	.type	callVirtualMethod, @function
callVirtualMethod:
.LFB0:
	.cfi_startproc
.LCF0:
0:	addis 2,12,.TOC.-.LCF0@ha
	addi 2,2,.TOC.-.LCF0@l
	.localentry	callVirtualMethod,.-callVirtualMethod
	mflr 0
	std 0,16(1)
	std 31,-8(1)
	stdu 1,-208(1)
	.cfi_def_cfa_offset 208
	.cfi_offset 65, 16
	.cfi_offset 31, -8
	mr 31,1
	.cfi_def_cfa_register 31
	std 3,136(31)
	std 5,128(31)
	std 6,120(31)
	std 7,112(31)
	std 9,104(31)
	mr 9,4
	stw 9,248(31)
	mr 9,8
	stw 9,280(31)
	mr 9,10
	stw 9,296(31)
	ld 9,304(31)
	std 9,96(31)
	ld 9,-28688(13)
	std 9,184(31)
	li 9,0
	lwz 9,280(31)
	cmpwi 0,9,0
	beq 0,.L2
	lwz 9,280(31)
	addi 9,9,1
	rldicl 9,9,0,32
	rlwinm 9,9,0,0,30
	stw 9,280(31)
.L2:
#ifdef __LITTLE_ENDIAN__
	lwz 9,312(31)
#else
	lwz 9,316(31)
#endif
	cmplwi 0,9,13
	ble 0,.L3
	li 9,13
#ifdef __LITTLE_ENDIAN__
	stw 9,312(31)
#else
	stw 9,316(31)
#endif
.L3:
	lwz 9,296(31)
	cmplwi 0,9,8
	ble 0,.L4
	li 9,8
	stw 9,296(31)
.L4:
	lwz 9,280(31)
	slwi 9,9,3
	rldicl 9,9,0,32
	addi 9,9,15
	srdi 9,9,4
	sldi 9,9,4
	ld 10,0(1)
	neg 9,9
	stdux 10,1,9
	addi 9,1,96
	addi 9,9,15
	srdi 9,9,4
	sldi 9,9,4
	std 9,160(31)
	lwz 9,280(31)
	slwi 9,9,3
	rldicl 9,9,0,32
	mr 8,9
	ld 10,160(31)
	ld 9,112(31)
	mr 5,8
	mr 4,9
	mr 3,10
	bl memcpy
	nop
	ld 9,136(31)
	ld 9,0(9)
	std 9,168(31)
	lwz 9,248(31)
	slwi 9,9,3
	rldicl 9,9,0,32
	mr 10,9
	ld 9,168(31)
	add 9,9,10
	std 9,168(31)
	ld 9,168(31)
	ld 9,0(9)
	std 9,168(31)
	ld 9,168(31)
	std 9,176(31)
	ld 9,96(31)
#APP
 # 123 "uno_ppc64_asm.cc" 1
	lfd  1,  0(9)
	lfd  2,  8(9)
	lfd  3, 16(9)
	lfd  4, 24(9)
	lfd  5, 32(9)
	lfd  6, 40(9)
	lfd  7, 48(9)
	lfd  8, 56(9)
	lfd  9, 64(9)
	lfd 10, 72(9)
	lfd 11, 80(9)
	lfd 12, 88(9)
	lfd 13, 96(9)
	
 # 0 "" 2
#NO_APP
	ld 9,104(31)
	ld 3,0(9)
	ld 9,104(31)
	addi 9,9,8
	ld 4,0(9)
	ld 9,104(31)
	addi 9,9,16
	ld 5,0(9)
	ld 9,104(31)
	addi 9,9,24
	ld 6,0(9)
	ld 9,104(31)
	addi 9,9,32
	ld 7,0(9)
	ld 9,104(31)
	addi 9,9,40
	ld 8,0(9)
	ld 9,104(31)
	addi 9,9,48
	ld 0,0(9)
	ld 9,104(31)
	addi 9,9,56
	ld 9,0(9)
	ld 11,176(31)
	mr 10,9
	mr 9,0
	std 2,24(1)
	mr 12,11
	mtctr 12
	bctrl
	ld 2,24(1)
#APP
 # 149 "uno_ppc64_asm.cc" 1
	mr     3,     3
	mr     4,     4
	fmr    0,     1
	
 # 0 "" 2
#NO_APP
	stfd 0,152(31)
	mr 9,3
	mr 10,4
	lfd 0,152(31)
	ld 7,128(31)
	ld 6,120(31)
	fmr 1,0
	mr 4,10
	mr 3,9
	bl MapReturn
	nop
	nop
	ld 9,184(31)
	ld 10,-28688(13)
	xor. 9,9,10
	li 10,0
	beq 0,.L5
	bl __stack_chk_fail
	nop
.L5:
	addi 1,31,208
	.cfi_def_cfa 1, 0
	ld 0,16(1)
	mtlr 0
	ld 31,-8(1)
	blr
	.long 0
	.byte 0,9,0,1,128,1,0,1
	.cfi_endproc
.LFE0:
	.size	callVirtualMethod,.-callVirtualMethod
	.section	".toc","aw"
	.align 3
.LC0:
	.quad	.L9
	.section	".text"
	.align 2
	.globl privateSnippetExecutor
	.type	privateSnippetExecutor, @function
privateSnippetExecutor:
.LFB1:
	.cfi_startproc
.LCF1:
0:	addis 2,12,.TOC.-.LCF1@ha
	addi 2,2,.TOC.-.LCF1@l
	.localentry	privateSnippetExecutor,.-privateSnippetExecutor
	mflr 0
	std 0,16(1)
	std 31,-8(1)
	stdu 1,-272(1)
	.cfi_def_cfa_offset 272
	.cfi_offset 65, 16
	.cfi_offset 31, -8
	mr 31,1
	.cfi_def_cfa_register 31
	ld 0,-28688(13)
	std 0,248(31)
	li 0,0
	std 3,80(31)
	std 4,88(31)
	std 5,96(31)
	std 6,104(31)
	std 7,112(31)
	std 8,120(31)
	std 9,128(31)
	mr 9,10
	std 9,136(31)
	addi 9,31,144
#APP
 # 173 "uno_ppc64_asm.cc" 1
	stfd 1,   0(9)	
stfd 2,   8(9)	
stfd 3,  16(9)	
stfd 4,  24(9)	
stfd 5,  32(9)	
stfd 6,  40(9)	
stfd 7,  48(9)	
stfd 8,  56(9)	
stfd 9,  64(9)	
stfd 10, 72(9)	
stfd 11, 80(9)	
stfd 12, 88(9)	
stfd 13, 96(9)	

 # 0 "" 2
#NO_APP
	std 11,48(31)
	std 1,56(31)
	ld 9,48(31)
	addi 7,31,64
	addi 8,31,144
	addi 10,31,80
	ld 6,56(31)
	mr 5,8
	mr 4,10
	mr 3,9
	bl cpp_mediate
	nop
	mr 9,3
	stw 9,44(31)
	lwa 9,44(31)
	cmplwi 0,9,15
	bgt 0,.L7
	sldi 10,9,2
	addis 8,2,.LC0@toc@ha
	ld 9,.LC0@toc@l(8)
	add 9,10,9
	lwz 10,0(9)
	ld 9,.LC0@toc@l(8)
	extsw 10,10
	add 9,10,9
	mtctr 9
	bctr
	.p2align 2
	.align 2
.L9:
	.long .L19-.L9
	.long .L13-.L9
	.long .L15-.L9
	.long .L15-.L9
	.long .L14-.L9
	.long .L13-.L9
	.long .L12-.L9
	.long .L8-.L9
	.long .L7-.L9
	.long .L7-.L9
	.long .L11-.L9
	.long .L10-.L9
	.long .L7-.L9
	.long .L7-.L9
	.long .L7-.L9
	.long .L8-.L9
.L15:
#APP
 # 209 "uno_ppc64_asm.cc" 1
	lbz 3,64(31)
	
 # 0 "" 2
#NO_APP
	b .L17
.L13:
#APP
 # 214 "uno_ppc64_asm.cc" 1
	lhz 3,64(31)
	
 # 0 "" 2
#NO_APP
	b .L17
.L14:
#APP
 # 218 "uno_ppc64_asm.cc" 1
	lha 3,64(31)
	
 # 0 "" 2
#NO_APP
	b .L17
.L8:
#APP
 # 223 "uno_ppc64_asm.cc" 1
	lwz 3,64(31)
	
 # 0 "" 2
#NO_APP
	b .L17
.L12:
#APP
 # 227 "uno_ppc64_asm.cc" 1
	lwa 3,64(31)
	
 # 0 "" 2
#NO_APP
	b .L17
.L11:
	addi 9,31,64
#APP
 # 231 "uno_ppc64_asm.cc" 1
	lfs 1,0(9)
	
 # 0 "" 2
#NO_APP
	b .L17
.L10:
	addi 9,31,64
#APP
 # 235 "uno_ppc64_asm.cc" 1
	lfd 1,0(9)
	
 # 0 "" 2
#NO_APP
	b .L17
.L7:
#APP
 # 239 "uno_ppc64_asm.cc" 1
	ld 3,64(31)
	
 # 0 "" 2
 # 241 "uno_ppc64_asm.cc" 1
	ld 4,72(31)
	
 # 0 "" 2
#NO_APP
	b .L17
.L19:
	nop
.L17:
	nop
	ld 9,248(31)
	ld 10,-28688(13)
	xor. 9,9,10
	li 10,0
	beq 0,.L18
	bl __stack_chk_fail
	nop
.L18:
	addi 1,31,272
	.cfi_def_cfa 1, 0
	ld 0,16(1)
	mtlr 0
	ld 31,-8(1)
	blr
	.long 0
	.byte 0,9,0,1,128,1,0,1
	.cfi_endproc
.LFE1:
	.size	privateSnippetExecutor,.-privateSnippetExecutor
	.section	.rodata
	.align 2
	.type	_ZL15codeSnippetSize, @object
	.size	_ZL15codeSnippetSize, 4
_ZL15codeSnippetSize:
	.long	32
	.section	".text"
	.align 2
	.globl codeSnippet
	.type	codeSnippet, @function
codeSnippet:
.LFB2:
	.cfi_startproc
.LCF2:
0:	addis 2,12,.TOC.-.LCF2@ha
	addi 2,2,.TOC.-.LCF2@l
	.localentry	codeSnippet,.-codeSnippet
	std 31,-8(1)
	stdu 1,-96(1)
	.cfi_def_cfa_offset 96
	.cfi_offset 31, -8
	mr 31,1
	.cfi_def_cfa_register 31
	std 3,56(31)
	mr 9,4
	mr 8,5
	mr 10,6
	stw 9,52(31)
	mr 9,8
	stw 9,48(31)
	mr 9,10
	stb 9,47(31)
	lwa 9,48(31)
	sldi 10,9,32
	lwa 9,52(31)
	or 9,10,9
	std 9,64(31)
	lbz 9,47(31)
	cmpwi 0,9,0
	beq 0,.L21
	ld 9,64(31)
	oris 9,9,0x8000
	std 9,64(31)
.L21:
	ld 9,56(31)
	std 9,72(31)
	ld 9,72(31)
	lis 10,0xe96c
	ori 10,10,0x18
	stw 10,0(9)
	ld 9,72(31)
	addi 9,9,4
	lis 10,0xe98c
	ori 10,10,0x10
	stw 10,0(9)
	ld 9,72(31)
	addi 9,9,8
	lis 10,0x7d89
	ori 10,10,0x3a6
	stw 10,0(9)
	ld 9,72(31)
	addi 9,9,12
	lis 10,0x4e80
	ori 10,10,0x420
	stw 10,0(9)
	ld 9,72(31)
	addi 9,9,16
	addis 10,2,privateSnippetExecutor@toc@ha
	addi 10,10,privateSnippetExecutor@toc@l
	std 10,0(9)
	ld 9,72(31)
	addi 9,9,24
	ld 10,64(31)
	std 10,0(9)
	ld 9,56(31)
	addi 9,9,32
	mr 3,9
	addi 1,31,96
	.cfi_def_cfa 1, 0
	ld 31,-8(1)
	blr
	.long 0
	.byte 0,9,0,0,128,1,0,1
	.cfi_endproc
.LFE2:
	.size	codeSnippet,.-codeSnippet
	.section	.note.GNU-stack,"",@progbits
