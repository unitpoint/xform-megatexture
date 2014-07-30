#include <xForm.h>

//===============================================================
//                                                        M
//  SSE implementation of xSIMDProcessor                MrE
//                                                        E
//===============================================================

#ifdef _WIN32

#include <xmmintrin.h>

#define SHUFFLEPS(x, y, z, w)		(((x) & 3) << 6 | ((y) & 3) << 4 | ((z) & 3) << 2 | ((w) & 3))
#define R_SHUFFLEPS(x, y, z, w)	(((w) & 3) << 6 | ((z) & 3) << 4 | ((y) & 3) << 2 | ((x) & 3))

// transpose a 4x4 matrix loaded into 4 xmm registers (reg4 is temporary)
#define TRANSPOSE_4x4(reg0, reg1, reg2, reg3, reg4)											\
	__asm	movaps		reg4, reg2								/* reg4 =  8,  9, 10, 11 */		\
	__asm	unpcklps	reg2, reg3								/* reg2 =  8, 12,  9, 13 */		\
	__asm	unpckhps	reg4, reg3								/* reg4 = 10, 14, 11, 15 */		\
	__asm	movaps		reg3, reg0								/* reg3 =  0,  1,  2,  3 */		\
	__asm	unpcklps	reg0, reg1								/* reg0 =  0,  4,  1,  5 */		\
	__asm	unpckhps	reg3, reg1								/* reg3 =  2,  6,  3,  7 */		\
	__asm	movaps		reg1, reg0								/* reg1 =  0,  4,  1,  5 */		\
	__asm	shufps		reg0, reg2, R_SHUFFLEPS(0, 1, 0, 1)	/* reg0 =  0,  4,  8, 12 */		\
	__asm	shufps		reg1, reg2, R_SHUFFLEPS(2, 3, 2, 3)	/* reg1 =  1,  5,  9, 13 */		\
	__asm	movaps		reg2, reg3								/* reg2 =  2,  6,  3,  7 */		\
	__asm	shufps		reg2, reg4, R_SHUFFLEPS(0, 1, 0, 1)	/* reg2 =  2,  6, 10, 14 */		\
	__asm	shufps		reg3, reg4, R_SHUFFLEPS(2, 3, 2, 3)	/* reg3 =  3,  7, 11, 15 */

// transpose a 4x4 matrix from memory into 4 xmm registers (reg4 is temporary)
#define TRANPOSE_4x4_FROM_MEMORY(address, reg0, reg1, reg2, reg3, reg4)						\
	__asm	movlps		reg1, [address+ 0]						/* reg1 =  0,  1,  X,  X */		\
	__asm	movlps		reg3, [address+ 8]						/* reg3 =  2,  3,  X,  X */		\
	__asm	movhps		reg1, [address+16]						/* reg1 =  0,  1,  4,  5 */		\
	__asm	movhps		reg3, [address+24]						/* reg3 =  2,  3,  6,  7 */		\
	__asm	movlps		reg2, [address+32]						/* reg2 =  8,  9,  X,  X */		\
	__asm	movlps		reg4, [address+40]						/* reg4 = 10, 11,  X,  X */		\
	__asm	movhps		reg2, [address+48]						/* reg2 =  8,  9, 12, 13 */		\
	__asm	movhps		reg4, [address+56]						/* reg4 = 10, 11, 14, 15 */		\
	__asm	movaps		reg0, reg1								/* reg0 =  0,  1,  4,  5 */		\
	__asm	shufps		reg0, reg2, R_SHUFFLEPS(0, 2, 0, 2)	/* reg0 =  0,  4,  8, 12 */		\
	__asm	shufps		reg1, reg2, R_SHUFFLEPS(1, 3, 1, 3)	/* reg1 =  1,  5,  9, 13 */		\
	__asm	movaps		reg2, reg3								/* reg2 =  2,  3,  6,  7 */		\
	__asm	shufps		reg2, reg4, R_SHUFFLEPS(0, 2, 0, 2)	/* reg2 =  2,  6, 10, 14 */		\
	__asm	shufps		reg3, reg4, R_SHUFFLEPS(1, 3, 1, 3)	/* reg3 =  3,  7, 11, 15 */

// transpose a 4x4 matrix to memory from 4 xmm registers (reg4 is temporary)
#define TRANPOSE_4x4_TO_MEMORY(address, reg0, reg1, reg2, reg3, reg4)							\
	__asm	movaps		reg4, reg0								/* reg4 =  0,  4,  8, 12 */		\
	__asm	unpcklps	reg0, reg1								/* reg0 =  0,  1,  4,  5 */		\
	__asm	unpckhps	reg4, reg1								/* reg4 =  8,  9, 12, 13 */		\
	__asm	movaps		reg1, reg2								/* reg1 =  2,  6, 10, 14 */		\
	__asm	unpcklps	reg2, reg3								/* reg2 =  2,  3,  6,  7 */		\
	__asm	unpckhps	reg1, reg3								/* reg1 = 10, 11, 14, 15 */		\
	__asm	movlps		[address+ 0], reg0						/* mem0 =  0,  1,  X,  X */		\
	__asm	movlps		[address+ 8], reg2						/* mem0 =  0,  1,  2,  3 */		\
	__asm	movhps		[address+16], reg0						/* mem1 =  4,  5,  X,  X */		\
	__asm	movhps		[address+24], reg2						/* mem1 =  4,  5,  6,  7 */		\
	__asm	movlps		[address+32], reg4						/* mem2 =  8,  9,  X,  X */		\
	__asm	movlps		[address+40], reg1						/* mem2 =  8,  9, 10, 11 */		\
	__asm	movhps		[address+48], reg4						/* mem3 = 12, 13,  X,  X */		\
	__asm	movhps		[address+56], reg1						/* mem3 = 12, 13, 14, 15 */

// transpose a 4x3 matrix loaded into 3 xmm registers (reg3 is temporary)
#define TRANSPOSE_4x3(reg0, reg1, reg2, reg3)													\
	__asm	movaps		reg3, reg2								/* reg3 =  8,  9, 10, 11 */		\
	__asm	shufps		reg3, reg1, R_SHUFFLEPS(2, 3, 0, 1)	/* reg3 = 10, 11,  4,  5 */		\
	__asm	shufps		reg2, reg0, R_SHUFFLEPS(0, 1, 2, 3)	/* reg2 =  8,  9,  2,  3 */		\
	__asm	shufps		reg1, reg0, R_SHUFFLEPS(2, 3, 0, 1)	/* reg1 =  6,  7,  0,  1 */		\
	__asm	movaps		reg0, reg1								/* reg0 =  6,  7,  0,  1 */		\
	__asm	shufps		reg0, reg2, R_SHUFFLEPS(2, 0, 3, 1)	/* reg0 =  0,  6,  3,  9 */		\
	__asm	shufps		reg1, reg3, R_SHUFFLEPS(3, 1, 2, 0)	/* reg1 =  1,  7,  4, 10 */		\
	__asm	shufps		reg2, reg3, R_SHUFFLEPS(2, 0, 3, 1)	/* reg2 =  2,  8,  5, 11 */

// transpose a 4x3 matrix from memory into 3 xmm registers (reg3 is temporary)
#define TRANSPOSE_4x3_FROM_MEMORY(address, reg0, reg1, reg2, reg3)							\
	__asm	movlps		reg1, [address+ 0]						/* reg1 =  0,  1,  X,  X */		\
	__asm	movlps		reg2, [address+ 8]						/* reg2 =  2,  3,  X,  X */		\
	__asm	movlps		reg3, [address+16]						/* reg3 =  4,  5,  X,  X */		\
	__asm	movhps		reg1, [address+24]						/* reg1 =  0,  1,  6,  7 */		\
	__asm	movhps		reg2, [address+32]						/* reg2 =  2,  3,  8,  9 */		\
	__asm	movhps		reg3, [address+40]						/* reg3 =  4,  5, 10, 11 */		\
	__asm	movaps		reg0, reg1								/* reg0 =  0,  1,  6,  7 */		\
	__asm	shufps		reg0, reg2, R_SHUFFLEPS(0, 2, 1, 3)	/* reg0 =  0,  6,  3,  9 */		\
	__asm	shufps		reg1, reg3, R_SHUFFLEPS(1, 3, 0, 2)	/* reg1 =  1,  7,  4, 10 */		\
	__asm	shufps		reg2, reg3, R_SHUFFLEPS(0, 2, 1, 3)	/* reg2 =  2,  8,  5, 11 */

// transpose a 4x3 matrix to memory from 3 xmm registers (reg3 is temporary)
#define TRANSPOSE_4x3_TO_MEMORY(address, reg0, reg1, reg2, reg3)								\
	__asm	movhlps		reg3, reg0 								/* reg3 =  3,  9,  X,  X */		\
	__asm	unpcklps	reg0, reg1								/* reg0 =  0,  1,  6,  7 */		\
	__asm	unpckhps	reg1, reg2								/* reg1 =  4,  5, 10, 11 */		\
	__asm	unpcklps	reg2, reg3								/* reg2 =  2,  3,  8,  9 */		\
	__asm	movlps		[address+ 0], reg0						/* mem0 =  0,  1,  X,  X */		\
	__asm	movlps		[address+ 8], reg2						/* mem0 =  0,  1,  2,  3 */		\
	__asm	movlps		[address+16], reg1						/* mem1 =  4,  5,  X,  X */		\
	__asm	movhps		[address+24], reg0						/* mem1 =  4,  5,  6,  7 */		\
	__asm	movhps		[address+32], reg2						/* mem2 =  8,  9,  X,  X */		\
	__asm	movhps		[address+40], reg1						/* mem2 =  8,  9, 10, 11 */


// with alignment
#define KFLOATINITS(SRC0, COUNT, PRE, POST)				KFLOATINITDSS(SRC0,SRC0,SRC0,COUNT,PRE,POST)
#define KFLOATINITD(DST, COUNT, PRE, POST)				KFLOATINITDSS(DST,DST,DST,COUNT,PRE,POST)
#define KFLOATINITDS(DST, SRC0, COUNT, PRE, POST)		KFLOATINITDSS(DST,SRC0,SRC0,COUNT,PRE,POST)

#define KFLOATINITDSS(DST, SRC0, SRC1, COUNT, PRE, POST)\
	__asm	mov		ecx,DST								\
	__asm	shr		ecx,2								\
	__asm	mov		ebx,COUNT							\
	__asm	neg		ecx									\
	__asm	mov		edx,SRC0							\
	__asm	and		ecx,3								\
	__asm	mov		esi,SRC1							\
	__asm	sub		ebx,ecx								\
	__asm	jge		noUnderFlow							\
	__asm	xor		ebx,ebx								\
	__asm	mov		ecx,COUNT							\
	__asm	noUnderFlow:								\
	__asm	mov		PRE,ecx								\
	__asm	mov		eax,ebx								\
	__asm	mov		edi,DST								\
	__asm	and		eax,8-1								\
	__asm	mov		POST,eax							\
	__asm	and		ebx,0xfffffff8						\
	__asm	jle		done								\
	__asm	shl		ebx,2								\
	__asm	lea		ecx,[ecx*4+ebx]						\
	__asm	neg		ebx									\
	__asm	add		edx,ecx								\
	__asm	add		esi,ecx								\
	__asm	add		edi,ecx								\
	__asm	mov		eax,edx								\
	__asm	or		eax,esi

// without alignment (pre==0)
#define KFLOATINITS_NA(SRC0, COUNT, PRE, POST)				KFLOATINITDSS_NA(SRC0,SRC0,SRC0,COUNT,PRE,POST)
#define KFLOATINITD_NA(DST, COUNT, PRE, POST)				KFLOATINITDSS_NA(DST,DST,DST,COUNT,PRE,POST)
#define KFLOATINITDS_NA(DST, SRC0, COUNT, PRE, POST)			KFLOATINITDSS_NA(DST,SRC0,SRC0,COUNT,PRE,POST)
#define KFLOATINITDSS_NA(DST, SRC0, SRC1, COUNT, PRE, POST)\
	__asm	mov		eax,COUNT							\
	__asm	mov		PRE,0								\
	__asm	and		eax,8-1								\
	__asm	mov		ebx,COUNT							\
	__asm	mov		POST,eax							\
	__asm	and		ebx,0xfffffff8						\
	__asm	je		done								\
	__asm	shl		ebx,2								\
	__asm	mov		edx,SRC0							\
	__asm	mov		esi,SRC1							\
	__asm	mov		edi,DST								\
	__asm	add		edx,ebx								\
	__asm	add		esi,ebx								\
	__asm	add		edi,ebx								\
	__asm	mov		eax,edx								\
	__asm	or		eax,esi								\
	__asm	or		eax,edi								\
	__asm	neg		ebx									\

/*
	when OPER is called:
	edx = s0
	esi	= s1
	edi	= d
	ebx	= index*4

	xmm0 & xmm1	must not be trashed
*/
#define KMOVDS1(DST, SRC0)							\
	__asm	movss	xmm2,SRC0							\
	__asm	movss	DST,xmm2
#define KMOVDS4(DST, SRC0)							\
	__asm	movups	xmm2,SRC0							\
	__asm	movups	DST,xmm2
#define KMINDS1(DST, SRC0)							\
	__asm	movss	xmm2,SRC0							\
	__asm	minss	DST,xmm2
#define KMAXDS1(DST, SRC0)							\
	__asm	movss	xmm2,SRC0							\
	__asm	maxss	DST,xmm2

// general ALU operation
#define KALUDSS1(OP, DST, SRC0, SRC1)					\
	__asm	movss	xmm2,SRC0							\
	__asm	OP##ss	xmm2,SRC1							\
	__asm	movss	DST,xmm2
#define KALUDSS4(OP, DST, SRC0, SRC1)					\
	__asm	movups	xmm2,SRC0							\
	__asm	movups	xmm3,SRC1							\
	__asm	OP##ps	xmm2,xmm3							\
	__asm	movups	DST,xmm2

#define KADDDSS1(DST, SRC0, SRC1)		KALUDSS1(add, DST,SRC0,SRC1)
#define KADDDSS4(DST, SRC0, SRC1)		KALUDSS4(add, DST,SRC0,SRC1)
#define KSUBDSS1(DST, SRC0, SRC1)		KALUDSS1(sub, DST,SRC0,SRC1)
#define KSUBDSS4(DST, SRC0, SRC1)		KALUDSS4(sub, DST,SRC0,SRC1)
#define KMULDSS1(DST, SRC0, SRC1)		KALUDSS1(mul, DST,SRC0,SRC1)
#define KMULDSS4(DST, SRC0, SRC1)		KALUDSS4(mul, DST,SRC0,SRC1)

#define KDIVDSS1(DST, SRC0, SRC1)						\
	__asm	movss	xmm2,SRC1							\
	__asm	rcpss	xmm3,xmm2							\
	__asm	mulss	xmm2,xmm3							\
	__asm	mulss	xmm2,xmm3							\
	__asm	addss	xmm3,xmm3							\
	__asm	subss	xmm3,xmm2							\
	__asm	mulss	xmm3,SRC0							\
	__asm	movss	DST,xmm3
#define KDIVDSS4(DST, SRC0, SRC1)						\
	__asm	movups	xmm2,SRC1							\
	__asm	rcpps	xmm3,xmm2							\
	__asm	mulps	xmm2,xmm3							\
	__asm	mulps	xmm2,xmm3							\
	__asm	addps	xmm3,xmm3							\
	__asm	subps	xmm3,xmm2							\
	__asm	movups	xmm2,SRC0							\
	__asm	mulps	xmm3,xmm2							\
	__asm	movups	DST,xmm3
#define	KF2IDS1(SRC0)									\
	__asm	movss		xmm2,SRC0						\
	__asm	cvttps2pi	mm2,xmm2						\
	__asm	movd		[edi+ebx],mm2
#define	KF2IDS4(SRC0)									\
	__asm	movups		xmm2,SRC0						\
	__asm	cvttps2pi	mm2,xmm2						\
	__asm	movq		[edi+ebx+0],mm2					\
	__asm	shufps		xmm2,xmm2,SHUFFLEPS(1,0,3,2)	\
	__asm	cvttps2pi	mm2,xmm2						\
	__asm	movq		[edi+ebx+8],mm2
#define	KISQRTDS1(DST,SRC0)							\
	__asm	movss	xmm2,SRC0							\
	__asm	rsqrtss	xmm3,xmm2							\
	__asm	mulss	xmm2,xmm3							\
	__asm	mulss	xmm2,xmm3							\
	__asm	subss	xmm2,xmm1							\
	__asm	mulss	xmm3,xmm0							\
	__asm	mulss	xmm3,xmm2							\
	__asm	movss	DST,xmm3
#define	KISQRTDS4(DST,SRC0)							\
	__asm	movups	xmm2,SRC0							\
	__asm	rsqrtps	xmm3,xmm2							\
	__asm	mulps	xmm2,xmm3							\
	__asm	mulps	xmm2,xmm3							\
	__asm	subps	xmm2,xmm1							\
	__asm	mulps	xmm3,xmm0							\
	__asm	mulps	xmm3,xmm2							\
	__asm	movups	DST,xmm3

// this is used in vector4 implementation to shift constant V4
#define KANDREGDSV(DST, SRC0, VALUE)					\
	__asm	mov		DST,SRC0							\
	__asm	and		DST,VALUE

// this is used in vector4 code to operate with float arrays as sources
#define KEXPANDFLOAT(DST, SRC)						\
	__asm	movss	DST,SRC								\
	__asm	shufps  DST,DST,0

#define	KADDDS1(DST,SRC)		KADDDSS1(DST,DST,SRC)
#define	KADDDS4(DST,SRC)		KADDDSS4(DST,DST,SRC)
#define	KSUBDS1(DST,SRC)		KSUBDSS1(DST,DST,SRC)
#define	KSUBDS4(DST,SRC)		KSUBDSS4(DST,DST,SRC)
#define	KMULDS1(DST,SRC)		KMULDSS1(DST,DST,SRC)
#define	KMULDS4(DST,SRC)		KMULDSS4(DST,DST,SRC)
#define	KDIVDS1(DST,SRC)		KDIVDSS1(DST,DST,SRC)
#define	KDIVDS4(DST,SRC)		KDIVDSS4(DST,DST,SRC)

// handles pre & post leftovers
#define	KFLOATOPER(OPER, OPER4, COUNT)				\
	__asm		mov		ecx,pre							\
	__asm		mov		ebx,COUNT						\
	__asm		cmp		ebx,ecx							\
	__asm		cmovl	ecx,COUNT						\
	__asm		test	ecx,ecx							\
	__asm		je		preDone							\
	__asm		xor		ebx,ebx							\
	__asm	lpPre:										\
				OPER									\
	__asm		add		ebx,4							\
	__asm		dec		ecx								\
	__asm		jg		lpPre							\
	__asm	preDone:									\
	__asm		mov		ecx,post						\
	__asm		mov		ebx,COUNT						\
	__asm		sub		ebx,ecx							\
	__asm		shl		ebx,2							\
	__asm		cmp		ecx,4							\
	__asm		jl		post4Done						\
				OPER4									\
	__asm		sub		ecx,4							\
	__asm		add		ebx,4*4							\
	__asm	post4Done:									\
	__asm		test	ecx,ecx							\
	__asm		je		postDone						\
	__asm	lpPost:										\
				OPER									\
	__asm		add		ebx,4							\
	__asm		dec		ecx								\
	__asm		jg		lpPost							\
	__asm	postDone:

// operate on a constant and a float array
#define KFLOAT_CA(ALUOP, DST, SRC, CONSTANT, COUNT)	\
	int	pre,post;										\
	__asm		movss	xmm0,CONSTANT					\
	__asm		shufps	xmm0,xmm0,0						\
			KFLOATINITDS(DST, SRC, COUNT, pre, post)	\
	__asm		and		eax,15							\
	__asm		jne		lpNA							\
	__asm		jmp		lpA								\
	__asm		align	16								\
	__asm	lpA:										\
	__asm		prefetchnta	[edx+ebx+64]				\
	__asm		movaps	xmm1,xmm0						\
	__asm		movaps	xmm2,xmm0						\
	__asm		ALUOP##ps	xmm1,[edx+ebx]				\
	__asm		ALUOP##ps	xmm2,[edx+ebx+16]			\
	__asm		movaps	[edi+ebx],xmm1					\
	__asm		movaps	[edi+ebx+16],xmm2				\
	__asm		add		ebx,16*2						\
	__asm		jl		lpA								\
	__asm		jmp		done							\
	__asm		align	16								\
	__asm	lpNA:										\
	__asm		prefetchnta	[edx+ebx+64]				\
	__asm		movaps	xmm1,xmm0						\
	__asm		movaps	xmm2,xmm0						\
	__asm		movups	xmm3,[edx+ebx]					\
	__asm		movups	xmm4,[edx+ebx+16]				\
	__asm		ALUOP##ps	xmm1,xmm3					\
	__asm		ALUOP##ps	xmm2,xmm4					\
	__asm		movaps	[edi+ebx],xmm1					\
	__asm		movaps	[edi+ebx+16],xmm2				\
	__asm		add		ebx,16*2						\
	__asm		jl		lpNA							\
	__asm	done:										\
	__asm		mov		edx,SRC							\
	__asm		mov		edi,DST							\
	__asm		KFLOATOPER(KALUDSS1(ALUOP, [edi+ebx],xmm0,[edx+ebx]),	\
	__asm					KALUDSS4(ALUOP, [edi+ebx],xmm0,[edx+ebx]), COUNT)

// operate on two float arrays
#define KFLOAT_AA(ALUOP, DST, SRC0, SRC1, COUNT)		\
	int	pre,post;										\
	KFLOATINITDSS(DST, SRC0, SRC1, COUNT, pre, post)	\
	__asm		and		eax,15							\
	__asm		jne		lpNA							\
	__asm		jmp		lpA								\
	__asm		align	16								\
	__asm	lpA:										\
	__asm		movaps	xmm1,[edx+ebx]					\
	__asm		movaps	xmm2,[edx+ebx+16]				\
	__asm		ALUOP##ps	xmm1,[esi+ebx]				\
	__asm		ALUOP##ps	xmm2,[esi+ebx+16]			\
	__asm		prefetchnta	[edx+ebx+64]				\
	__asm		prefetchnta	[esi+ebx+64]				\
	__asm		movaps	[edi+ebx],xmm1					\
	__asm		movaps	[edi+ebx+16],xmm2				\
	__asm		add		ebx,16*2						\
	__asm		jl		lpA								\
	__asm		jmp		done							\
	__asm		align	16								\
	__asm	lpNA:										\
	__asm		movups	xmm1,[edx+ebx]					\
	__asm		movups	xmm2,[edx+ebx+16]				\
	__asm		movups	xmm3,[esi+ebx]					\
	__asm		movups	xmm4,[esi+ebx+16]				\
	__asm		prefetchnta	[edx+ebx+64]				\
	__asm		prefetchnta	[esi+ebx+64]				\
	__asm		ALUOP##ps	xmm1,xmm3					\
	__asm		ALUOP##ps	xmm2,xmm4					\
	__asm		movaps	[edi+ebx],xmm1					\
	__asm		movaps	[edi+ebx+16],xmm2				\
	__asm		add		ebx,16*2						\
	__asm		jl		lpNA							\
	__asm	done:										\
	__asm		mov		edx,SRC0						\
	__asm		mov		esi,SRC1						\
	__asm		mov		edi,DST							\
	KFLOATOPER(KALUDSS1(ALUOP, [edi+ebx],[edx+ebx],[esi+ebx]),		\
				KALUDSS4(ALUOP, [edi+ebx],[edx+ebx],[esi+ebx]), COUNT)


#define DRAWVERT_SIZE				60
#define DRAWVERT_XYZ_OFFSET			(0*4)
#define DRAWVERT_ST_OFFSET			(3*4)
#define DRAWVERT_NORMAL_OFFSET		(5*4)
#define DRAWVERT_TANGENT0_OFFSET	(8*4)
#define DRAWVERT_TANGENT1_OFFSET	(11*4)
#define DRAWVERT_COLOR_OFFSET		(14*4)

#define JOINTQUAT_SIZE				(7*4)
#define JOINTMAT_SIZE				(4*3*4)
#define JOINTWEIGHT_SIZE			(4*4)


#define ALIGN4_INIT1(X, INIT)				ALIGN16(static X[4]) = { INIT, INIT, INIT, INIT }
#define ALIGN4_INIT4(X, I0, I1, I2, I3)	ALIGN16(static X[4]) = { I0, I1, I2, I3 }
#define ALIGN8_INIT1(X, INIT)				ALIGN16(static X[8]) = { INIT, INIT, INIT, INIT, INIT, INIT, INIT, INIT }

ALIGN8_INIT1(unsigned short SIMD_W_zero, 0);
ALIGN8_INIT1(unsigned short SIMD_W_maxShort, 1<<15);

ALIGN4_INIT1(unsigned long SIMD_DW_mat2quatShuffle0, (3<<0)|(2<<8)|(1<<16)|(0<<24));
ALIGN4_INIT1(unsigned long SIMD_DW_mat2quatShuffle1, (0<<0)|(1<<8)|(2<<16)|(3<<24));
ALIGN4_INIT1(unsigned long SIMD_DW_mat2quatShuffle2, (1<<0)|(0<<8)|(3<<16)|(2<<24));
ALIGN4_INIT1(unsigned long SIMD_DW_mat2quatShuffle3, (2<<0)|(3<<8)|(0<<16)|(1<<24));

ALIGN4_INIT4(unsigned long SIMD_SP_singleSignBitMask, (unsigned long) (1 << 31), 0, 0, 0);
ALIGN4_INIT1(unsigned long SIMD_SP_signBitMask, (unsigned long) (1 << 31));
ALIGN4_INIT1(unsigned long SIMD_SP_absMask, (unsigned long) ~(1 << 31));
ALIGN4_INIT1(unsigned long SIMD_SP_infinityMask, (unsigned long) ~(1 << 23));
ALIGN4_INIT1(unsigned long SIMD_SP_not, 0xFFFFFFFF);

ALIGN4_INIT1(float SIMD_SP_zero, 0.0f);
ALIGN4_INIT1(float SIMD_SP_half, 0.5f);
ALIGN4_INIT1(float SIMD_SP_one, 1.0f);
ALIGN4_INIT1(float SIMD_SP_two, 2.0f);
ALIGN4_INIT1(float SIMD_SP_three, 3.0f);
ALIGN4_INIT1(float SIMD_SP_four, 4.0f);
ALIGN4_INIT1(float SIMD_SP_maxShort, (1<<15));
ALIGN4_INIT1(float SIMD_SP_tiny, 1e-10f);
ALIGN4_INIT1(float SIMD_SP_PI, xMath::PI);
ALIGN4_INIT1(float SIMD_SP_halfPI, xMath::HALF_PI);
ALIGN4_INIT1(float SIMD_SP_twoPI, xMath::TWO_PI);
ALIGN4_INIT1(float SIMD_SP_oneOverTwoPI, 1.0f / xMath::TWO_PI);
ALIGN4_INIT1(float SIMD_SP_infinity, xMath::INFINITY);
ALIGN4_INIT4(float SIMD_SP_lastOne, 0.0f, 0.0f, 0.0f, 1.0f);

ALIGN4_INIT1(float SIMD_SP_rsqrt_c0,  3.0f);
ALIGN4_INIT1(float SIMD_SP_rsqrt_c1, -0.5f);
ALIGN4_INIT1(float SIMD_SP_mat2quat_rsqrt_c1, -0.5f*0.5f);

ALIGN4_INIT1(float SIMD_SP_sin_c0, -2.39e-08f);
ALIGN4_INIT1(float SIMD_SP_sin_c1,  2.7526e-06f);
ALIGN4_INIT1(float SIMD_SP_sin_c2, -1.98409e-04f);
ALIGN4_INIT1(float SIMD_SP_sin_c3,  8.3333315e-03f);
ALIGN4_INIT1(float SIMD_SP_sin_c4, -1.666666664e-01f);

ALIGN4_INIT1(float SIMD_SP_cos_c0, -2.605e-07f);
ALIGN4_INIT1(float SIMD_SP_cos_c1,  2.47609e-05f);
ALIGN4_INIT1(float SIMD_SP_cos_c2, -1.3888397e-03f);
ALIGN4_INIT1(float SIMD_SP_cos_c3,  4.16666418e-02f);
ALIGN4_INIT1(float SIMD_SP_cos_c4, -4.999999963e-01f);

ALIGN4_INIT1(float SIMD_SP_atan_c0,  0.0028662257f);
ALIGN4_INIT1(float SIMD_SP_atan_c1, -0.0161657367f);
ALIGN4_INIT1(float SIMD_SP_atan_c2,  0.0429096138f);
ALIGN4_INIT1(float SIMD_SP_atan_c3, -0.0752896400f);
ALIGN4_INIT1(float SIMD_SP_atan_c4,  0.1065626393f);
ALIGN4_INIT1(float SIMD_SP_atan_c5, -0.1420889944f);
ALIGN4_INIT1(float SIMD_SP_atan_c6,  0.1999355085f);
ALIGN4_INIT1(float SIMD_SP_atan_c7, -0.3333314528f);

/*
============
SSE_InvSqrt
============
*/
float SSE_InvSqrt(float x) {
	float y;

	__asm {
		movss		xmm0, x
		rsqrtss		xmm1, xmm0
		mulss		xmm0, xmm1
		mulss		xmm0, xmm1
		subss		xmm0, SIMD_SP_rsqrt_c0
		mulss		xmm1, SIMD_SP_rsqrt_c1
		mulss		xmm0, xmm1
		movss		y, xmm0
	}
	return y;
}

/*
============
SSE_InvSqrt4
============
*/
void SSE_InvSqrt4(float x[4]) {
	__asm {
		mov			edi, x
		movaps		xmm0, [edi]
		rsqrtps		xmm1, xmm0
		mulps		xmm0, xmm1
		mulps		xmm0, xmm1
		subps		xmm0, SIMD_SP_rsqrt_c0
		mulps		xmm1, SIMD_SP_rsqrt_c1
		mulps		xmm0, xmm1
		movaps		[edi], xmm0
	}
}

/*
============
SSE_SinZeroHalfPI

  The angle must be between zero and half PI.
============
*/
float SSE_SinZeroHalfPI(float a) {
#if 1

	float t;

	assert(a >= 0.0f && a <= xMath::HALF_PI);

	__asm {
		movss		xmm0, a
		movss		xmm1, xmm0
		mulss		xmm1, xmm1
		movss		xmm2, SIMD_SP_sin_c0
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_sin_c1
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_sin_c2
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_sin_c3
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_sin_c4
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_one
		mulss		xmm2, xmm0
		movss		t, xmm2
	}

	return t;

#else

	float s, t;

	assert(a >= 0.0f && a <= xMath::HALF_PI);

	s = a * a;
	t = -2.39e-08f;
	t *= s;
	t += 2.7526e-06f;
	t *= s;
	t += -1.98409e-04f;
	t *= s;
	t += 8.3333315e-03f;
	t *= s;
	t += -1.666666664e-01f;
	t *= s;
	t += 1.0f;
	t *= a;

	return t;

#endif
}

/*
============
SSE_Sin4ZeroHalfPI

  The angle must be between zero and half PI.
============
*/
void SSE_Sin4ZeroHalfPI(float a[4], float s[4]) {
	__asm {
		mov			edi, a
		mov			esi, s
		movaps		xmm0, [edi]
		movaps		xmm1, xmm0
		mulps		xmm1, xmm1
		movaps		xmm2, SIMD_SP_sin_c0
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_sin_c1
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_sin_c2
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_sin_c3
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_sin_c4
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_one
		mulps		xmm2, xmm0
		movaps		[esi], xmm2
	}
}

/*
============
SSE_Sin
============
*/
float SSE_Sin(float a) {
#if 1

	float t;

	__asm {
		movss		xmm1, a
		movss		xmm2, xmm1
		movss		xmm3, xmm1
		mulss		xmm2, SIMD_SP_oneOverTwoPI
		cvttss2si	ecx, xmm2
		cmpltss		xmm3, SIMD_SP_zero
		andps		xmm3, SIMD_SP_one
		cvtsi2ss	xmm2, ecx
		subss		xmm2, xmm3
		mulss		xmm2, SIMD_SP_twoPI
		subss		xmm1, xmm2

		movss		xmm0, SIMD_SP_PI			// xmm0 = PI
		subss		xmm0, xmm1					// xmm0 = PI - a
		movss		xmm1, xmm0					// xmm1 = PI - a
		andps		xmm1, SIMD_SP_signBitMask	// xmm1 = signbit(PI - a)
		movss		xmm2, xmm0					// xmm2 = PI - a
		xorps		xmm2, xmm1					// xmm2 = fabs(PI - a)
		cmpnltss	xmm2, SIMD_SP_halfPI		// xmm2 = (fabs(PI - a) >= xMath::HALF_PI) ? 0xFFFFFFFF : 0x00000000
		movss		xmm3, SIMD_SP_PI			// xmm3 = PI
		xorps		xmm3, xmm1					// xmm3 = PI ^ signbit(PI - a)
		andps		xmm3, xmm2					// xmm3 = (fabs(PI - a) >= xMath::HALF_PI) ? (PI ^ signbit(PI - a)) : 0.0f
		andps		xmm2, SIMD_SP_signBitMask	// xmm2 = (fabs(PI - a) >= xMath::HALF_PI) ? SIMD_SP_signBitMask : 0.0f
		xorps		xmm0, xmm2
		addps		xmm0, xmm3

		movss		xmm1, xmm0
		mulss		xmm1, xmm1
		movss		xmm2, SIMD_SP_sin_c0
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_sin_c1
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_sin_c2
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_sin_c3
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_sin_c4
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_one
		mulss		xmm2, xmm0
		movss		t, xmm2
	}

	return t;

#else

	float s, t;

	if ((a < 0.0f) || (a >= xMath::TWO_PI)) {
		a -= floorf(a / xMath::TWO_PI) * xMath::TWO_PI;
	}

	a = xMath::PI - a;
	if (fabs(a) >= xMath::HALF_PI) {
		a = ((a < 0.0f) ? -xMath::PI : xMath::PI) - a;
	}

	s = a * a;
	t = -2.39e-08f;
	t *= s;
	t += 2.7526e-06f;
	t *= s;
	t += -1.98409e-04f;
	t *= s;
	t += 8.3333315e-03f;
	t *= s;
	t += -1.666666664e-01f;
	t *= s;
	t += 1.0f;
	t *= a;

	return t;

#endif
}

/*
============
SSE_Sin4
============
*/
void SSE_Sin4(float a[4], float s[4]) {
	__asm {
		mov			edi, a
		mov			esi, s
		movaps		xmm1, [edi]
		movaps		xmm2, xmm1
		mulps		xmm2, SIMD_SP_oneOverTwoPI
		movhlps		xmm3, xmm2
		cvttss2si	ecx, xmm2
		cvtsi2ss	xmm2, ecx
		cvttss2si	edx, xmm3
		cvtsi2ss	xmm3, edx
		shufps		xmm2, xmm2, R_SHUFFLEPS(1, 0, 0, 0)
		shufps		xmm3, xmm3, R_SHUFFLEPS(1, 0, 0, 0)
		cvttss2si	ecx, xmm2
		cvtsi2ss	xmm2, ecx
		cvttss2si	edx, xmm3
		cvtsi2ss	xmm3, edx
		shufps		xmm2, xmm3, R_SHUFFLEPS(1, 0, 1, 0)
		movaps		xmm3, xmm1
		cmpltps		xmm3, SIMD_SP_zero
		andps		xmm3, SIMD_SP_one
		subps		xmm2, xmm3
		mulps		xmm2, SIMD_SP_twoPI
		subps		xmm1, xmm2

		movaps		xmm0, SIMD_SP_PI			// xmm0 = PI
		subps		xmm0, xmm1					// xmm0 = PI - a
		movaps		xmm1, xmm0					// xmm1 = PI - a
		andps		xmm1, SIMD_SP_signBitMask	// xmm1 = signbit(PI - a)
		movaps		xmm2, xmm0					// xmm2 = PI - a
		xorps		xmm2, xmm1					// xmm2 = fabs(PI - a)
		cmpnltps	xmm2, SIMD_SP_halfPI		// xmm2 = (fabs(PI - a) >= xMath::HALF_PI) ? 0xFFFFFFFF : 0x00000000
		movaps		xmm3, SIMD_SP_PI			// xmm3 = PI
		xorps		xmm3, xmm1					// xmm3 = PI ^ signbit(PI - a)
		andps		xmm3, xmm2					// xmm3 = (fabs(PI - a) >= xMath::HALF_PI) ? (PI ^ signbit(PI - a)) : 0.0f
		andps		xmm2, SIMD_SP_signBitMask	// xmm2 = (fabs(PI - a) >= xMath::HALF_PI) ? SIMD_SP_signBitMask : 0.0f
		xorps		xmm0, xmm2
		addps		xmm0, xmm3

		movaps		xmm1, xmm0
		mulps		xmm1, xmm1
		movaps		xmm2, SIMD_SP_sin_c0
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_sin_c1
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_sin_c2
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_sin_c3
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_sin_c4
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_one
		mulps		xmm2, xmm0
		movaps		[esi], xmm2
	}
}

/*
============
SSE_CosZeroHalfPI

  The angle must be between zero and half PI.
============
*/
float SSE_CosZeroHalfPI(float a) {
#if 1

	float t;

	assert(a >= 0.0f && a <= xMath::HALF_PI);

	__asm {
		movss		xmm0, a
		mulss		xmm0, xmm0
		movss		xmm1, SIMD_SP_cos_c0
		mulss		xmm1, xmm0
		addss		xmm1, SIMD_SP_cos_c1
		mulss		xmm1, xmm0
		addss		xmm1, SIMD_SP_cos_c2
		mulss		xmm1, xmm0
		addss		xmm1, SIMD_SP_cos_c3
		mulss		xmm1, xmm0
		addss		xmm1, SIMD_SP_cos_c4
		mulss		xmm1, xmm0
		addss		xmm1, SIMD_SP_one
		movss		t, xmm1
	}

	return t;

#else

	float s, t;

	assert(a >= 0.0f && a <= xMath::HALF_PI);

	s = a * a;
	t = -2.605e-07f;
	t *= s;
	t += 2.47609e-05f;
	t *= s;
	t += -1.3888397e-03f;
	t *= s;
	t += 4.16666418e-02f;
	t *= s;
	t += -4.999999963e-01f;
	t *= s;
	t += 1.0f;

	return t;

#endif
}

/*
============
SSE_Cos4ZeroHalfPI

  The angle must be between zero and half PI.
============
*/
void SSE_Cos4ZeroHalfPI(float a[4], float c[4]) {
	__asm {
		mov			edi, a
		mov			esi, c
		movaps		xmm0, [edi]
		mulps		xmm0, xmm0
		movaps		xmm1, SIMD_SP_cos_c0
		mulps		xmm1, xmm0
		addps		xmm1, SIMD_SP_cos_c1
		mulps		xmm1, xmm0
		addps		xmm1, SIMD_SP_cos_c2
		mulps		xmm1, xmm0
		addps		xmm1, SIMD_SP_cos_c3
		mulps		xmm1, xmm0
		addps		xmm1, SIMD_SP_cos_c4
		mulps		xmm1, xmm0
		addps		xmm1, SIMD_SP_one
		movaps		[esi], xmm2
	}
}

/*
============
SSE_Cos
============
*/
float SSE_Cos(float a) {
#if 1

	float t;

	__asm {
		movss		xmm1, a
		movss		xmm2, xmm1
		movss		xmm3, xmm1
		mulss		xmm2, SIMD_SP_oneOverTwoPI
		cvttss2si	ecx, xmm2
		cmpltss		xmm3, SIMD_SP_zero
		andps		xmm3, SIMD_SP_one
		cvtsi2ss	xmm2, ecx
		subss		xmm2, xmm3
		mulss		xmm2, SIMD_SP_twoPI
		subss		xmm1, xmm2

		movss		xmm0, SIMD_SP_PI			// xmm0 = PI
		subss		xmm0, xmm1					// xmm0 = PI - a
		movss		xmm1, xmm0					// xmm1 = PI - a
		andps		xmm1, SIMD_SP_signBitMask	// xmm1 = signbit(PI - a)
		movss		xmm2, xmm0					// xmm2 = PI - a
		xorps		xmm2, xmm1					// xmm2 = fabs(PI - a)
		cmpnltss	xmm2, SIMD_SP_halfPI		// xmm2 = (fabs(PI - a) >= xMath::HALF_PI) ? 0xFFFFFFFF : 0x00000000
		movss		xmm3, SIMD_SP_PI			// xmm3 = PI
		xorps		xmm3, xmm1					// xmm3 = PI ^ signbit(PI - a)
		andps		xmm3, xmm2					// xmm3 = (fabs(PI - a) >= xMath::HALF_PI) ? (PI ^ signbit(PI - a)) : 0.0f
		andps		xmm2, SIMD_SP_signBitMask	// xmm2 = (fabs(PI - a) >= xMath::HALF_PI) ? SIMD_SP_signBitMask : 0.0f
		xorps		xmm0, xmm2
		addps		xmm0, xmm3

		mulss		xmm0, xmm0
		movss		xmm1, SIMD_SP_cos_c0
		mulss		xmm1, xmm0
		addss		xmm1, SIMD_SP_cos_c1
		mulss		xmm1, xmm0
		addss		xmm1, SIMD_SP_cos_c2
		mulss		xmm1, xmm0
		addss		xmm1, SIMD_SP_cos_c3
		mulss		xmm1, xmm0
		addss		xmm1, SIMD_SP_cos_c4
		mulss		xmm1, xmm0
		addss		xmm1, SIMD_SP_one
		xorps		xmm2, SIMD_SP_signBitMask
		xorps		xmm1, xmm2
		movss		t, xmm1
	}

	return t;

#else

	float s, t;

	if ((a < 0.0f) || (a >= xMath::TWO_PI)) {
		a -= floorf(a / xMath::TWO_PI) * xMath::TWO_PI;
	}

	a = xMath::PI - a;
	if (fabs(a) >= xMath::HALF_PI) {
		a = ((a < 0.0f) ? -xMath::PI : xMath::PI) - a;
		d = 1.0f;
	} else {
		d = -1.0f;
	}

	s = a * a;
	t = -2.605e-07f;
	t *= s;
	t += 2.47609e-05f;
	t *= s;
	t += -1.3888397e-03f;
	t *= s;
	t += 4.16666418e-02f;
	t *= s;
	t += -4.999999963e-01f;
	t *= s;
	t += 1.0f;
	t *= d;

	return t;

#endif
}

/*
============
SSE_Cos4
============
*/
void SSE_Cos4(float a[4], float c[4]) {
	__asm {
		mov			edi, a
		mov			esi, c
		movaps		xmm1, [edi]
		movaps		xmm2, xmm1
		mulps		xmm2, SIMD_SP_oneOverTwoPI
		movhlps		xmm3, xmm2
		cvttss2si	ecx, xmm2
		cvtsi2ss	xmm2, ecx
		cvttss2si	edx, xmm3
		cvtsi2ss	xmm3, edx
		shufps		xmm2, xmm2, R_SHUFFLEPS(1, 0, 0, 0)
		shufps		xmm3, xmm3, R_SHUFFLEPS(1, 0, 0, 0)
		cvttss2si	ecx, xmm2
		cvtsi2ss	xmm2, ecx
		cvttss2si	edx, xmm3
		cvtsi2ss	xmm3, edx
		shufps		xmm2, xmm3, R_SHUFFLEPS(1, 0, 1, 0)
		movaps		xmm3, xmm1
		cmpltps		xmm3, SIMD_SP_zero
		andps		xmm3, SIMD_SP_one
		subps		xmm2, xmm3
		mulps		xmm2, SIMD_SP_twoPI
		subps		xmm1, xmm2

		movaps		xmm0, SIMD_SP_PI			// xmm0 = PI
		subps		xmm0, xmm1					// xmm0 = PI - a
		movaps		xmm1, xmm0					// xmm1 = PI - a
		andps		xmm1, SIMD_SP_signBitMask	// xmm1 = signbit(PI - a)
		movaps		xmm2, xmm0					// xmm2 = PI - a
		xorps		xmm2, xmm1					// xmm2 = fabs(PI - a)
		cmpnltps	xmm2, SIMD_SP_halfPI		// xmm2 = (fabs(PI - a) >= xMath::HALF_PI) ? 0xFFFFFFFF : 0x00000000
		movaps		xmm3, SIMD_SP_PI			// xmm3 = PI
		xorps		xmm3, xmm1					// xmm3 = PI ^ signbit(PI - a)
		andps		xmm3, xmm2					// xmm3 = (fabs(PI - a) >= xMath::HALF_PI) ? (PI ^ signbit(PI - a)) : 0.0f
		andps		xmm2, SIMD_SP_signBitMask	// xmm2 = (fabs(PI - a) >= xMath::HALF_PI) ? SIMD_SP_signBitMask : 0.0f
		xorps		xmm0, xmm2
		addps		xmm0, xmm3

		mulps		xmm0, xmm0
		movaps		xmm1, SIMD_SP_cos_c0
		mulps		xmm1, xmm0
		addps		xmm1, SIMD_SP_cos_c1
		mulps		xmm1, xmm0
		addps		xmm1, SIMD_SP_cos_c2
		mulps		xmm1, xmm0
		addps		xmm1, SIMD_SP_cos_c3
		mulps		xmm1, xmm0
		addps		xmm1, SIMD_SP_cos_c4
		mulps		xmm1, xmm0
		addps		xmm1, SIMD_SP_one
		xorps		xmm2, SIMD_SP_signBitMask
		xorps		xmm1, xmm2
		movaps		[esi], xmm1
	}
}

/*
============
SSE_SinCos
============
*/
void SSE_SinCos(float a, float &s, float &c) {
	__asm {
		mov			edi, s
		mov			esi, c
		movss		xmm1, a
		movss		xmm2, xmm1
		movss		xmm3, xmm1
		mulss		xmm2, SIMD_SP_oneOverTwoPI
		cvttss2si	ecx, xmm2
		cmpltss		xmm3, SIMD_SP_zero
		andps		xmm3, SIMD_SP_one
		cvtsi2ss	xmm2, ecx
		subss		xmm2, xmm3
		mulss		xmm2, SIMD_SP_twoPI
		subss		xmm1, xmm2

		movss		xmm0, SIMD_SP_PI			// xmm0 = PI
		subss		xmm0, xmm1					// xmm0 = PI - a
		movss		xmm1, xmm0					// xmm1 = PI - a
		andps		xmm1, SIMD_SP_signBitMask	// xmm1 = signbit(PI - a)
		movss		xmm2, xmm0					// xmm2 = PI - a
		xorps		xmm2, xmm1					// xmm2 = fabs(PI - a)
		cmpnltss	xmm2, SIMD_SP_halfPI		// xmm2 = (fabs(PI - a) >= xMath::HALF_PI) ? 0xFFFFFFFF : 0x00000000
		movss		xmm3, SIMD_SP_PI			// xmm3 = PI
		xorps		xmm3, xmm1					// xmm3 = PI ^ signbit(PI - a)
		andps		xmm3, xmm2					// xmm3 = (fabs(PI - a) >= xMath::HALF_PI) ? (PI ^ signbit(PI - a)) : 0.0f
		andps		xmm2, SIMD_SP_signBitMask	// xmm2 = (fabs(PI - a) >= xMath::HALF_PI) ? SIMD_SP_signBitMask : 0.0f
		xorps		xmm0, xmm2
		addps		xmm0, xmm3

		movss		xmm1, xmm0
		mulss		xmm1, xmm1
		movss		xmm3, SIMD_SP_sin_c0
		movss		xmm4, SIMD_SP_cos_c0
		mulss		xmm3, xmm1
		mulss		xmm4, xmm1
		addss		xmm3, SIMD_SP_sin_c1
		addss		xmm4, SIMD_SP_cos_c1
		mulss		xmm3, xmm1
		mulss		xmm4, xmm1
		addss		xmm3, SIMD_SP_sin_c2
		addss		xmm4, SIMD_SP_cos_c2
		mulss		xmm3, xmm1
		mulss		xmm4, xmm1
		addss		xmm3, SIMD_SP_sin_c3
		addss		xmm4, SIMD_SP_cos_c3
		mulss		xmm3, xmm1
		mulss		xmm4, xmm1
		addss		xmm3, SIMD_SP_sin_c4
		addss		xmm4, SIMD_SP_cos_c4
		mulss		xmm3, xmm1
		mulss		xmm4, xmm1
		addss		xmm3, SIMD_SP_one
		addss		xmm4, SIMD_SP_one
		mulss		xmm3, xmm0
		xorps		xmm2, SIMD_SP_signBitMask
		xorps		xmm4, xmm2
		movss		[edi], xmm2
		movss		[esi], xmm3
	}
}

/*
============
SSE_SinCos4
============
*/
void SSE_SinCos4(float a[4], float s[4], float c[4]) {
	__asm {
		mov			eax, a
		mov			edi, s
		mov			esi, c
		movaps		xmm1, [eax]
		movaps		xmm2, xmm1
		mulps		xmm2, SIMD_SP_oneOverTwoPI
		movhlps		xmm3, xmm2
		cvttss2si	ecx, xmm2
		cvtsi2ss	xmm2, ecx
		cvttss2si	edx, xmm3
		cvtsi2ss	xmm3, edx
		shufps		xmm2, xmm2, R_SHUFFLEPS(1, 0, 0, 0)
		shufps		xmm3, xmm3, R_SHUFFLEPS(1, 0, 0, 0)
		cvttss2si	ecx, xmm2
		cvtsi2ss	xmm2, ecx
		cvttss2si	edx, xmm3
		cvtsi2ss	xmm3, edx
		shufps		xmm2, xmm3, R_SHUFFLEPS(1, 0, 1, 0)
		movaps		xmm3, xmm1
		cmpltps		xmm3, SIMD_SP_zero
		andps		xmm3, SIMD_SP_one
		subps		xmm2, xmm3
		mulps		xmm2, SIMD_SP_twoPI
		subps		xmm1, xmm2

		movaps		xmm0, SIMD_SP_PI			// xmm0 = PI
		subps		xmm0, xmm1					// xmm0 = PI - a
		movaps		xmm1, xmm0					// xmm1 = PI - a
		andps		xmm1, SIMD_SP_signBitMask	// xmm1 = signbit(PI - a)
		movaps		xmm2, xmm0					// xmm2 = PI - a
		xorps		xmm2, xmm1					// xmm2 = fabs(PI - a)
		cmpnltps	xmm2, SIMD_SP_halfPI		// xmm2 = (fabs(PI - a) >= xMath::HALF_PI) ? 0xFFFFFFFF : 0x00000000
		movaps		xmm3, SIMD_SP_PI			// xmm3 = PI
		xorps		xmm3, xmm1					// xmm3 = PI ^ signbit(PI - a)
		andps		xmm3, xmm2					// xmm3 = (fabs(PI - a) >= xMath::HALF_PI) ? (PI ^ signbit(PI - a)) : 0.0f
		andps		xmm2, SIMD_SP_signBitMask	// xmm2 = (fabs(PI - a) >= xMath::HALF_PI) ? SIMD_SP_signBitMask : 0.0f
		xorps		xmm0, xmm2
		addps		xmm0, xmm3

		movaps		xmm0, [eax]
		movaps		xmm1, xmm0
		mulps		xmm1, xmm1
		movaps		xmm3, SIMD_SP_sin_c0
		movaps		xmm4, SIMD_SP_cos_c0
		mulps		xmm3, xmm1
		mulps		xmm4, xmm1
		addps		xmm3, SIMD_SP_sin_c1
		addps		xmm4, SIMD_SP_cos_c1
		mulps		xmm3, xmm1
		mulps		xmm4, xmm1
		addps		xmm3, SIMD_SP_sin_c2
		addps		xmm4, SIMD_SP_cos_c2
		mulps		xmm3, xmm1
		mulps		xmm4, xmm1
		addps		xmm3, SIMD_SP_sin_c3
		addps		xmm4, SIMD_SP_cos_c3
		mulps		xmm3, xmm1
		mulps		xmm4, xmm1
		addps		xmm3, SIMD_SP_sin_c4
		addps		xmm4, SIMD_SP_cos_c4
		mulps		xmm3, xmm1
		mulps		xmm4, xmm1
		addps		xmm3, SIMD_SP_one
		addps		xmm4, SIMD_SP_one
		mulps		xmm3, xmm0
		xorps		xmm2, SIMD_SP_signBitMask
		xorps		xmm4, xmm2
		movaps		[edi], xmm3
		movaps		[esi], xmm4
	}
}

/*
============
SSE_ATanPositive

  Both 'x' and 'y' must be positive.
============
*/
float SSE_ATanPositive(float y, float x) {
#if 1

	float t;

	assert(y >= 0.0f && x >= 0.0f);

	__asm {
		movss		xmm0, x
		movss		xmm3, xmm0
		movss		xmm1, y
		minss		xmm0, xmm1
		maxss		xmm1, xmm3
		cmpeqss		xmm3, xmm0
		rcpss		xmm2, xmm1
		mulss		xmm1, xmm2
		mulss		xmm1, xmm2
		addss		xmm2, xmm2
		subss		xmm2, xmm1				// xmm2 = 1 / y or 1 / x
		mulss		xmm0, xmm2				// xmm0 = x / y or y / x
		movss		xmm1, xmm3
		andps		xmm1, SIMD_SP_signBitMask
		xorps		xmm0, xmm1				// xmm0 = -x / y or y / x
		andps		xmm3, SIMD_SP_halfPI	// xmm3 = HALF_PI or 0.0f
		movss		xmm1, xmm0
		mulss		xmm1, xmm1				// xmm1 = s
		movss		xmm2, SIMD_SP_atan_c0
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_atan_c1
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_atan_c2
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_atan_c3
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_atan_c4
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_atan_c5
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_atan_c6
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_atan_c7
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_one
		mulss		xmm2, xmm0
		addss		xmm2, xmm3
		movss		t, xmm2
	}

	return t;

#else

	float a, d, s, t;

	assert(y >= 0.0f && x >= 0.0f);

	if (y > x) {
		a = -x / y;
		d = xMath::HALF_PI;
	} else {
		a = y / x;
		d = 0.0f;
	}
	s = a * a;
	t = 0.0028662257f;
	t *= s;
	t += -0.0161657367f;
	t *= s;
	t += 0.0429096138f;
	t *= s;
	t += -0.0752896400f;
	t *= s;
	t += 0.1065626393f;
	t *= s;
	t += -0.1420889944f;
	t *= s;
	t += 0.1999355085f;
	t *= s;
	t += -0.3333314528f;
	t *= s;
	t += 1.0f;
	t *= a;
	t += d;

	return t;

#endif
}

/*
============
SSE_ATan4Positive

  Both 'x' and 'y' must be positive.
============
*/
void SSE_ATan4Positive(float y[4], float x[4], float at[4]) {
	__asm {
		mov			esi, x
		mov			edi, y
		mov			edx, at
		movaps		xmm0, [esi]
		movaps		xmm3, xmm0
		movaps		xmm1, [edi]
		minps		xmm0, xmm1
		maxps		xmm1, xmm3
		cmpeqps		xmm3, xmm0
		rcpps		xmm2, xmm1
		mulps		xmm1, xmm2
		mulps		xmm1, xmm2
		addps		xmm2, xmm2
		subps		xmm2, xmm1				// xmm2 = 1 / y or 1 / x
		mulps		xmm0, xmm2				// xmm0 = x / y or y / x
		movaps		xmm1, xmm3
		andps		xmm1, SIMD_SP_signBitMask
		xorps		xmm0, xmm1				// xmm0 = -x / y or y / x
		andps		xmm3, SIMD_SP_halfPI	// xmm3 = HALF_PI or 0.0f
		movaps		xmm1, xmm0
		mulps		xmm1, xmm1				// xmm1 = s
		movaps		xmm2, SIMD_SP_atan_c0
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_atan_c1
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_atan_c2
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_atan_c3
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_atan_c4
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_atan_c5
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_atan_c6
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_atan_c7
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_one
		mulps		xmm2, xmm0
		addps		xmm2, xmm3
		movaps		[edx], xmm2
	}
}

/*
============
SSE_ATan
============
*/
float SSE_ATan(float y, float x) {
#if 1

	float t;

	__asm {
		movss		xmm0, x
		movss		xmm3, xmm0
		movss		xmm4, xmm0
		andps		xmm0, SIMD_SP_absMask
		movss		xmm1, y
		xorps		xmm4, xmm1
		andps		xmm1, SIMD_SP_absMask
		andps		xmm4, SIMD_SP_signBitMask
		minss		xmm0, xmm1
		maxss		xmm1, xmm3
		cmpeqss		xmm3, xmm0
		rcpss		xmm2, xmm1
		mulss		xmm1, xmm2
		mulss		xmm1, xmm2
		addss		xmm2, xmm2
		subss		xmm2, xmm1				// xmm2 = 1 / y or 1 / x
		mulss		xmm0, xmm2				// xmm0 = x / y or y / x
		xorps		xmm0, xmm4
		movss		xmm1, xmm3
		andps		xmm1, SIMD_SP_signBitMask
		xorps		xmm0, xmm1				// xmm0 = -x / y or y / x
		orps		xmm4, SIMD_SP_halfPI	// xmm4 = +/- HALF_PI
		andps		xmm3, xmm4				// xmm3 = +/- HALF_PI or 0.0f
		movss		xmm1, xmm0
		mulss		xmm1, xmm1				// xmm1 = s
		movss		xmm2, SIMD_SP_atan_c0
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_atan_c1
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_atan_c2
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_atan_c3
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_atan_c4
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_atan_c5
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_atan_c6
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_atan_c7
		mulss		xmm2, xmm1
		addss		xmm2, SIMD_SP_one
		mulss		xmm2, xmm0
		addss		xmm2, xmm3
		movss		t, xmm2
	}

	return t;

#else

	float a, d, s, t;

	if (fabs(y) > fabs(x)) {
		a = -x / y;
		d = xMath::HALF_PI;
		*((unsigned long *)&d) ^= (*((unsigned long *)&x) ^ *((unsigned long *)&y)) & (1<<31);
	} else {
		a = y / x;
		d = 0.0f;
	}

	s = a * a;
	t = 0.0028662257f;
	t *= s;
	t += -0.0161657367f;
	t *= s;
	t += 0.0429096138f;
	t *= s;
	t += -0.0752896400f;
	t *= s;
	t += 0.1065626393f;
	t *= s;
	t += -0.1420889944f;
	t *= s;
	t += 0.1999355085f;
	t *= s;
	t += -0.3333314528f;
	t *= s;
	t += 1.0f;
	t *= a;
	t += d;

	return t;

#endif
}

/*
============
SSE_ATan4
============
*/
void SSE_ATan4(float y[4], float x[4], float at[4]) {
	__asm {
		mov			esi, x
		mov			edi, y
		mov			edx, at
		movaps		xmm0, [esi]
		movaps		xmm3, xmm0
		movaps		xmm4, xmm0
		andps		xmm0, SIMD_SP_absMask
		movaps		xmm1, [edi]
		xorps		xmm4, xmm1
		andps		xmm1, SIMD_SP_absMask
		andps		xmm4, SIMD_SP_signBitMask
		minps		xmm0, xmm1
		maxps		xmm1, xmm3
		cmpeqps		xmm3, xmm0
		rcpps		xmm2, xmm1
		mulps		xmm1, xmm2
		mulps		xmm1, xmm2
		addps		xmm2, xmm2
		subps		xmm2, xmm1				// xmm2 = 1 / y or 1 / x
		mulps		xmm0, xmm2				// xmm0 = x / y or y / x
		xorps		xmm0, xmm4
		movaps		xmm1, xmm3
		andps		xmm1, SIMD_SP_signBitMask
		xorps		xmm0, xmm1				// xmm0 = -x / y or y / x
		orps		xmm4, SIMD_SP_halfPI	// xmm4 = +/- HALF_PI
		andps		xmm3, xmm4				// xmm3 = +/- HALF_PI or 0.0f
		movaps		xmm1, xmm0
		mulps		xmm1, xmm1				// xmm1 = s
		movaps		xmm2, SIMD_SP_atan_c0
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_atan_c1
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_atan_c2
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_atan_c3
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_atan_c4
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_atan_c5
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_atan_c6
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_atan_c7
		mulps		xmm2, xmm1
		addps		xmm2, SIMD_SP_one
		mulps		xmm2, xmm0
		addps		xmm2, xmm3
		movaps		[edx], xmm2
	}
}

/*
============
SSE_TestTrigonometry
============
*/
void SSE_TestTrigonometry() {
	int i;
	float a, s1, s2, c1, c2;

	for (i = 0; i < 100; i++) {
		a = i * xMath::HALF_PI / 100.0f;

		s1 = sin(a);
		s2 = SSE_SinZeroHalfPI(a);

		if (fabs(s1 - s2) > 1e-7f) {
			assert(0);
		}

		c1 = cos(a);
		c2 = SSE_CosZeroHalfPI(a);

		if (fabs(c1 - c2) > 1e-7f) {
			assert(0);
		}
	}

	for (i = -200; i < 200; i++) {
		a = i * xMath::TWO_PI / 100.0f;

		s1 = sin(a);
		s2 = SSE_Sin(a);

		if (fabs(s1 - s2) > 1e-6f) {
			assert(0);
		}

		c1 = cos(a);
		c2 = SSE_Cos(a);

		if (fabs(c1 - c2) > 1e-6f) {
			assert(0);
		}

		SSE_SinCos(a, s2, c2);
		if (fabs(s1 - s2) > 1e-6f || fabs(c1 - c2) > 1e-6f) {
			assert(0);
		}
	}
}

/*
============
xSIMD_SSE::GetName
============
*/
const TCHAR * xSIMD_SSE::Name() const {
	return _T("MMX & SSE");
}

/*
============
xSIMD_SSE::Add

  dst[i] = constant + src[i];
============
*/
void VPCALL xSIMD_SSE::Add(float *dst, float constant, float *src, const int count) {
	KFLOAT_CA(add, dst, src, constant, count)
}

/*
============
xSIMD_SSE::Add

  dst[i] = src0[i] + src1[i];
============
*/
void VPCALL xSIMD_SSE::Add(float *dst, float *src0, float *src1, const int count) {
	KFLOAT_AA(add, dst, src0, src1, count)
}

/*
============
xSIMD_SSE::Sub

  dst[i] = constant - src[i];
============
*/
void VPCALL xSIMD_SSE::Sub(float *dst, float constant, float *src, const int count) {
	KFLOAT_CA(sub, dst, src, constant, count)
}

/*
============
xSIMD_SSE::Sub

  dst[i] = src0[i] - src1[i];
============
*/
void VPCALL xSIMD_SSE::Sub(float *dst, float *src0, float *src1, const int count) {
	KFLOAT_AA(sub, dst, src0, src1, count)
}

/*
============
xSIMD_SSE::Mul

  dst[i] = constant * src[i];
============
*/
void VPCALL xSIMD_SSE::Mul(float *dst, float constant, float *src, const int count) {
	KFLOAT_CA(mul, dst, src, constant, count)
}

/*
============
xSIMD_SSE::Mul

  dst[i] = src0[i] * src1[i];
============
*/
void VPCALL xSIMD_SSE::Mul(float *dst, float *src0, float *src1, const int count) {
	KFLOAT_AA(mul, dst, src0, src1, count)
}

/*
============
xSIMD_SSE::Div

  dst[i] = constant / src[i];
============
*/
void VPCALL xSIMD_SSE::Div(float *dst, float constant, float *src, const int count) {
	int pre, post;

	//	1 / x = 2 * rcpps(x) - (x * rcpps(x) * rcpps(x));
	__asm
	{
		movss	xmm1,constant
		shufps	xmm1,xmm1,0

		KFLOATINITDS(dst, src, count, pre, post)
		and		eax,15
		jne		lpNA
		jmp		lpA
		align	16
lpA:
		movaps	xmm2,[edx+ebx]
		movaps	xmm3,[edx+ebx+16]
		rcpps	xmm4,xmm2
		rcpps	xmm5,xmm3
		prefetchnta	[edx+ebx+64]
		mulps	xmm2,xmm4
		mulps	xmm2,xmm4
		mulps	xmm3,xmm5
		mulps	xmm3,xmm5
		addps	xmm4,xmm4
		addps	xmm5,xmm5
		subps	xmm4,xmm2
		subps	xmm5,xmm3
		mulps	xmm4,xmm1
		mulps	xmm5,xmm1
		movaps	[edi+ebx],xmm4
		movaps	[edi+ebx+16],xmm5
		add		ebx,16*2
		jl		lpA
		jmp		done
		align	16
lpNA:
		movups	xmm2,[edx+ebx]
		movups	xmm3,[edx+ebx+16]
		rcpps	xmm4,xmm2
		rcpps	xmm5,xmm3
		prefetchnta	[edx+ebx+64]
		mulps	xmm2,xmm4
		mulps	xmm2,xmm4
		mulps	xmm3,xmm5
		mulps	xmm3,xmm5
		addps	xmm4,xmm4
		addps	xmm5,xmm5
		subps	xmm4,xmm2
		subps	xmm5,xmm3
		mulps	xmm4,xmm1
		mulps	xmm5,xmm1
		movaps	[edi+ebx],xmm4
		movaps	[edi+ebx+16],xmm5
		add		ebx,16*2
		jl		lpNA
done:
		mov		edx,src
		mov		edi,dst
		KFLOATOPER(KDIVDSS1([edi+ebx],xmm1,[edx+ebx]),
					KDIVDSS4([edi+ebx],xmm1,[edx+ebx]), count)
	}
}

/*
============
xSIMD_SSE::Div

  dst[i] = src0[i] / src1[i];
============
*/
void VPCALL xSIMD_SSE::Div(float *dst, float *src0, float *src1, const int count) {
	int		pre,post;

	//	1 / x = 2 * rcpps(x) - (x * rcpps(x) * rcpps(x));
	__asm
	{
		KFLOATINITDSS(dst, src0, src1, count, pre, post)
		and		eax,15
		jne		lpNA
		jmp		lpA
		align	16
lpA:
		movaps	xmm2,[esi+ebx]
		movaps	xmm3,[esi+ebx+16]
		rcpps	xmm4,xmm2
		rcpps	xmm5,xmm3
		prefetchnta	[esi+ebx+64]
		mulps	xmm2,xmm4
		mulps	xmm2,xmm4
		mulps	xmm3,xmm5
		mulps	xmm3,xmm5
		addps	xmm4,xmm4
		addps	xmm5,xmm5
		subps	xmm4,xmm2
		subps	xmm5,xmm3
		mulps	xmm4,[edx+ebx]
		mulps	xmm5,[edx+ebx+16]
		movaps	[edi+ebx],xmm4
		movaps	[edi+ebx+16],xmm5
		add		ebx,16*2
		jl		lpA
		jmp		done
		align	16
lpNA:
		movups	xmm2,[esi+ebx]
		movups	xmm3,[esi+ebx+16]
		rcpps	xmm4,xmm2
		rcpps	xmm5,xmm3
		prefetchnta	[esi+ebx+64]
		mulps	xmm2,xmm4
		mulps	xmm2,xmm4
		mulps	xmm3,xmm5
		mulps	xmm3,xmm5
		addps	xmm4,xmm4
		addps	xmm5,xmm5
		subps	xmm4,xmm2
		subps	xmm5,xmm3
		movups	xmm2,[edx+ebx]
		movups	xmm3,[edx+ebx+16]
		mulps	xmm4,xmm2
		mulps	xmm5,xmm3
		movaps	[edi+ebx],xmm4
		movaps	[edi+ebx+16],xmm5
		add		ebx,16*2
		jl		lpNA
done:
		mov		edx,src0
		mov		esi,src1
		mov		edi,dst
		KFLOATOPER(KDIVDSS1([edi+ebx],[edx+ebx],[esi+ebx]),
					KDIVDSS4([edi+ebx],[edx+ebx],[esi+ebx]), count)
	}
}
/*
============
Simd_MulAdd

 assumes count >= 7
============
*/
static void Simd_MulAdd(float *dst, float constant, float *src, const int count) {
	__asm	mov			esi, dst
	__asm	mov			edi, src
	__asm	mov			eax, count
	__asm	shl			eax, 2
	__asm	mov			ecx, esi
	__asm	mov			edx, eax
	__asm	or			ecx, edi
	__asm	fld			constant
	__asm	and			ecx, 15
	__asm	jz			SimdMulAdd16
	__asm	and			ecx, 3
	__asm	jnz			SimdMulAdd8
	__asm	mov			ecx, esi
	__asm	xor			ecx, edi
	__asm	and			ecx, 15
	__asm	jnz			MulAdd8
	__asm	mov			ecx, esi
	__asm	and			ecx, 15
	__asm	neg			ecx
	__asm	add			ecx, 16
	__asm	sub			eax, ecx
	__asm	add			edi, ecx
	__asm	add			esi, ecx
	__asm	neg			ecx
	__asm	mov			edx, eax
	__asm loopPreMulAdd16:
	__asm	fld			st
	__asm	fmul		dword ptr [edi+ecx]
	__asm	fadd		dword ptr [esi+ecx]
	__asm	fstp		dword ptr [esi+ecx]
	__asm	add			ecx, 4
	__asm	jl			loopPreMulAdd16
	__asm SimdMulAdd16:
	__asm	and			eax, ~15
	__asm	movss		xmm1, constant
	__asm	shufps		xmm1, xmm1, 0x00
	__asm	add			esi, eax
	__asm	add			edi, eax
	__asm	neg			eax
	__asm	align		16
	__asm loopMulAdd16:
	__asm	movaps		xmm0, [edi+eax]
	__asm	mulps		xmm0, xmm1
	__asm	addps		xmm0, [esi+eax]
	__asm	movaps		[esi+eax], xmm0
	__asm	add			eax, 16
	__asm	jl			loopMulAdd16
	__asm	jmp			postMulAdd
	__asm MulAdd8:
	__asm	mov			ecx, esi
	__asm	and			ecx, 7
	__asm	jz			SimdMulAdd8
	__asm	sub			eax, ecx
	__asm	add			esi, ecx
	__asm	add			edi, ecx
	__asm	neg			ecx
	__asm	mov			edx, eax
	__asm loopPreMulAdd8:
	__asm	fld			st
	__asm	fmul		dword ptr [edi+ecx]
	__asm	fadd		dword ptr [esi+ecx]
	__asm	fstp		dword ptr [esi+ecx]
	__asm	add			ecx, 4
	__asm	jl			loopPreMulAdd8
	__asm SimdMulAdd8:
	__asm	and			eax, ~15
	__asm	movss		xmm1, constant
	__asm	shufps		xmm1, xmm1, 0x00
	__asm	add			esi, eax
	__asm	add			edi, eax
	__asm	neg			eax
	__asm	align		16
	__asm loopMulAdd8:
	__asm	movlps		xmm0, [edi+eax]
	__asm	movhps		xmm0, [edi+eax+8]
	__asm	mulps		xmm0, xmm1
	__asm	movlps		xmm2, [esi+eax]
	__asm	movhps		xmm2, [esi+eax+8]
	__asm	addps		xmm0, xmm2
	__asm	movlps		[esi+eax], xmm0
	__asm	movhps		[esi+eax+8], xmm0
	__asm	add			eax, 16
	__asm	jl			loopMulAdd8
	__asm	jmp			postMulAdd
	__asm postMulAdd:
	__asm	and			edx, 15
	__asm	jz			MulAddDone
	__asm	add			esi, edx
	__asm	add			edi, edx
	__asm	neg			edx
	__asm loopPostMulAdd:
	__asm	fld			st
	__asm	fmul		dword ptr [edi+edx]
	__asm	fadd		dword ptr [esi+edx]
	__asm	fstp		dword ptr [esi+edx]
	__asm	add			edx, 4
	__asm	jl			loopPostMulAdd
	__asm MulAddDone:
	__asm	fstp		st
}

#define MULADD_FEW(OPER)																				\
switch(count) {																						\
	case 0:																								\
		return;																							\
	case 1:																								\
		dst[0] OPER c * src[0];																			\
		return;																							\
	case 2:																								\
		dst[0] OPER c * src[0]; dst[1] OPER c * src[1];													\
		return;																							\
	case 3:																								\
		dst[0] OPER c * src[0]; dst[1] OPER c * src[1]; dst[2] OPER c * src[2];							\
		return;																							\
	case 4:																								\
		dst[0] OPER c * src[0]; dst[1] OPER c * src[1]; dst[2] OPER c * src[2]; dst[3] OPER c * src[3];	\
		return;																							\
	case 5:																								\
		dst[0] OPER c * src[0]; dst[1] OPER c * src[1]; dst[2] OPER c * src[2]; dst[3] OPER c * src[3];	\
		dst[4] OPER c * src[4];																			\
		return;																							\
	case 6:																								\
		dst[0] OPER c * src[0]; dst[1] OPER c * src[1]; dst[2] OPER c * src[2]; dst[3] OPER c * src[3];	\
		dst[4] OPER c * src[4]; dst[5] OPER c * src[5];													\
		return;																							\
	case 7:																								\
		dst[0] OPER c * src[0]; dst[1] OPER c * src[1]; dst[2] OPER c * src[2]; dst[3] OPER c * src[3];	\
		dst[4] OPER c * src[4]; dst[5] OPER c * src[5]; dst[6] OPER c * src[6];							\
		return;																							\
	case 8:																								\
		dst[0] OPER c * src[0]; dst[1] OPER c * src[1]; dst[2] OPER c * src[2]; dst[3] OPER c * src[3];	\
		dst[4] OPER c * src[4]; dst[5] OPER c * src[5]; dst[6] OPER c * src[6]; dst[7] OPER c * src[7];	\
		return;																							\
	case 9:																								\
		dst[0] OPER c * src[0]; dst[1] OPER c * src[1]; dst[2] OPER c * src[2]; dst[3] OPER c * src[3];	\
		dst[4] OPER c * src[4]; dst[5] OPER c * src[5]; dst[6] OPER c * src[6]; dst[7] OPER c * src[7];	\
		dst[8] OPER c * src[8];																			\
		return;																							\
	case 10:																							\
		dst[0] OPER c * src[0]; dst[1] OPER c * src[1]; dst[2] OPER c * src[2]; dst[3] OPER c * src[3];	\
		dst[4] OPER c * src[4]; dst[5] OPER c * src[5]; dst[6] OPER c * src[6]; dst[7] OPER c * src[7];	\
		dst[8] OPER c * src[8]; dst[9] OPER c * src[9];													\
		return;																							\
	case 11:																							\
		dst[0] OPER c * src[0]; dst[1] OPER c * src[1]; dst[2] OPER c * src[2]; dst[3] OPER c * src[3];	\
		dst[4] OPER c * src[4]; dst[5] OPER c * src[5]; dst[6] OPER c * src[6]; dst[7] OPER c * src[7];	\
		dst[8] OPER c * src[8]; dst[9] OPER c * src[9]; dst[10] OPER c * src[10];						\
		return;																							\
}

/*
============
xSIMD_SSE::MulAdd

  dst[i] += constant * src[i];
============
*/
void VPCALL xSIMD_SSE::MulAdd(float *dst, float constant, float *src, const int count) {
	float c = constant;
	MULADD_FEW(+=)
	Simd_MulAdd(dst, constant, src, count);
}

/*
============
xSIMD_SSE::MulAdd

  dst[i] += src0[i] * src1[i];
============
*/
void VPCALL xSIMD_SSE::MulAdd(float *dst, float *src0, float *src1, const int count) {
	for (int i = 0; i < count; i++) {
		dst[i] += src0[i] + src1[i];
	}
}

/*
============
xSIMD_SSE::MulSub

  dst[i] -= constant * src[i];
============
*/
void VPCALL xSIMD_SSE::MulSub(float *dst, float constant, float *src, const int count) {
	float c = constant;
	MULADD_FEW(-=)
	Simd_MulAdd(dst, -constant, src, count);
}

/*
============
xSIMD_SSE::MulSub

  dst[i] -= src0[i] * src1[i];
============
*/
void VPCALL xSIMD_SSE::MulSub(float *dst, float *src0, float *src1, const int count) {
	for (int i = 0; i < count; i++) {
		dst[i] -= src0[i] + src1[i];
	}
}

/*
============
xSIMD_SSE::Dot

  dst[i] = constant * src[i];
============
*/
void VPCALL xSIMD_SSE::Dot(float *dst, const xVec3 &constant, const xVec3 *src, const int count) {
	__asm
	{
		mov			eax, count
		mov			edi, constant
		mov			edx, eax
		mov			esi, src
		mov			ecx, dst
		and			eax, ~3

		movss		xmm4, [edi+0]
		shufps		xmm4, xmm4, R_SHUFFLEPS(0, 0, 0, 0)
		movss		xmm5, [edi+4]
		shufps		xmm5, xmm5, R_SHUFFLEPS(0, 0, 0, 0)
		movss		xmm6, [edi+8]
		shufps		xmm6, xmm6, R_SHUFFLEPS(0, 0, 0, 0)

		jz			done4
		imul		eax, 12
		add			esi, eax
		neg			eax

	loop4:
		movlps		xmm1, [esi+eax+ 0]
		movlps		xmm2, [esi+eax+ 8]
		movlps		xmm3, [esi+eax+16]
		movhps		xmm1, [esi+eax+24]
		movhps		xmm2, [esi+eax+32]
		movhps		xmm3, [esi+eax+40]
		movaps		xmm0, xmm1
		shufps		xmm0, xmm2, R_SHUFFLEPS(0, 2, 1, 3)
		shufps		xmm1, xmm3, R_SHUFFLEPS(1, 3, 0, 2)
		shufps		xmm2, xmm3, R_SHUFFLEPS(0, 2, 1, 3)
		add			ecx, 16
		add			eax, 4*12
		mulps		xmm0, xmm4
		mulps		xmm1, xmm5
		mulps		xmm2, xmm6
		addps		xmm0, xmm1
		addps		xmm0, xmm2
		shufps		xmm0, xmm0, R_SHUFFLEPS(0, 2, 1, 3)
		movlps		[ecx-16+0], xmm0
		movhps		[ecx-16+8], xmm0
		jl			loop4

	done4:
		and			edx, 3
		jz			done1

	loop1:
		movss		xmm0, [esi+eax+0]
		movss		xmm1, [esi+eax+4]
		movss		xmm2, [esi+eax+8]
		mulss		xmm0, xmm4
		mulss		xmm1, xmm5
		mulss		xmm2, xmm6
		add			ecx, 4
		addss		xmm0, xmm1
		add			eax, 12
		addss		xmm0, xmm2
		dec			edx
		movss		[ecx-4], xmm0
		jnz			loop1

	done1:
	}
}

/*
============
xSIMD_SSE::Dot

  dst[i] = constant * src[i].Normal() + src[i][3];
============
*/
void VPCALL xSIMD_SSE::Dot(float *dst, const xVec3 &constant, const xPlane *src, const int count) {
	__asm {
		mov			eax, count
		mov			edi, constant
		mov			edx, eax
		mov			esi, src
		mov			ecx, dst
		and			eax, ~3

		movss		xmm5, [edi+0]
		shufps		xmm5, xmm5, R_SHUFFLEPS(0, 0, 0, 0)
		movss		xmm6, [edi+4]
		shufps		xmm6, xmm6, R_SHUFFLEPS(0, 0, 0, 0)
		movss		xmm7, [edi+8]
		shufps		xmm7, xmm7, R_SHUFFLEPS(0, 0, 0, 0)

		jz			startVert1
		imul		eax, 16
		add			esi, eax
		neg			eax

	loopVert4:

		movlps		xmm1, [esi+eax+ 0]
		movlps		xmm3, [esi+eax+ 8]
		movhps		xmm1, [esi+eax+16]
		movhps		xmm3, [esi+eax+24]
		movlps		xmm2, [esi+eax+32]
		movlps		xmm4, [esi+eax+40]
		movhps		xmm2, [esi+eax+48]
		movhps		xmm4, [esi+eax+56]
		movaps		xmm0, xmm1
		shufps		xmm0, xmm2, R_SHUFFLEPS(0, 2, 0, 2)
		shufps		xmm1, xmm2, R_SHUFFLEPS(1, 3, 1, 3)
		movaps		xmm2, xmm3
		shufps		xmm2, xmm4, R_SHUFFLEPS(0, 2, 0, 2)
		shufps		xmm3, xmm4, R_SHUFFLEPS(1, 3, 1, 3)

		add			ecx, 16
		add			eax, 4*16

		mulps		xmm0, xmm5
		mulps		xmm1, xmm6
		mulps		xmm2, xmm7
		addps		xmm0, xmm3
		addps		xmm0, xmm1
		addps		xmm0, xmm2

		movlps		[ecx-16+0], xmm0
		movhps		[ecx-16+8], xmm0
		jl			loopVert4

	startVert1:
		and			edx, 3
		jz			done

	loopVert1:
		movss		xmm0, [esi+eax+0]
		movss		xmm1, [esi+eax+4]
		movss		xmm2, [esi+eax+8]
		mulss		xmm0, xmm5
		mulss		xmm1, xmm6
		mulss		xmm2, xmm7
		addss		xmm0, [esi+eax+12]
		add			ecx, 4
		addss		xmm0, xmm1
		add			eax, 16
		addss		xmm0, xmm2
		dec			edx
		movss		[ecx-4], xmm0
		jnz			loopVert1

	done:
	}
}

/*
============
xSIMD_SSE::Dot

  dst[i] = constant * src[i].xyz;
============
*/
void VPCALL xSIMD_SSE::Dot(float *dst, const xVec3 &constant, const xDrawVert *src, const int count) {

	assert(sizeof(xDrawVert) == DRAWVERT_SIZE);
	assert((int)&((xDrawVert *)0)->xyz == DRAWVERT_XYZ_OFFSET);

	// 0,  1,  2
	// 3,  4,  5
	// 6,  7,  8
	// 9, 10, 11

	__asm {
		mov			eax, count
		mov			edi, constant
		mov			edx, eax
		mov			esi, src
		mov			ecx, dst
		and			eax, ~3

		movss		xmm4, [edi+0]
		shufps		xmm4, xmm4, R_SHUFFLEPS(0, 0, 0, 0)
		movss		xmm5, [edi+4]
		shufps		xmm5, xmm5, R_SHUFFLEPS(0, 0, 0, 0)
		movss		xmm6, [edi+8]
		shufps		xmm6, xmm6, R_SHUFFLEPS(0, 0, 0, 0)

		jz			startVert1
		imul		eax, DRAWVERT_SIZE
		add			esi, eax
		neg			eax

	loopVert4:
		movss		xmm0, [esi+eax+1*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]	//  3,  X,  X,  X
		movss		xmm2, [esi+eax+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+8]	//  2,  X,  X,  X
		movhps		xmm0, [esi+eax+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]	//  3,  X,  0,  1
		movaps		xmm1, xmm0												//  3,  X,  0,  1

		movlps		xmm1, [esi+eax+1*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+4]	//  4,  5,  0,  1
		shufps		xmm2, xmm1, R_SHUFFLEPS(0, 1, 0, 1)					//  2,  X,  4,  5

		movss		xmm3, [esi+eax+3*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]	//  9,  X,  X,  X
		movhps		xmm3, [esi+eax+2*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]	//  9,  X,  6,  7
		shufps		xmm0, xmm3, R_SHUFFLEPS(2, 0, 2, 0)					//  0,  3,  6,  9

		movlps		xmm3, [esi+eax+3*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+4]	// 10, 11,  6,  7
		shufps		xmm1, xmm3, R_SHUFFLEPS(3, 0, 3, 0)					//  1,  4,  7, 10

		movhps		xmm3, [esi+eax+2*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+8]	// 10, 11,  8,  X
		shufps		xmm2, xmm3, R_SHUFFLEPS(0, 3, 2, 1)					//  2,  5,  8, 11

		add			ecx, 16
		add			eax, 4*DRAWVERT_SIZE

		mulps		xmm0, xmm4
		mulps		xmm1, xmm5
		mulps		xmm2, xmm6
		addps		xmm0, xmm1
		addps		xmm0, xmm2

		movlps		[ecx-16+0], xmm0
		movhps		[ecx-16+8], xmm0
		jl			loopVert4

	startVert1:
		and			edx, 3
		jz			done

	loopVert1:
		movss		xmm0, [esi+eax+DRAWVERT_XYZ_OFFSET+0]
		movss		xmm1, [esi+eax+DRAWVERT_XYZ_OFFSET+4]
		movss		xmm2, [esi+eax+DRAWVERT_XYZ_OFFSET+8]
		mulss		xmm0, xmm4
		mulss		xmm1, xmm5
		mulss		xmm2, xmm6
		add			ecx, 4
		addss		xmm0, xmm1
		add			eax, DRAWVERT_SIZE
		addss		xmm0, xmm2
		dec			edx
		movss		[ecx-4], xmm0
		jnz			loopVert1

	done:
	}
}

/*
============
xSIMD_SSE::Dot

  dst[i] = constant.Normal() * src[i] + constant[3];
============
*/
void VPCALL xSIMD_SSE::Dot(float *dst, const xPlane &constant, const xVec3 *src, const int count) {
	__asm
	{
		mov			eax, count
		mov			edi, constant
		mov			edx, eax
		mov			esi, src
		mov			ecx, dst
		and			eax, ~3

		movss		xmm4, [edi+0]
		shufps		xmm4, xmm4, R_SHUFFLEPS(0, 0, 0, 0)
		movss		xmm5, [edi+4]
		shufps		xmm5, xmm5, R_SHUFFLEPS(0, 0, 0, 0)
		movss		xmm6, [edi+8]
		shufps		xmm6, xmm6, R_SHUFFLEPS(0, 0, 0, 0)
		movss		xmm7, [edi+12]
		shufps		xmm7, xmm7, R_SHUFFLEPS(0, 0, 0, 0)

		jz			done4
		imul		eax, 12
		add			esi, eax
		neg			eax

	loop4:
		movlps		xmm1, [esi+eax+ 0]
		movlps		xmm2, [esi+eax+ 8]
		movlps		xmm3, [esi+eax+16]
		movhps		xmm1, [esi+eax+24]
		movhps		xmm2, [esi+eax+32]
		movhps		xmm3, [esi+eax+40]
		movaps		xmm0, xmm1
		shufps		xmm0, xmm2, R_SHUFFLEPS(0, 2, 1, 3)
		shufps		xmm1, xmm3, R_SHUFFLEPS(1, 3, 0, 2)
		shufps		xmm2, xmm3, R_SHUFFLEPS(0, 2, 1, 3)

		add			ecx, 16
		add			eax, 4*12

		mulps		xmm0, xmm4
		mulps		xmm1, xmm5
		mulps		xmm2, xmm6
		addps		xmm0, xmm7
		addps		xmm0, xmm1
		addps		xmm0, xmm2
		shufps		xmm0, xmm0, R_SHUFFLEPS(0, 2, 1, 3)

		movlps		[ecx-16+0], xmm0
		movhps		[ecx-16+8], xmm0
		jl			loop4

	done4:
		and			edx, 3
		jz			done1

	loop1:
		movss		xmm0, [esi+eax+0]
		movss		xmm1, [esi+eax+4]
		movss		xmm2, [esi+eax+8]
		mulss		xmm0, xmm4
		mulss		xmm1, xmm5
		mulss		xmm2, xmm6
		addss		xmm0, xmm7
		add			ecx, 4
		addss		xmm0, xmm1
		add			eax, 12
		addss		xmm0, xmm2
		dec			edx
		movss		[ecx-4], xmm0
		jnz			loop1

	done1:
	}
}

/*
============
xSIMD_SSE::Dot

  dst[i] = constant.Normal() * src[i].Normal() + constant[3] * src[i][3];
============
*/
void VPCALL xSIMD_SSE::Dot(float *dst, const xPlane &constant, const xPlane *src, const int count) {

#define SINGLE_OP(SRC, DEST)							\
	__asm	movlps		xmm0,[SRC]						\
	__asm	movlps		xmm1,[SRC+8]					\
	__asm	mulps		xmm0,xmm4						\
	__asm	mulps		xmm1,xmm5						\
	__asm	addps		xmm0,xmm1						\
	__asm	movaps		xmm1,xmm0						\
	__asm	shufps		xmm1,xmm1,SHUFFLEPS(1,1,1,1)	\
	__asm	addss		xmm0,xmm1						\
	__asm	movss		[DEST],xmm0						\
	__asm	add			SRC,16							\
	__asm	add			DEST,4

#define DUAL_OP(SRC, DEST)								\
	__asm	movlps		xmm0,[SRC]						\
	__asm	movlps		xmm1,[SRC+8]					\
	__asm	movhps		xmm0,[SRC+16]					\
	__asm	movhps		xmm1,[SRC+24]					\
	__asm	mulps		xmm0,xmm4						\
	__asm	mulps		xmm1,xmm5						\
	__asm	addps		xmm0,xmm1						\
	__asm	shufps		xmm1,xmm0,SHUFFLEPS(2,0,1,0)	\
	__asm	shufps		xmm0,xmm0,SHUFFLEPS(3,1,2,0)	\
	__asm	addps		xmm0,xmm1						\
	__asm	movhps		[DEST],xmm0						\
	__asm	add			SRC,32							\
	__asm	add			DEST,8

	__asm {
		mov			edx, dst
		mov			eax, src
		mov			ebx, constant
		mov			ecx, count

		movlps		xmm4, [ebx]
		shufps		xmm4, xmm4, SHUFFLEPS(1,0,1,0)
		movlps		xmm5, [ebx+8]
		shufps		xmm5, xmm5, SHUFFLEPS(1,0,1,0)

		xorps		xmm0, xmm0
		xorps		xmm1, xmm1

	_lpAlignDest:
		test		edx, 0x0f
		jz			_destAligned
		SINGLE_OP(eax,edx)
		dec			ecx
		jnz			_lpAlignDest
		jmp			_vpExit

	_destAligned:
		push		ecx

		cmp			ecx, 4
		jl			_post

		and			ecx, ~3
		shl			ecx, 2
		lea			eax, [eax+ecx*4]
		add			edx, ecx
		neg			ecx		

		movlps		xmm0, [eax+ecx*4]
		movhps		xmm0, [eax+ecx*4+16]
		movlps		xmm2, [eax+ecx*4+32]
		movhps		xmm2, [eax+ecx*4+48]
		jmp			_lpStart

		align	16
	_lp:
		prefetchnta	[eax+ecx*4+128]
		addps		xmm1, xmm0
		movlps		xmm0, [eax+ecx*4]
		movhps		xmm0, [eax+ecx*4+16]
		movlps		xmm2, [eax+ecx*4+32]
		movhps		xmm2, [eax+ecx*4+48]
		movaps		[edx+ecx-16],xmm1
	_lpStart:
		movlps		xmm1, [eax+ecx*4+8]
		movhps		xmm1, [eax+ecx*4+24]
		movlps		xmm3, [eax+ecx*4+40]
		movhps		xmm3, [eax+ecx*4+56]
		add			ecx, 16
		mulps		xmm1, xmm5
		mulps		xmm2, xmm4
		mulps		xmm3, xmm5
		addps		xmm2, xmm3						// y3+w3 x3+z3 y2+w2 x2+z2
		mulps		xmm0, xmm4
		addps		xmm0, xmm1						// y1+w1 x1+z1 y0+w0 x0+z0
		movaps		xmm1, xmm0
		shufps		xmm0, xmm2, SHUFFLEPS(2,0,2,0)	// x3+z3 x2+z2 x1+z1 x0+z0
		shufps		xmm1, xmm2, SHUFFLEPS(3,1,3,1)	// y3+w3 y2+w2 y1+w1 y0+w0
		js			_lp
		addps		xmm1, xmm0
		movaps		[edx+ecx-16], xmm1
	_post:
		pop			ecx
		and			ecx, 0x3
		cmp			ecx, 2
		jl			_post1
		DUAL_OP(eax,edx)
		sub			ecx, 2
	_post1:
		cmp			ecx, 1
		jne			_vpExit
		SINGLE_OP(eax,edx)
	_vpExit:
	}

#undef DUAL_OP
#undef SINGLE_OP

}

/*
============
xSIMD_SSE::Dot

  dst[i] = constant.Normal() * src[i].xyz + constant[3];
============
*/
void VPCALL xSIMD_SSE::Dot(float *dst, const xPlane &constant, const xDrawVert *src, const int count) {

	assert(sizeof(xDrawVert) == DRAWVERT_SIZE);
	assert((int)&((xDrawVert *)0)->xyz == DRAWVERT_XYZ_OFFSET);

	// 0,  1,  2
	// 3,  4,  5
	// 6,  7,  8
	// 9, 10, 11

	__asm {
		mov			eax, count
		mov			edi, constant
		mov			edx, eax
		mov			esi, src
		mov			ecx, dst
		and			eax, ~3

		movss		xmm4, [edi+0]
		shufps		xmm4, xmm4, R_SHUFFLEPS(0, 0, 0, 0)
		movss		xmm5, [edi+4]
		shufps		xmm5, xmm5, R_SHUFFLEPS(0, 0, 0, 0)
		movss		xmm6, [edi+8]
		shufps		xmm6, xmm6, R_SHUFFLEPS(0, 0, 0, 0)
		movss		xmm7, [edi+12]
		shufps		xmm7, xmm7, R_SHUFFLEPS(0, 0, 0, 0)

		jz			startVert1
		imul		eax, DRAWVERT_SIZE
		add			esi, eax
		neg			eax

	loopVert4:
		movss		xmm0, [esi+eax+1*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]	//  3,  X,  X,  X
		movss		xmm2, [esi+eax+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+8]	//  2,  X,  X,  X
		movhps		xmm0, [esi+eax+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]	//  3,  X,  0,  1
		movaps		xmm1, xmm0												//  3,  X,  0,  1

		movlps		xmm1, [esi+eax+1*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+4]	//  4,  5,  0,  1
		shufps		xmm2, xmm1, R_SHUFFLEPS(0, 1, 0, 1)					//  2,  X,  4,  5

		movss		xmm3, [esi+eax+3*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]	//  9,  X,  X,  X
		movhps		xmm3, [esi+eax+2*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]	//  9,  X,  6,  7
		shufps		xmm0, xmm3, R_SHUFFLEPS(2, 0, 2, 0)					//  0,  3,  6,  9

		movlps		xmm3, [esi+eax+3*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+4]	// 10, 11,  6,  7
		shufps		xmm1, xmm3, R_SHUFFLEPS(3, 0, 3, 0)					//  1,  4,  7, 10

		movhps		xmm3, [esi+eax+2*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+8]	// 10, 11,  8,  X
		shufps		xmm2, xmm3, R_SHUFFLEPS(0, 3, 2, 1)					//  2,  5,  8, 11

		add			ecx, 16
		add			eax, 4*DRAWVERT_SIZE

		mulps		xmm0, xmm4
		mulps		xmm1, xmm5
		mulps		xmm2, xmm6
		addps		xmm0, xmm7
		addps		xmm0, xmm1
		addps		xmm0, xmm2

		movlps		[ecx-16+0], xmm0
		movhps		[ecx-16+8], xmm0
		jl			loopVert4

	startVert1:
		and			edx, 3
		jz			done

	loopVert1:
		movss		xmm0, [esi+eax+DRAWVERT_XYZ_OFFSET+0]
		movss		xmm1, [esi+eax+DRAWVERT_XYZ_OFFSET+4]
		movss		xmm2, [esi+eax+DRAWVERT_XYZ_OFFSET+8]
		mulss		xmm0, xmm4
		mulss		xmm1, xmm5
		mulss		xmm2, xmm6
		addss		xmm0, xmm7
		add			ecx, 4
		addss		xmm0, xmm1
		add			eax, DRAWVERT_SIZE
		addss		xmm0, xmm2
		dec			edx
		movss		[ecx-4], xmm0
		jnz			loopVert1

	done:
	}
}

/*
============
xSIMD_SSE::Dot

  dst[i] = src0[i] * src1[i];
============
*/
void VPCALL xSIMD_SSE::Dot(float *dst, const xVec3 *src0, const xVec3 *src1, const int count) {
	__asm
	{
		mov			eax, count
		mov			edi, src0
		mov			edx, eax
		mov			esi, src1
		mov			ecx, dst
		and			eax, ~3

		jz			done4
		imul		eax, 12
		add			edi, eax
		add			esi, eax
		neg			eax

	loop4:
		movlps		xmm0, [esi+eax]						// 0, 1, X, X
		movlps		xmm3, [edi+eax]						// 0, 1, X, X
		movlps		xmm1, [esi+eax+8]					// 2, 3, X, X
		movlps		xmm4, [edi+eax+8]					// 2, 3, X, X
		movhps		xmm0, [esi+eax+24]					// 0, 1, 6, 7
		movhps		xmm3, [edi+eax+24]					// 0, 1, 6, 7
		movhps		xmm1, [esi+eax+32]					// 2, 3, 8, 9
		movhps		xmm4, [edi+eax+32]					// 2, 3, 8, 9
		movlps		xmm2, [esi+eax+16]					// 4, 5, X, X
		movlps		xmm5, [edi+eax+16]					// 4, 5, X, X
		movhps		xmm2, [esi+eax+40]					// 4, 5, 10, 11
		movhps		xmm5, [edi+eax+40]					// 4, 5, 10, 11

		add			ecx, 16
		add			eax, 48

		mulps		xmm0, xmm3
		mulps		xmm1, xmm4
		mulps		xmm2, xmm5
		movaps		xmm7, xmm0
		shufps		xmm7, xmm1, R_SHUFFLEPS(0, 2, 1, 3)	// 0, 6, 3, 9
		shufps		xmm0, xmm2, R_SHUFFLEPS(1, 3, 0, 2)	// 1, 7, 4, 10
		shufps		xmm1, xmm2, R_SHUFFLEPS(0, 2, 1, 3)	// 2, 8, 5, 11
		addps		xmm7, xmm0
		addps		xmm7, xmm1
		shufps		xmm7, xmm7, R_SHUFFLEPS(0, 2, 1, 3)

		movlps		[ecx-16+0], xmm7
		movhps		[ecx-16+8], xmm7
		jl			loop4

	done4:
		and			edx, 3
		jz			done1

	loop1:
		movss		xmm0, [esi+eax+0]
		movss		xmm3, [edi+eax+0]
		movss		xmm1, [esi+eax+4]
		movss		xmm4, [edi+eax+4]
		movss		xmm2, [esi+eax+8]
		movss		xmm5, [edi+eax+8]
		mulss		xmm0, xmm3
		mulss		xmm1, xmm4
		mulss		xmm2, xmm5
		add			ecx, 4
		addss		xmm0, xmm1
		add			eax, 12
		addss		xmm0, xmm2
		dec			edx
		movss		[ecx-4], xmm0
		jnz			loop1

	done1:
	}
}

/*
============
xSIMD_SSE::Dot

  dot = src1[0] * src2[0] + src1[1] * src2[1] + src1[2] * src2[2] + ...
============
*/
void VPCALL xSIMD_SSE::Dot(float &dot, float *src1, float *src2, const int count) {
	switch(count) {
		case 0:
			dot = 0.0f;
			return;
		case 1:
			dot = src1[0] * src2[0];
			return;
		case 2:
			dot = src1[0] * src2[0] + src1[1] * src2[1];
			return;
		case 3:
			dot = src1[0] * src2[0] + src1[1] * src2[1] + src1[2] * src2[2];
			return;
		default:
			__asm {
				mov			ecx, src1
				mov			edx, src2
				mov			eax, ecx
				or			eax, edx
				and			eax, 15
				jz			alignedDot
				// unaligned
				mov			eax, count
				shr			eax, 2
				shl			eax, 4
				add			ecx, eax
				add			edx, eax
				neg			eax
				movups		xmm0, [ecx+eax]
				movups		xmm1, [edx+eax]
				mulps		xmm0, xmm1
				add			eax, 16
				jz			doneDot
			loopUnalignedDot:
				movups		xmm1, [ecx+eax]
				movups		xmm2, [edx+eax]
				mulps		xmm1, xmm2
				addps		xmm0, xmm1
				add			eax, 16
				jl			loopUnalignedDot
				jmp			doneDot
				// aligned
			alignedDot:
				mov			eax, count
				shr			eax, 2
				shl			eax, 4
				add			ecx, eax
				add			edx, eax
				neg			eax
				movaps		xmm0, [ecx+eax]
				movaps		xmm1, [edx+eax]
				mulps		xmm0, xmm1
				add			eax, 16
				jz			doneDot
			loopAlignedDot:
				movaps		xmm1, [ecx+eax]
				movaps		xmm2, [edx+eax]
				mulps		xmm1, xmm2
				addps		xmm0, xmm1
				add			eax, 16
				jl			loopAlignedDot
			doneDot:
			}
			switch(count & 3) {
				case 1:
					__asm {
						movss	xmm1, [ecx]
						movss	xmm2, [edx]
						mulss	xmm1, xmm2
						addss	xmm0, xmm1
					}
					break;
				case 2:
					__asm {
						xorps	xmm2, xmm2
						movlps	xmm1, [ecx]
						movlps	xmm2, [edx]
						mulps	xmm1, xmm2
						addps	xmm0, xmm1
					}
					break;
				case 3:
					__asm {
						movss	xmm1, [ecx]
						movhps	xmm1, [ecx+4]
						movss	xmm2, [edx]
						movhps	xmm2, [edx+4]
						mulps	xmm1, xmm2
						addps	xmm0, xmm1
					}
					break;
			}
			__asm {
				movhlps		xmm1, xmm0
				addps		xmm0, xmm1
				movaps		xmm1, xmm0
				shufps		xmm1, xmm1, R_SHUFFLEPS(1, 0, 0, 0)
				addss		xmm0, xmm1
				mov			eax, dot
				movss		[eax], xmm0
			}
			return;
	}
}

//
//	cmpeqps		==		Equal
//	cmpneqps	!=		Not Equal
//	cmpltps		<		Less Than
//  cmpnltps	>=		Not Less Than
//	cmpnleps	>		Not Less Or Equal
//
#define FLIP	not al
#define NOFLIP

#define COMPARECONSTANT(DST, SRC0, CONSTANT, COUNT, CMP, CMPSIMD, DOFLIP)				\
	int i, cnt, pre, post;																\
	float *aligned;																		\
																						\
	/* if the float array is not aligned on a 4 byte boundary */						\
	if (((int) SRC0) & 3) {															\
		/* unaligned memory access */													\
		pre = 0;																		\
		cnt = COUNT >> 2;																\
		post = COUNT - (cnt<<2);														\
		__asm	mov			edx, cnt													\
		__asm	test		edx, edx													\
		__asm	je			doneCmp														\
		__asm	push		ebx															\
		__asm	neg			edx															\
		__asm	mov			esi, SRC0													\
		__asm	prefetchnta	[esi+64]													\
		__asm	movss		xmm1, CONSTANT												\
		__asm	shufps		xmm1, xmm1, R_SHUFFLEPS(0, 0, 0, 0)						\
		__asm	mov			edi, DST													\
		__asm	mov			ecx, 0x01010101												\
		__asm loopNA:																	\
		__asm	movups		xmm0, [esi]													\
		__asm	prefetchnta	[esi+128]													\
		__asm	CMPSIMD		xmm0, xmm1													\
		__asm	movmskps	eax, xmm0													\
		__asm	DOFLIP																	\
		__asm	mov			ah, al														\
		__asm	shr			ah, 1														\
		__asm	mov			bx, ax														\
		__asm	shl			ebx, 14														\
		__asm	mov			bx, ax														\
		__asm	and			ebx, ecx													\
		__asm	mov			dword ptr [edi], ebx										\
		__asm	add			esi, 16														\
		__asm	add			edi, 4														\
		__asm	inc			edx															\
		__asm	jl			loopNA														\
		__asm	pop			ebx															\
	}																					\
	else {																				\
		/* aligned memory access */														\
		aligned = (float *) ((((int) SRC0) + 15) & ~15);								\
		if ((int)aligned > ((int)src0) + COUNT) {										\
			pre = COUNT;																\
			post = 0;																	\
		}																				\
		else {																			\
			pre = aligned - SRC0;														\
			cnt = (COUNT - pre) >> 2;													\
			post = COUNT - pre - (cnt<<2);												\
			__asm	mov			edx, cnt												\
			__asm	test		edx, edx												\
			__asm	je			doneCmp													\
			__asm	push		ebx														\
			__asm	neg			edx														\
			__asm	mov			esi, aligned											\
			__asm	prefetchnta	[esi+64]												\
			__asm	movss		xmm1, CONSTANT											\
			__asm	shufps		xmm1, xmm1, R_SHUFFLEPS(0, 0, 0, 0)					\
			__asm	mov			edi, DST												\
			__asm	add			edi, pre												\
			__asm	mov			ecx, 0x01010101											\
			__asm loopA:																\
			__asm	movaps		xmm0, [esi]												\
			__asm	prefetchnta	[esi+128]												\
			__asm	CMPSIMD		xmm0, xmm1												\
			__asm	movmskps	eax, xmm0												\
			__asm	DOFLIP																\
			__asm	mov			ah, al													\
			__asm	shr			ah, 1													\
			__asm	mov			bx, ax													\
			__asm	shl			ebx, 14													\
			__asm	mov			bx, ax													\
			__asm	and			ebx, ecx												\
			__asm	mov			dword ptr [edi], ebx									\
			__asm	add			esi, 16													\
			__asm	add			edi, 4													\
			__asm	inc			edx														\
			__asm	jl			loopA													\
			__asm	pop			ebx														\
		}																				\
	}																					\
	doneCmp:																			\
	double c = constant;																\
	for (i = 0; i < pre; i++) {														\
		dst[i] = src0[i] CMP c;															\
	}																					\
 	for (i = count - post; i < count; i++) {											\
		dst[i] = src0[i] CMP c;															\
	}

#define COMPAREBITCONSTANT(DST, BITNUM, SRC0, CONSTANT, COUNT, CMP, CMPSIMD, DOFLIP)	\
	int i, cnt, pre, post;																\
	float *aligned;																		\
																						\
	/* if the float array is not aligned on a 4 byte boundary */						\
	if (((int) SRC0) & 3) {															\
		/* unaligned memory access */													\
		pre = 0;																		\
		cnt = COUNT >> 2;																\
		post = COUNT - (cnt<<2);														\
		__asm	mov			edx, cnt													\
		__asm	test		edx, edx													\
		__asm	je			doneCmp														\
		__asm	push		ebx															\
		__asm	neg			edx															\
		__asm	mov			esi, SRC0													\
		__asm	prefetchnta	[esi+64]													\
		__asm	movss		xmm1, CONSTANT												\
		__asm	shufps		xmm1, xmm1, R_SHUFFLEPS(0, 0, 0, 0)						\
		__asm	mov			edi, DST													\
		__asm	mov			cl, bitNum													\
		__asm loopNA:																	\
		__asm	movups		xmm0, [esi]													\
		__asm	prefetchnta	[esi+128]													\
		__asm	CMPSIMD		xmm0, xmm1													\
		__asm	movmskps	eax, xmm0													\
		__asm	DOFLIP																	\
		__asm	mov			ah, al														\
		__asm	shr			ah, 1														\
		__asm	mov			bx, ax														\
		__asm	shl			ebx, 14														\
		__asm	mov			bx, ax														\
		__asm	and			ebx, 0x01010101												\
		__asm	shl			ebx, cl														\
		__asm	or			ebx, dword ptr [edi]										\
		__asm	mov			dword ptr [edi], ebx										\
		__asm	add			esi, 16														\
		__asm	add			edi, 4														\
		__asm	inc			edx															\
		__asm	jl			loopNA														\
		__asm	pop			ebx															\
	}																					\
	else {																				\
		/* aligned memory access */														\
		aligned = (float *) ((((int) SRC0) + 15) & ~15);								\
		if ((int)aligned > ((int)src0) + COUNT) {										\
			pre = COUNT;																\
			post = 0;																	\
		}																				\
		else {																			\
			pre = aligned - SRC0;														\
			cnt = (COUNT - pre) >> 2;													\
			post = COUNT - pre - (cnt<<2);												\
			__asm	mov			edx, cnt												\
			__asm	test		edx, edx												\
			__asm	je			doneCmp													\
			__asm	push		ebx														\
			__asm	neg			edx														\
			__asm	mov			esi, aligned											\
			__asm	prefetchnta	[esi+64]												\
			__asm	movss		xmm1, CONSTANT											\
			__asm	shufps		xmm1, xmm1, R_SHUFFLEPS(0, 0, 0, 0)					\
			__asm	mov			edi, DST												\
			__asm	add			edi, pre												\
			__asm	mov			cl, bitNum												\
			__asm loopA:																\
			__asm	movaps		xmm0, [esi]												\
			__asm	prefetchnta	[esi+128]												\
			__asm	CMPSIMD		xmm0, xmm1												\
			__asm	movmskps	eax, xmm0												\
			__asm	DOFLIP																\
			__asm	mov			ah, al													\
			__asm	shr			ah, 1													\
			__asm	mov			bx, ax													\
			__asm	shl			ebx, 14													\
			__asm	mov			bx, ax													\
			__asm	and			ebx, 0x01010101											\
			__asm	shl			ebx, cl													\
			__asm	or			ebx, dword ptr [edi]									\
			__asm	mov			dword ptr [edi], ebx									\
			__asm	add			esi, 16													\
			__asm	add			edi, 4													\
			__asm	inc			edx														\
			__asm	jl			loopA													\
			__asm	pop			ebx														\
		}																				\
	}																					\
	doneCmp:																			\
	float c = constant;																	\
	for (i = 0; i < pre; i++) {														\
		dst[i] |= (src0[i] CMP c) << BITNUM;											\
	}																					\
 	for (i = count - post; i < count; i++) {											\
		dst[i] |= (src0[i] CMP c) << BITNUM;											\
	}

/*
============
xSIMD_SSE::CmpGT

  dst[i] = src0[i] > constant;
============
*/
void VPCALL xSIMD_SSE::CmpGT(byte *dst, float *src0, float constant, const int count) {
	COMPARECONSTANT(dst, src0, constant, count, >, cmpnleps, NOFLIP)
}

/*
============
xSIMD_SSE::CmpGT

  dst[i] |= (src0[i] > constant) << bitNum;
============
*/
void VPCALL xSIMD_SSE::CmpGT(byte *dst, const byte bitNum, float *src0, float constant, const int count) {
	COMPAREBITCONSTANT(dst, bitNum, src0, constant, count, >, cmpnleps, NOFLIP)
}

/*
============
xSIMD_SSE::CmpGE

  dst[i] = src0[i] >= constant;
============
*/
void VPCALL xSIMD_SSE::CmpGE(byte *dst, float *src0, float constant, const int count) {
	COMPARECONSTANT(dst, src0, constant, count, >=, cmpnltps, NOFLIP)
}

/*
============
xSIMD_SSE::CmpGE

  dst[i] |= (src0[i] >= constant) << bitNum;
============
*/
void VPCALL xSIMD_SSE::CmpGE(byte *dst, const byte bitNum, float *src0, float constant, const int count) {
	COMPAREBITCONSTANT(dst, bitNum, src0, constant, count, >=, cmpnltps, NOFLIP)
}

/*
============
xSIMD_SSE::CmpLT

  dst[i] = src0[i] < constant;
============
*/
void VPCALL xSIMD_SSE::CmpLT(byte *dst, float *src0, float constant, const int count) {
	COMPARECONSTANT(dst, src0, constant, count, <, cmpltps, NOFLIP)
}

/*
============
xSIMD_SSE::CmpLT

  dst[i] |= (src0[i] < constant) << bitNum;
============
*/
void VPCALL xSIMD_SSE::CmpLT(byte *dst, const byte bitNum, float *src0, float constant, const int count) {
	COMPAREBITCONSTANT(dst, bitNum, src0, constant, count, <, cmpltps, NOFLIP)
}

/*
============
xSIMD_SSE::CmpLE

  dst[i] = src0[i] <= constant;
============
*/
void VPCALL xSIMD_SSE::CmpLE(byte *dst, float *src0, float constant, const int count) {
	COMPARECONSTANT(dst, src0, constant, count, <=, cmpnleps, FLIP)
}

/*
============
xSIMD_SSE::CmpLE

  dst[i] |= (src0[i] <= constant) << bitNum;
============
*/
void VPCALL xSIMD_SSE::CmpLE(byte *dst, const byte bitNum, float *src0, float constant, const int count) {
	COMPAREBITCONSTANT(dst, bitNum, src0, constant, count, <=, cmpnleps, FLIP)
}

/*
============
xSIMD_SSE::MinMax
============
*/
void VPCALL xSIMD_SSE::MinMax(float &min, float &max, float *src, const int count) {
	int i, pre, post;

	min = xMath::INFINITY; max = -xMath::INFINITY;

	__asm
	{
		push		ebx
		mov			eax, min
		mov			ebx, max
		movss		xmm0, [eax]
		movss		xmm1, [ebx]
		shufps		xmm0, xmm0, 0
		shufps		xmm1, xmm1, 0

		KFLOATINITS(src, count, pre, post)
		and			eax, 15
		jz			lpA
		jmp			lpNA
		align		16
lpNA:
		movups		xmm2, [edx+ebx]
		movups		xmm3, [edx+ebx+16]
		minps		xmm0, xmm2
		maxps		xmm1, xmm2
		prefetchnta	[edx+ebx+64]
		minps		xmm0, xmm3
		maxps		xmm1, xmm3
		add			ebx, 16*2
		jl			lpNA
		jmp			done2
lpA:
		movaps		xmm2, [edx+ebx]
		movaps		xmm3, [edx+ebx+16]
		minps		xmm0, xmm2
		maxps		xmm1, xmm2
		prefetchnta	[edx+ebx+64]
		minps		xmm0, xmm3
		maxps		xmm1, xmm3
		add			ebx, 16*2
		jl			lpA
		jmp			done2
		align		16
done2:
		movaps		xmm2, xmm0
		movaps		xmm3, xmm1
		shufps		xmm2, xmm2, R_SHUFFLEPS(1, 2, 3, 0)
		shufps		xmm3, xmm3, R_SHUFFLEPS(1, 2, 3, 0)
		minss		xmm0, xmm2
		maxss		xmm1, xmm3
		shufps		xmm2, xmm2, R_SHUFFLEPS(1, 2, 3, 0)
		shufps		xmm3, xmm3, R_SHUFFLEPS(1, 2, 3, 0)
		minss		xmm0, xmm2
		maxss		xmm1, xmm3
		shufps		xmm2, xmm2, R_SHUFFLEPS(1, 2, 3, 0)
		shufps		xmm3, xmm3, R_SHUFFLEPS(1, 2, 3, 0)
		minss		xmm0, xmm2
		maxss		xmm1, xmm3
		mov			eax, min
		mov			ebx, max
		movss		[eax], xmm0
		movss		[ebx], xmm1
done:
		pop			ebx
	}

	for (i = 0; i < pre; i++) {
		float tmp = src[i];
		if (tmp > max) {
			max = tmp;
		}
		if (tmp < min) {
			min = tmp;
		}
	}
 	for (i = count - post; i < count; i++) {
		float tmp = src[i];
		if (tmp > max) {
			max = tmp;
		}
		if (tmp < min) {
			min = tmp;
		}
	}
}

/*
============
xSIMD_SSE::MinMax
============
*/
void VPCALL xSIMD_SSE::MinMax(xVec2 &min, xVec2 &max, const xVec2 *src, const int count) {
	__asm {
		mov			eax, count
		test		eax, eax
		movss		xmm0, xMath::INFINITY
		xorps		xmm1, xmm1
		shufps		xmm0, xmm0, R_SHUFFLEPS(0, 0, 0, 0)
		subps		xmm1, xmm0
		jz			done
		mov			ecx, eax
		and			ecx, 1
		mov			esi, src
		jz			startLoop
		movlps		xmm2, [esi]
		shufps		xmm2, xmm2, R_SHUFFLEPS(0, 1, 0, 1)
		dec			eax
		add			esi, 2*4
		minps		xmm0, xmm2
		maxps		xmm1, xmm2
	startLoop:
		imul		eax, 2*4
		add			esi, eax
		neg			eax
	loopVert:
		movlps		xmm2, [esi+eax]
		movhps		xmm2, [esi+eax+8]
		add			eax, 4*4
		minps		xmm0, xmm2
		maxps		xmm1, xmm2
		jl			loopVert
	done:
		movaps		xmm2, xmm0
		shufps		xmm2, xmm2, R_SHUFFLEPS(2, 3, 0, 1)
		minps		xmm0, xmm2
		mov			esi, min
		movlps		[esi], xmm0
		movaps		xmm3, xmm1
		shufps		xmm3, xmm3, R_SHUFFLEPS(2, 3, 0, 1)
		maxps		xmm1, xmm3
		mov			edi, max
		movlps		[edi], xmm1
	}
}

/*
============
xSIMD_SSE::MinMax
============
*/
void VPCALL xSIMD_SSE::MinMax(xVec3 &min, xVec3 &max, const xVec3 *src, const int count) {
	__asm {

		movss		xmm0, xMath::INFINITY
		xorps		xmm1, xmm1
		shufps		xmm0, xmm0, R_SHUFFLEPS(0, 0, 0, 0)
		subps		xmm1, xmm0
		movaps		xmm2, xmm0
		movaps		xmm3, xmm1

		mov			esi, src
		mov			eax, count
		and			eax, ~3
		jz			done4
		imul		eax, 12
		add			esi, eax
		neg			eax

	loop4:
//		prefetchnta	[esi+4*12]

		movss		xmm4, [esi+eax+0*12+8]
		movhps		xmm4, [esi+eax+0*12+0]
		minps		xmm0, xmm4
		maxps		xmm1, xmm4

		movss		xmm5, [esi+eax+1*12+0]
		movhps		xmm5, [esi+eax+1*12+4]
		minps		xmm2, xmm5
		maxps		xmm3, xmm5

		movss		xmm6, [esi+eax+2*12+8]
		movhps		xmm6, [esi+eax+2*12+0]
		minps		xmm0, xmm6
		maxps		xmm1, xmm6

		movss		xmm7, [esi+eax+3*12+0]
		movhps		xmm7, [esi+eax+3*12+4]
		minps		xmm2, xmm7
		maxps		xmm3, xmm7

		add			eax, 4*12
		jl			loop4

	done4:
		mov			eax, count
		and			eax, 3
		jz			done1
		imul		eax, 12
		add			esi, eax
		neg			eax

	loop1:
		movss		xmm4, [esi+eax+0*12+8]
		movhps		xmm4, [esi+eax+0*12+0]
		minps		xmm0, xmm4
		maxps		xmm1, xmm4

		add			eax, 12
		jl			loop1

	done1:
		shufps		xmm2, xmm2, R_SHUFFLEPS(3, 1, 0, 2)
		shufps		xmm3, xmm3, R_SHUFFLEPS(3, 1, 0, 2)
		minps		xmm0, xmm2
		maxps		xmm1, xmm3
		mov			esi, min
		movhps		[esi], xmm0
		movss		[esi+8], xmm0
		mov			edi, max
		movhps		[edi], xmm1
		movss		[edi+8], xmm1
	}
}

/*
============
xSIMD_SSE::MinMax
============
*/
void VPCALL xSIMD_SSE::MinMax(xVec3 &min, xVec3 &max, const xDrawVert *src, const int count) {

	assert(sizeof(xDrawVert) == DRAWVERT_SIZE);
	assert((int)&((xDrawVert *)0)->xyz == DRAWVERT_XYZ_OFFSET);

	__asm {

		movss		xmm0, xMath::INFINITY
		xorps		xmm1, xmm1
		shufps		xmm0, xmm0, R_SHUFFLEPS(0, 0, 0, 0)
		subps		xmm1, xmm0
		movaps		xmm2, xmm0
		movaps		xmm3, xmm1

		mov			esi, src
		mov			eax, count
		and			eax, ~3
		jz			done4
		imul		eax, DRAWVERT_SIZE
		add			esi, eax
		neg			eax

	loop4:
//		prefetchnta	[esi+4*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET]

		movss		xmm4, [esi+eax+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+8]
		movhps		xmm4, [esi+eax+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]
		minps		xmm0, xmm4
		maxps		xmm1, xmm4

		movss		xmm5, [esi+eax+1*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]
		movhps		xmm5, [esi+eax+1*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+4]
		minps		xmm2, xmm5
		maxps		xmm3, xmm5

		movss		xmm6, [esi+eax+2*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+8]
		movhps		xmm6, [esi+eax+2*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]
		minps		xmm0, xmm6
		maxps		xmm1, xmm6

		movss		xmm7, [esi+eax+3*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]
		movhps		xmm7, [esi+eax+3*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+4]
		minps		xmm2, xmm7
		maxps		xmm3, xmm7

		add			eax, 4*DRAWVERT_SIZE
		jl			loop4

	done4:
		mov			eax, count
		and			eax, 3
		jz			done1
		imul		eax, DRAWVERT_SIZE
		add			esi, eax
		neg			eax

	loop1:
		movss		xmm4, [esi+eax+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+8]
		movhps		xmm4, [esi+eax+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]
		minps		xmm0, xmm4
		maxps		xmm1, xmm4

		add			eax, DRAWVERT_SIZE
		jl			loop1

	done1:
		shufps		xmm2, xmm2, R_SHUFFLEPS(3, 1, 0, 2)
		shufps		xmm3, xmm3, R_SHUFFLEPS(3, 1, 0, 2)
		minps		xmm0, xmm2
		maxps		xmm1, xmm3
		mov			esi, min
		movhps		[esi], xmm0
		movss		[esi+8], xmm0
		mov			edi, max
		movhps		[edi], xmm1
		movss		[edi+8], xmm1
	}
}

/*
============
xSIMD_SSE::MinMax
============
*/
void VPCALL xSIMD_SSE::MinMax(xVec3 &min, xVec3 &max, const xDrawVert *src, const int *indexes, const int count) {

	assert(sizeof(xDrawVert) == DRAWVERT_SIZE);
	assert((int)&((xDrawVert *)0)->xyz == DRAWVERT_XYZ_OFFSET);

	__asm {

		movss		xmm0, xMath::INFINITY
		xorps		xmm1, xmm1
		shufps		xmm0, xmm0, R_SHUFFLEPS(0, 0, 0, 0)
		subps		xmm1, xmm0
		movaps		xmm2, xmm0
		movaps		xmm3, xmm1

		mov			edi, indexes
		mov			esi, src
		mov			eax, count
		and			eax, ~3
		jz			done4
		shl			eax, 2
		add			edi, eax
		neg			eax

	loop4:
//		prefetchnta	[edi+128]
//		prefetchnta	[esi+4*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET]

		mov			edx, [edi+eax+0]
		imul		edx, DRAWVERT_SIZE
		movss		xmm4, [esi+edx+DRAWVERT_XYZ_OFFSET+8]
		movhps		xmm4, [esi+edx+DRAWVERT_XYZ_OFFSET+0]
		minps		xmm0, xmm4
		maxps		xmm1, xmm4

		mov			edx, [edi+eax+4]
		imul		edx, DRAWVERT_SIZE
		movss		xmm5, [esi+edx+DRAWVERT_XYZ_OFFSET+0]
		movhps		xmm5, [esi+edx+DRAWVERT_XYZ_OFFSET+4]
		minps		xmm2, xmm5
		maxps		xmm3, xmm5

		mov			edx, [edi+eax+8]
		imul		edx, DRAWVERT_SIZE
		movss		xmm6, [esi+edx+DRAWVERT_XYZ_OFFSET+8]
		movhps		xmm6, [esi+edx+DRAWVERT_XYZ_OFFSET+0]
		minps		xmm0, xmm6
		maxps		xmm1, xmm6

		mov			edx, [edi+eax+12]
		imul		edx, DRAWVERT_SIZE
		movss		xmm7, [esi+edx+DRAWVERT_XYZ_OFFSET+0]
		movhps		xmm7, [esi+edx+DRAWVERT_XYZ_OFFSET+4]
		minps		xmm2, xmm7
		maxps		xmm3, xmm7

		add			eax, 4*4
		jl			loop4

	done4:
		mov			eax, count
		and			eax, 3
		jz			done1
		shl			eax, 2
		add			edi, eax
		neg			eax

	loop1:
		mov			edx, [edi+eax+0]
		imul		edx, DRAWVERT_SIZE;
		movss		xmm4, [esi+edx+DRAWVERT_XYZ_OFFSET+8]
		movhps		xmm4, [esi+edx+DRAWVERT_XYZ_OFFSET+0]
		minps		xmm0, xmm4
		maxps		xmm1, xmm4

		add			eax, 4
		jl			loop1

	done1:
		shufps		xmm2, xmm2, R_SHUFFLEPS(3, 1, 0, 2)
		shufps		xmm3, xmm3, R_SHUFFLEPS(3, 1, 0, 2)
		minps		xmm0, xmm2
		maxps		xmm1, xmm3
		mov			esi, min
		movhps		[esi], xmm0
		movss		[esi+8], xmm0
		mov			edi, max
		movhps		[edi], xmm1
		movss		[edi+8], xmm1
	}
}

/*
============
xSIMD_SSE::Clamp
============
*/
void VPCALL xSIMD_SSE::Clamp(float *dst, float *src, float min, float max, const int count) {
	int	i, pre, post;

	__asm
	{
		movss	xmm0,min
		movss	xmm1,max
		shufps	xmm0,xmm0,0
		shufps	xmm1,xmm1,0

		KFLOATINITDS(dst, src, count, pre, post)
		and		eax,15
		jne		lpNA
		jmp		lpA
		align	16
lpA:
		movaps	xmm2,[edx+ebx]
		movaps	xmm3,[edx+ebx+16]
		maxps	xmm2,xmm0
		maxps	xmm3,xmm0
		prefetchnta	[edx+ebx+64]
		minps	xmm2,xmm1
		minps	xmm3,xmm1
		movaps	[edi+ebx],xmm2
		movaps	[edi+ebx+16],xmm3
		add		ebx,16*2
		jl		lpA
		jmp		done

		align	16
lpNA:
		movups	xmm2,[edx+ebx]
		movups	xmm3,[edx+ebx+16]
		maxps	xmm2,xmm0
		maxps	xmm3,xmm0
		prefetchnta	[edx+ebx+64]
		minps	xmm2,xmm1
		minps	xmm3,xmm1
		movaps	[edi+ebx],xmm2
		movaps	[edi+ebx+16],xmm3
		add		ebx,16*2
		jl		lpNA
done:
	}

	for (i = 0; i < pre; i++) {
		if (src[i] < min)
			dst[i] = min;
		else if (src[i] > max)
			dst[i] = max;
		else
			dst[i] = src[i];
	}

	for(i = count - post; i < count; i++) {
		if (src[i] < min)
			dst[i] = min;
		else if (src[i] > max)
			dst[i] = max;
		else
			dst[i] = src[i];
	}
}

/*
============
xSIMD_SSE::ClampMin
============
*/
void VPCALL xSIMD_SSE::ClampMin(float *dst, float *src, float min, const int count) {
	int	i, pre, post;

	__asm
	{
		movss	xmm0,min
		shufps	xmm0,xmm0,0

		KFLOATINITDS(dst, src, count, pre, post)
		and		eax,15
		jne		lpNA
		jmp		lpA
		align	16
lpA:
		movaps	xmm2,[edx+ebx]
		movaps	xmm3,[edx+ebx+16]
		maxps	xmm2,xmm0
		prefetchnta	[edx+ebx+64]
		maxps	xmm3,xmm0
		movaps	[edi+ebx],xmm2
		movaps	[edi+ebx+16],xmm3
		add		ebx,16*2
		jl		lpA
		jmp		done

		align	16
lpNA:
		movups	xmm2,[edx+ebx]
		movups	xmm3,[edx+ebx+16]
		maxps	xmm2,xmm0
		prefetchnta	[edx+ebx+64]
		maxps	xmm3,xmm0
		movaps	[edi+ebx],xmm2
		movaps	[edi+ebx+16],xmm3
		add		ebx,16*2
		jl		lpNA
done:
	}

	for(i = 0; i < pre; i++) {
		if (src[i] < min)
			dst[i] = min;
		else
			dst[i] = src[i];
	}
	for(i = count - post; i < count; i++) {
		if (src[i] < min)
			dst[i] = min;
		else
			dst[i] = src[i];
	}
}

/*
============
xSIMD_SSE::ClampMax
============
*/
void VPCALL xSIMD_SSE::ClampMax(float *dst, float *src, float max, const int count) {
	int	i, pre, post;

	__asm
	{
		movss	xmm1,max
		shufps	xmm1,xmm1,0

		KFLOATINITDS(dst, src, count, pre, post)
		and		eax,15
		jne		lpNA
		jmp		lpA
		align	16
lpA:
		movaps	xmm2,[edx+ebx]
		movaps	xmm3,[edx+ebx+16]
		minps	xmm2,xmm1
		prefetchnta	[edx+ebx+64]
		minps	xmm3,xmm1
		movaps	[edi+ebx],xmm2
		movaps	[edi+ebx+16],xmm3
		add		ebx,16*2
		jl		lpA
		jmp		done

		align	16
lpNA:
		movups	xmm2,[edx+ebx]
		movups	xmm3,[edx+ebx+16]
		minps	xmm2,xmm1
		prefetchnta	[edx+ebx+64]
		minps	xmm3,xmm1
		movaps	[edi+ebx],xmm2
		movaps	[edi+ebx+16],xmm3
		add		ebx,16*2
		jl		lpNA
done:
	}

	for(i = 0; i < pre; i++) {
		if (src[i] > max)
			dst[i] = max;
		else
			dst[i] = src[i];
	}

	for(i = count - post; i < count; i++) {
		if (src[i] > max)
			dst[i] = max;
		else
			dst[i] = src[i];
	}
}

/*
============
xSIMD_SSE::Zero16
============
*/
void VPCALL xSIMD_SSE::Zero16(float *dst, const int count) {
	__asm {
		mov		edx, dst
		mov		eax, count
		add		eax, 3
		shr		eax, 2
		jz		doneZero16
		shl		eax, 4
		add		edx, eax
		neg		eax
		xorps	xmm0, xmm0
	loopZero16:
		movaps	[edx+eax], xmm0
		add		eax, 16
		jl		loopZero16
	doneZero16:
	}
}

/*
============
xSIMD_SSE::Negate16
============
*/
void VPCALL xSIMD_SSE::Negate16(float *dst, const int count) {
	__asm {
		mov		edx, dst
		mov		eax, count
		add		eax, 3
		shr		eax, 2
		jz		doneNegate16
		shl		eax, 4
		add		edx, eax
		neg		eax
		movss	xmm0, SIMD_SP_signBitMask
		shufps	xmm0, xmm0, R_SHUFFLEPS(0, 0, 0, 0)
	loopNegate16:
		movaps	xmm1, [edx+eax]
		xorps	xmm1, xmm0
		movaps	[edx+eax], xmm1
		add		eax, 16
		jl		loopNegate16
	doneNegate16:
	}
}

/*
============
xSIMD_SSE::Copy16
============
*/
void VPCALL xSIMD_SSE::Copy16(float *dst, float *src, const int count) {
	__asm {
		mov		ecx, src
		mov		edx, dst
		mov		eax, count
		add		eax, 3
		shr		eax, 2
		jz		doneCopy16
		shl		eax, 4
		add		ecx, eax
		add		edx, eax
		neg		eax
	loopCopy16:
		movaps	xmm0, [ecx+eax]
		movaps	[edx+eax], xmm0
		add		eax, 16
		jl		loopCopy16
	doneCopy16:
	}
}

/*
============
xSIMD_SSE::Add16
============
*/
void VPCALL xSIMD_SSE::Add16(float *dst, float *src1, float *src2, const int count) {
	__asm {
		mov		ecx, src1
		mov		edx, src2
		mov		esi, dst
		mov		eax, count
		add		eax, 3
		shr		eax, 2
		jz		doneAdd16
		shl		eax, 4
		add		esi, eax
		add		ecx, eax
		add		edx, eax
		neg		eax
	loopAdd16:
		movaps	xmm0, [ecx+eax]
		addps	xmm0, [edx+eax]
		movaps	[esi+eax], xmm0
		add		eax, 16
		jl		loopAdd16
	doneAdd16:
	}
}

/*
============
xSIMD_SSE::Sub16
============
*/
void VPCALL xSIMD_SSE::Sub16(float *dst, float *src1, float *src2, const int count) {
	__asm {
		mov		ecx, src1
		mov		edx, src2
		mov		esi, dst
		mov		eax, count
		add		eax, 3
		shr		eax, 2
		jz		doneSub16
		shl		eax, 4
		add		esi, eax
		add		ecx, eax
		add		edx, eax
		neg		eax
	loopSub16:
		movaps	xmm0, [ecx+eax]
		subps	xmm0, [edx+eax]
		movaps	[esi+eax], xmm0
		add		eax, 16
		jl		loopSub16
	doneSub16:
	}
}

/*
============
xSIMD_SSE::Mul16
============
*/
void VPCALL xSIMD_SSE::Mul16(float *dst, float *src1, float constant, const int count) {
	__asm {
		mov		ecx, dst
		mov		edx, src1
		mov		eax, count
		add		eax, 3
		shr		eax, 2
		jz		doneMulScalar16
		movss	xmm1, constant
		shl		eax, 4
		add		ecx, eax
		add		edx, eax
		neg		eax
		shufps	xmm1, xmm1, 0x00
	loopMulScalar16:
		movaps	xmm0, [edx+eax]
		mulps	xmm0, xmm1
		movaps	[ecx+eax], xmm0
		add		eax, 16
		jl		loopMulScalar16
	doneMulScalar16:
	}
}

/*
============
xSIMD_SSE::AddAssign16
============
*/
void VPCALL xSIMD_SSE::AddAssign16(float *dst, float *src, const int count) {
	__asm {
		mov		ecx, dst
		mov		edx, src
		mov		eax, count
		add		eax, 3
		shr		eax, 2
		jz		doneAddAssign16
		shl		eax, 4
		add		ecx, eax
		add		edx, eax
		neg		eax
	loopAddAssign16:
		movaps	xmm0, [ecx+eax]
		addps	xmm0, [edx+eax]
		movaps	[ecx+eax], xmm0
		add		eax, 16
		jl		loopAddAssign16
	doneAddAssign16:
	}
}

/*
============
xSIMD_SSE::SubAssign16
============
*/
void VPCALL xSIMD_SSE::SubAssign16(float *dst, float *src, const int count) {
	__asm {
		mov		ecx, dst
		mov		edx, src
		mov		eax, count
		add		eax, 3
		shr		eax, 2
		jz		doneSubAssign16
		shl		eax, 4
		add		ecx, eax
		add		edx, eax
		neg		eax
	loopSubAssign16:
		movaps	xmm0, [ecx+eax]
		subps	xmm0, [edx+eax]
		movaps	[ecx+eax], xmm0
		add		eax, 16
		jl		loopSubAssign16
	doneSubAssign16:
	}
}

/*
============
xSIMD_SSE::MulAssign16
============
*/
void VPCALL xSIMD_SSE::MulAssign16(float *dst, float constant, const int count) {
	__asm {
		mov		ecx, dst
		mov		eax, count
		add		eax, 3
		shr		eax, 2
		jz		doneMulAssign16
		movss	xmm1, constant
		shl		eax, 4
		add		ecx, eax
		neg		eax
		shufps	xmm1, xmm1, 0x00
	loopMulAssign16:
		movaps	xmm0, [ecx+eax]
		mulps	xmm0, xmm1
		movaps	[ecx+eax], xmm0
		add		eax, 16
		jl		loopMulAssign16
	doneMulAssign16:
	}
}

/*
============
xSIMD_SSE::TracePointCull
============
*/
void VPCALL xSIMD_SSE::TracePointCull(byte *cullBits, byte &totalOr, float radius, const xPlane *planes, const xDrawVert *verts, const int numVerts) {
#if 1

	assert(sizeof(xDrawVert) == DRAWVERT_SIZE);
	assert((int)&((xDrawVert *)0)->xyz == DRAWVERT_XYZ_OFFSET);

	__asm {
		push		ebx
		mov			eax, numVerts
		test		eax, eax
		jz			done

		mov			edi, planes
		movlps		xmm1, [edi]								// xmm1 =  0,  1,  X,  X
		movhps		xmm1, [edi+16]							// xmm1 =  0,  1,  4,  5
		movlps		xmm3, [edi+8]							// xmm3 =  2,  3,  X,  X
		movhps		xmm3, [edi+24]							// xmm3 =  2,  3,  6,  7
		movlps		xmm4, [edi+32]							// xmm4 =  8,  9,  X,  X
		movhps		xmm4, [edi+48]							// xmm4 =  8,  9, 12, 13
		movlps		xmm5, [edi+40]							// xmm5 = 10, 11,  X,  X
		movhps		xmm5, [edi+56]							// xmm5 = 10, 11, 14, 15
		movaps		xmm0, xmm1								// xmm0 =  0,  1,  4,  5
		shufps		xmm0, xmm4, R_SHUFFLEPS(0, 2, 0, 2)	// xmm0 =  0,  4,  8, 12
		shufps		xmm1, xmm4, R_SHUFFLEPS(1, 3, 1, 3)	// xmm1 =  1,  5,  9, 13
		movaps		xmm2, xmm3								// xmm2 =  2,  3,  6,  7
		shufps		xmm2, xmm5, R_SHUFFLEPS(0, 2, 0, 2)	// xmm2 =  2,  6, 10, 14
		shufps		xmm3, xmm5, R_SHUFFLEPS(1, 3, 1, 3)	// xmm3 =  3,  7, 11, 15
		movss		xmm7, radius
		shufps		xmm7, xmm7, R_SHUFFLEPS(0, 0, 0, 0)

		xor			edx, edx
		mov			esi, verts
		mov			edi, cullBits
		imul		eax, DRAWVERT_SIZE
		add			esi, eax
		neg			eax

	loopVert:
		movss		xmm4, [esi+eax+DRAWVERT_XYZ_OFFSET+0]
		shufps		xmm4, xmm4, R_SHUFFLEPS(0, 0, 0, 0)
		movss		xmm5, [esi+eax+DRAWVERT_XYZ_OFFSET+4]
		mulps		xmm4, xmm0
		shufps		xmm5, xmm5, R_SHUFFLEPS(0, 0, 0, 0)
		movss		xmm6, [esi+eax+DRAWVERT_XYZ_OFFSET+8]
		mulps		xmm5, xmm1
		shufps		xmm6, xmm6, R_SHUFFLEPS(0, 0, 0, 0)
		addps		xmm4, xmm5
		mulps		xmm6, xmm2
		addps		xmm4, xmm3
		addps		xmm4, xmm6
		movaps		xmm5, xmm4
		xorps		xmm5, SIMD_SP_signBitMask
		cmpltps		xmm4, xmm7
		movmskps	ecx, xmm4
		cmpltps		xmm5, xmm7
		movmskps	ebx, xmm5
		shl			cx, 4
		or			cl, bl
		inc			edi
		or			dl, cl
		add			eax, DRAWVERT_SIZE
		mov			byte ptr [edi-1], cl
		jl			loopVert

	done:
		mov			esi, totalOr
        mov			byte ptr [esi], dl
		pop			ebx
	}

#else

	int i;
	byte tOr;

	tOr = 0;

	for (i = 0; i < numVerts; i++) {
		byte bits;
		float d0, d1, d2, d3, t;
		const xVec3 &v = verts[i].xyz;

		d0 = planes[0][0] * v[0] + planes[0][1] * v[1] + planes[0][2] * v[2] + planes[0][3];
		d1 = planes[1][0] * v[0] + planes[1][1] * v[1] + planes[1][2] * v[2] + planes[1][3];
		d2 = planes[2][0] * v[0] + planes[2][1] * v[1] + planes[2][2] * v[2] + planes[2][3];
		d3 = planes[3][0] * v[0] + planes[3][1] * v[1] + planes[3][2] * v[2] + planes[3][3];

		t = d0 + radius;
		bits  = FLOATSIGNBITSET(t) << 0;
		t = d1 + radius;
		bits |= FLOATSIGNBITSET(t) << 1;
		t = d2 + radius;
		bits |= FLOATSIGNBITSET(t) << 2;
		t = d3 + radius;
		bits |= FLOATSIGNBITSET(t) << 3;

		t = d0 - radius;
		bits |= FLOATSIGNBITSET(t) << 4;
		t = d1 - radius;
		bits |= FLOATSIGNBITSET(t) << 5;
		t = d2 - radius;
		bits |= FLOATSIGNBITSET(t) << 6;
		t = d3 - radius;
		bits |= FLOATSIGNBITSET(t) << 7;

		bits ^= 0x0F;		// flip lower four bits

		tOr |= bits;
		cullBits[i] = bits;
	}

	totalOr = tOr;

#endif
}

/*
============
xSIMD_SSE::DecalPointCull
============
*/
void VPCALL xSIMD_SSE::DecalPointCull(byte *cullBits, const xPlane *planes, const xDrawVert *verts, const int numVerts) {
#if 1

	ALIGN16(float p0[4];)
	ALIGN16(float p1[4];)
	ALIGN16(float p2[4];)
	ALIGN16(float p3[4];)
	ALIGN16(float p4[4];)
	ALIGN16(float p5[4];)
	ALIGN16(float p6[4];)
	ALIGN16(float p7[4];)

	assert(sizeof(xDrawVert) == DRAWVERT_SIZE);
	assert((int)&((xDrawVert *)0)->xyz == DRAWVERT_XYZ_OFFSET);

	__asm {
		mov			ecx, planes
		movlps		xmm1, [ecx]								// xmm1 =  0,  1,  X,  X
		movhps		xmm1, [ecx+16]							// xmm1 =  0,  1,  4,  5
		movlps		xmm3, [ecx+8]							// xmm3 =  2,  3,  X,  X
		movhps		xmm3, [ecx+24]							// xmm3 =  2,  3,  6,  7
		movlps		xmm4, [ecx+32]							// xmm4 =  8,  9,  X,  X
		movhps		xmm4, [ecx+48]							// xmm4 =  8,  9, 12, 13
		movlps		xmm5, [ecx+40]							// xmm5 = 10, 11,  X,  X
		movhps		xmm5, [ecx+56]							// xmm5 = 10, 11, 14, 15
		movaps		xmm0, xmm1								// xmm0 =  0,  1,  4,  5
		shufps		xmm0, xmm4, R_SHUFFLEPS(0, 2, 0, 2)	// xmm0 =  0,  4,  8, 12
		shufps		xmm1, xmm4, R_SHUFFLEPS(1, 3, 1, 3)	// xmm1 =  1,  5,  9, 13
		movaps		xmm2, xmm3								// xmm2 =  2,  3,  6,  7
		shufps		xmm2, xmm5, R_SHUFFLEPS(0, 2, 0, 2)	// xmm2 =  2,  6, 10, 14
		shufps		xmm3, xmm5, R_SHUFFLEPS(1, 3, 1, 3)	// xmm3 =  3,  7, 11, 15

		movaps		p0, xmm0
		movaps		p1, xmm1
		movaps		p2, xmm2
		movaps		p3, xmm3

		movlps		xmm4, [ecx+64]							// xmm4 = p40, p41,   X,   X
		movhps		xmm4, [ecx+80]							// xmm4 = p40, p41, p50, p51
		movaps		xmm5, xmm4								// xmm5 = p40, p41, p50, p51
		shufps		xmm4, xmm4, R_SHUFFLEPS(0, 2, 0, 2)	// xmm4 = p40, p50, p40, p50
		shufps		xmm5, xmm5, R_SHUFFLEPS(1, 3, 1, 3)	// xmm5 = p41, p51, p41, p51
		movlps		xmm6, [ecx+72]							// xmm6 = p42, p43,   X,   X
		movhps		xmm6, [ecx+88]							// xmm6 = p42, p43, p52, p53
		movaps		xmm7, xmm6								// xmm7 = p42, p43, p52, p53
		shufps		xmm6, xmm6, R_SHUFFLEPS(0, 2, 0, 2)	// xmm6 = p42, p52, p42, p52
		shufps		xmm7, xmm7, R_SHUFFLEPS(1, 3, 1, 3)	// xmm7 = p43, p53, p43, p53

		movaps		p4, xmm4
		movaps		p5, xmm5
		movaps		p6, xmm6
		movaps		p7, xmm7

		mov			esi, verts
		mov			edi, cullBits
		mov			eax, numVerts
		and			eax, ~1
		jz			done2
		imul		eax, DRAWVERT_SIZE
		add			esi, eax
		neg			eax

	loopVert2:
		movaps		xmm6, p0
		movss		xmm0, [esi+eax+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]
		shufps		xmm0, xmm0, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm6, xmm0
		movaps		xmm7, p1
		movss		xmm1, [esi+eax+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+4]
		shufps		xmm1, xmm1, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm7, xmm1
		addps		xmm6, xmm7
		movaps		xmm7, p2
		movss		xmm2, [esi+eax+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+8]
		shufps		xmm2, xmm2, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm7, xmm2
		addps		xmm6, xmm7
		addps		xmm6, p3

		cmpnltps	xmm6, SIMD_SP_zero
		movmskps	ecx, xmm6
			
		movaps		xmm6, p0
		movss		xmm3, [esi+eax+1*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]
		shufps		xmm3, xmm3, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm6, xmm3
		movaps		xmm7, p1
		movss		xmm4, [esi+eax+1*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+4]
		shufps		xmm4, xmm4, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm7, xmm4
		addps		xmm6, xmm7
		movaps		xmm7, p2
		movss		xmm5, [esi+eax+1*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+8]
		shufps		xmm5, xmm5, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm7, xmm5
		addps		xmm6, xmm7
		addps		xmm6, p3

		cmpnltps	xmm6, SIMD_SP_zero
		movmskps	edx, xmm6
		mov			ch, dl

		shufps		xmm0, xmm3, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm0, p4
		shufps		xmm1, xmm4, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm1, p5
		addps		xmm0, xmm1
		shufps		xmm2, xmm5, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm2, p6
		addps		xmm0, xmm2
		addps		xmm0, p7

		cmpnltps	xmm0, SIMD_SP_zero
		movmskps	edx, xmm0

		add			edi, 2

		mov			dh, dl
		shl			dl, 4
		shl			dh, 2
		and			edx, (3<<4)|(3<<12)
		or			ecx, edx

		add			eax, 2*DRAWVERT_SIZE
		mov			word ptr [edi-2], cx
		jl			loopVert2

	done2:

		mov			eax, numVerts
		and			eax, 1
		jz			done

		movaps		xmm6, p0
		movss		xmm0, [esi+DRAWVERT_XYZ_OFFSET+0]
		shufps		xmm0, xmm0, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm6, xmm0
		movaps		xmm7, p1
		movss		xmm1, [esi+DRAWVERT_XYZ_OFFSET+4]
		shufps		xmm1, xmm1, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm7, xmm1
		addps		xmm6, xmm7
		movaps		xmm7, p2
		movss		xmm2, [esi+DRAWVERT_XYZ_OFFSET+8]
		shufps		xmm2, xmm2, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm7, xmm2
		addps		xmm6, xmm7
		addps		xmm6, p3

		cmpnltps	xmm6, SIMD_SP_zero
		movmskps	ecx, xmm6

		mulps		xmm0, p4
		mulps		xmm1, p5
		addps		xmm0, xmm1
		mulps		xmm2, p6
		addps		xmm0, xmm2
		addps		xmm0, p7

		cmpnltps	xmm0, SIMD_SP_zero
		movmskps	edx, xmm0

		and			edx, 3
		shl			edx, 4
		or			ecx, edx

		mov			byte ptr [edi], cl

	done:
	}


#else

	int i;

	for (i = 0; i < numVerts; i += 2) {
		unsigned short bits0, bits1;
		float d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11;
		const xVec3 &v0 = verts[i+0].xyz;
		const xVec3 &v1 = verts[i+1].xyz;

		d0  = planes[0][0] * v0[0] + planes[0][1] * v0[1] + planes[0][2] * v0[2] + planes[0][3];
		d1  = planes[1][0] * v0[0] + planes[1][1] * v0[1] + planes[1][2] * v0[2] + planes[1][3];
		d2  = planes[2][0] * v0[0] + planes[2][1] * v0[1] + planes[2][2] * v0[2] + planes[2][3];
		d3  = planes[3][0] * v0[0] + planes[3][1] * v0[1] + planes[3][2] * v0[2] + planes[3][3];

		d4  = planes[4][0] * v0[0] + planes[4][1] * v0[1] + planes[4][2] * v0[2] + planes[4][3];
		d5  = planes[5][0] * v0[0] + planes[5][1] * v0[1] + planes[5][2] * v0[2] + planes[5][3];
		d10 = planes[4][0] * v1[0] + planes[4][1] * v1[1] + planes[4][2] * v1[2] + planes[4][3];
		d11 = planes[5][0] * v1[0] + planes[5][1] * v1[1] + planes[5][2] * v1[2] + planes[5][3];

		d6  = planes[0][0] * v1[0] + planes[0][1] * v1[1] + planes[0][2] * v1[2] + planes[0][3];
		d7  = planes[1][0] * v1[0] + planes[1][1] * v1[1] + planes[1][2] * v1[2] + planes[1][3];
		d8  = planes[2][0] * v1[0] + planes[2][1] * v1[1] + planes[2][2] * v1[2] + planes[2][3];
		d9  = planes[3][0] * v1[0] + planes[3][1] * v1[1] + planes[3][2] * v1[2] + planes[3][3];

		bits0  = FLOATSIGNBITSET(d0) << (0+0);
		bits0 |= FLOATSIGNBITSET(d1) << (0+1);
		bits0 |= FLOATSIGNBITSET(d2) << (0+2);
		bits0 |= FLOATSIGNBITSET(d3) << (0+3);
		bits0 |= FLOATSIGNBITSET(d4) << (0+4);
		bits0 |= FLOATSIGNBITSET(d5) << (0+5);

		bits1  = FLOATSIGNBITSET(d6) << (8+0);
		bits1 |= FLOATSIGNBITSET(d7) << (8+1);
		bits1 |= FLOATSIGNBITSET(d8) << (8+2);
		bits1 |= FLOATSIGNBITSET(d9) << (8+3);
		bits1 |= FLOATSIGNBITSET(d10) << (8+4);
		bits1 |= FLOATSIGNBITSET(d11) << (8+5);

		*(unsigned short *)(cullBits + i) = (bits0 | bits1) ^ 0x3F3F;
	}

	if (numVerts & 1) {
		byte bits;
		float d0, d1, d2, d3, d4, d5;
		const xVec3 &v = verts[numVerts - 1].xyz;

		d0 = planes[0][0] * v[0] + planes[0][1] * v[1] + planes[0][2] * v[2] + planes[0][3];
		d1 = planes[1][0] * v[0] + planes[1][1] * v[1] + planes[1][2] * v[2] + planes[1][3];
		d2 = planes[2][0] * v[0] + planes[2][1] * v[1] + planes[2][2] * v[2] + planes[2][3];
		d3 = planes[3][0] * v[0] + planes[3][1] * v[1] + planes[3][2] * v[2] + planes[3][3];

		d4 = planes[4][0] * v[0] + planes[4][1] * v[1] + planes[4][2] * v[2] + planes[4][3];
		d5 = planes[5][0] * v[0] + planes[5][1] * v[1] + planes[5][2] * v[2] + planes[5][3];

		bits  = FLOATSIGNBITSET(d0) << 0;
		bits |= FLOATSIGNBITSET(d1) << 1;
		bits |= FLOATSIGNBITSET(d2) << 2;
		bits |= FLOATSIGNBITSET(d3) << 3;

		bits |= FLOATSIGNBITSET(d4) << 4;
		bits |= FLOATSIGNBITSET(d5) << 5;

		cullBits[numVerts - 1] = bits ^ 0x3F;		// flip lower 6 bits
	}

#endif
}

/*
============
xSIMD_SSE::OverlayPointCull
============
*/
void VPCALL xSIMD_SSE::OverlayPointCull(byte *cullBits, xVec2 *texCoords, const xPlane *planes, const xDrawVert *verts, const int numVerts) {
#if 1

	assert(sizeof(xDrawVert) == DRAWVERT_SIZE);
	assert((int)&((xDrawVert *)0)->xyz == DRAWVERT_XYZ_OFFSET);

	__asm {
		mov			eax, numVerts
		mov			edx, verts
		mov			esi, texCoords
		mov			edi, cullBits

		mov			ecx, planes
		movss		xmm4, [ecx+ 0]
		movss		xmm5, [ecx+16]
		shufps		xmm4, xmm5, R_SHUFFLEPS(0, 0, 0, 0)
		shufps		xmm4, xmm4, R_SHUFFLEPS(0, 2, 0, 2)
		movss		xmm5, [ecx+ 4]
		movss		xmm6, [ecx+20]
		shufps		xmm5, xmm6, R_SHUFFLEPS(0, 0, 0, 0)
		shufps		xmm5, xmm5, R_SHUFFLEPS(0, 2, 0, 2)
		movss		xmm6, [ecx+ 8]
		movss		xmm7, [ecx+24]
		shufps		xmm6, xmm7, R_SHUFFLEPS(0, 0, 0, 0)
		shufps		xmm6, xmm6, R_SHUFFLEPS(0, 2, 0, 2)
		movss		xmm7, [ecx+12]
		movss		xmm0, [ecx+28]
		shufps		xmm7, xmm0, R_SHUFFLEPS(0, 0, 0, 0)
		shufps		xmm7, xmm7, R_SHUFFLEPS(0, 2, 0, 2)

		and			eax, ~1
		jz			done2
		add			edi, eax
		neg			eax

	loopVert2:
		movss		xmm0, [edx+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]
		movss		xmm1, [edx+1*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]
		shufps		xmm0, xmm1, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm0, xmm4
		movss		xmm1, [edx+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+4]
		movss		xmm2, [edx+1*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+4]
		shufps		xmm1, xmm2, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm1, xmm5
		movss		xmm2, [edx+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+8]
		movss		xmm3, [edx+1*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+8]
		shufps		xmm2, xmm3, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm2, xmm6
		addps		xmm0, xmm1
		addps		xmm0, xmm2
		addps		xmm0, xmm7
		movaps		[esi], xmm0
		movaps		xmm1, xmm0
		movaps		xmm2, SIMD_SP_one
		subps		xmm2, xmm0
		shufps		xmm0, xmm2, R_SHUFFLEPS(0, 1, 0, 1)
		shufps		xmm1, xmm2, R_SHUFFLEPS(2, 3, 2, 3)
		add			edx, 2*DRAWVERT_SIZE
		movmskps	ecx, xmm0
		mov			byte ptr [edi+eax+0], cl
		add			esi, 4*4
		movmskps	ecx, xmm1
		mov			byte ptr [edi+eax+1], cl
		add			eax, 2
		jl			loopVert2

	done2:
		mov			eax, numVerts
		and			eax, 1
		jz			done

		movss		xmm0, [edx+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+0]
		shufps		xmm0, xmm0, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm0, xmm4
		movss		xmm1, [edx+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+4]
		shufps		xmm1, xmm1, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm1, xmm5
		movss		xmm2, [edx+0*DRAWVERT_SIZE+DRAWVERT_XYZ_OFFSET+8]
		shufps		xmm2, xmm2, R_SHUFFLEPS(0, 0, 0, 0)
		mulps		xmm2, xmm6
		addps		xmm0, xmm1
		addps		xmm0, xmm2
		addps		xmm0, xmm7
		movlps		[esi], xmm0
		movaps		xmm1, xmm0
		movaps		xmm2, SIMD_SP_one
		subps		xmm2, xmm0
		shufps		xmm0, xmm2, R_SHUFFLEPS(0, 1, 0, 1)
		movmskps	ecx, xmm0
		mov			byte ptr [edi], cl

	done:
	}

#else

	const xPlane &p0 = planes[0];
	const xPlane &p1 = planes[1];

	for (int i = 0; i < numVerts - 1; i += 2) {
		unsigned short bits;
		float d0, d1, d2, d3;

		const xVec3 &v0 = verts[i+0].xyz;
		const xVec3 &v1 = verts[i+1].xyz;

		d0 = p0[0] * v0[0] + p0[1] * v0[1] + p0[2] * v0[2] + p0[3];
		d1 = p1[0] * v0[0] + p1[1] * v0[1] + p1[2] * v0[2] + p1[3];
		d2 = p0[0] * v1[0] + p0[1] * v1[1] + p0[2] * v1[2] + p0[3];
		d3 = p1[0] * v1[0] + p1[1] * v1[1] + p1[2] * v1[2] + p1[3];

		texCoords[i+0][0] = d0;
		texCoords[i+0][1] = d1;
		texCoords[i+1][0] = d2;
		texCoords[i+1][1] = d3;

		bits  = FLOATSIGNBITSET(d0) << 0;
		bits |= FLOATSIGNBITSET(d1) << 1;
		bits |= FLOATSIGNBITSET(d2) << 8;
		bits |= FLOATSIGNBITSET(d3) << 9;

		d0 = 1.0f - d0;
		d1 = 1.0f - d1;
		d2 = 1.0f - d2;
		d3 = 1.0f - d3;

		bits |= FLOATSIGNBITSET(d0) << 2;
		bits |= FLOATSIGNBITSET(d1) << 3;
		bits |= FLOATSIGNBITSET(d2) << 10;
		bits |= FLOATSIGNBITSET(d3) << 11;

		*(unsigned short *)(cullBits + i) = bits;
	}

	if (numVerts & 1) {
		byte bits;
		float d0, d1;

		const xPlane &p0 = planes[0];
		const xPlane &p1 = planes[1];
		const xVec3 &v0 = verts[numVerts - 1].xyz;

		d0 = p0[0] * v0[0] + p0[1] * v0[1] + p0[2] * v0[2] + p0[3];
		d1 = p1[0] * v0[0] + p1[1] * v0[1] + p1[2] * v0[2] + p1[3];

		texCoords[i][0] = d0;
		texCoords[i][1] = d1;

		bits  = FLOATSIGNBITSET(d0) << 0;
		bits |= FLOATSIGNBITSET(d1) << 1;

		d0 = 1.0f - d0;
		d1 = 1.0f - d1;

		bits |= FLOATSIGNBITSET(d0) << 2;
		bits |= FLOATSIGNBITSET(d1) << 3;

		cullBits[numVerts - 1] = bits;
	}

#endif
}

#endif /* _WIN32 */
