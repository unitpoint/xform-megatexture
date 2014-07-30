#ifndef __X_SIMD_H__
#define __X_SIMD_H__

#pragma once

/*
===============================================================================

	Single Instruction Multiple Data (SIMD)

	For optimal use data should be aligned on a 16 byte boundary.
	All xSIMDProcessor routines are thread safe.

===============================================================================
*/

#include "../common/xString.h"

class xSIMDProcessor;

class xSIMD
{
  static bool Initialized;

public:

  static xSIMDProcessor * Processor;

  static void Init();
	static void __cdecl Shutdown();

	static void InitProcessor(const xString& module, bool forceGeneric);
};


/*
===============================================================================

	virtual base class for different SIMD processors

===============================================================================
*/

#ifdef _WIN32
#define VPCALL __fastcall
#else
#define VPCALL
#endif

class xVec2;
class xVec3;
class xVec4;
class xVec5;
class xVec6;
class xVecX;
class xMat2;
class xMat3;
class xMat4;
class xMat5;
class xMat6;
class xMatX;
class xPlane;
class xDrawVert;

enum cpuid_t
{
	CPUID_NONE							= 0x00000,
	CPUID_UNSUPPORTED				= 0x00001,	// unsupported (386/486)
	CPUID_GENERIC						= 0x00002,	// unrecognized processor
	CPUID_INTEL							= 0x00004,	// Intel
	CPUID_AMD							  = 0x00008,	// AMD
	CPUID_MMX							  = 0x00010,	// Multi Media Extensions
	CPUID_3DNOW							= 0x00020,	// 3DNow!
	CPUID_SSE							  = 0x00040,	// Streaming SIMD Extensions
	CPUID_SSE2							= 0x00080,	// Streaming SIMD Extensions 2
	CPUID_SSE3							= 0x00100,	// Streaming SIMD Extentions 3 aka Prescott's New Instructions
	CPUID_ALTIVEC						= 0x00200,	// AltiVec
	CPUID_HTT							  = 0x01000,	// Hyper-Threading Technology
	CPUID_CMOV							= 0x02000,	// Conditional Move (CMOV) and fast floating point comparison (FCOMI) instructions
	CPUID_FTZ							  = 0x04000,	// Flush-To-Zero mode (denormal results are flushed to zero)
	CPUID_DAZ							  = 0x08000	  // Denormals-Are-Zero mode (denormal source operands are set to zero)
}; 

class xSIMDProcessor 
{
public:

  xSIMDProcessor() { cpuid = CPUID_NONE; }

	cpuid_t cpuid;

	virtual const TCHAR * VPCALL Name() const = 0;

	virtual void VPCALL Add(float *dst,			float constant,	float *src,		const int count) = 0;
	virtual void VPCALL Add(float *dst,			float *src0,		float *src1,		const int count) = 0;
	virtual void VPCALL Sub(float *dst,			float constant,	float *src,		const int count) = 0;
	virtual void VPCALL Sub(float *dst,			float *src0,		float *src1,		const int count) = 0;
	virtual void VPCALL Mul(float *dst,			float constant,	float *src,		const int count) = 0;
	virtual void VPCALL Mul(float *dst,			float *src0,		float *src1,		const int count) = 0;
	virtual void VPCALL Div(float *dst,			float constant,	float *src,		const int count) = 0;
	virtual void VPCALL Div(float *dst,			float *src0,		float *src1,		const int count) = 0;
	virtual void VPCALL MulAdd(float *dst,			float constant,	float *src,		const int count) = 0;
	virtual void VPCALL MulAdd(float *dst,			float *src0,		float *src1,		const int count) = 0;
	virtual void VPCALL MulSub(float *dst,			float constant,	float *src,		const int count) = 0;
	virtual void VPCALL MulSub(float *dst,			float *src0,		float *src1,		const int count) = 0;

	virtual	void VPCALL Dot(float *dst,			const xVec3 &constant,	const xVec3 *src,		const int count) = 0;
	virtual	void VPCALL Dot(float *dst,			const xVec3 &constant,	const xPlane *src,		const int count) = 0;
	virtual void VPCALL Dot(float *dst,			const xVec3 &constant,	const xDrawVert *src,	const int count) = 0;
	virtual	void VPCALL Dot(float *dst,			const xPlane &constant,const xVec3 *src,		const int count) = 0;
	virtual	void VPCALL Dot(float *dst,			const xPlane &constant,const xPlane *src,		const int count) = 0;
	virtual void VPCALL Dot(float *dst,			const xPlane &constant,const xDrawVert *src,	const int count) = 0;
	virtual	void VPCALL Dot(float *dst,			const xVec3 *src0,		const xVec3 *src1,		const int count) = 0;
	virtual void VPCALL Dot(float &dot,			float *src1,		float *src2,		const int count) = 0;

	virtual	void VPCALL CmpGT(byte *dst,			float *src0,		float constant,	const int count) = 0;
	virtual	void VPCALL CmpGT(byte *dst,			const byte bitNum,		float *src0,		float constant,	const int count) = 0;
	virtual	void VPCALL CmpGE(byte *dst,			float *src0,		float constant,	const int count) = 0;
	virtual	void VPCALL CmpGE(byte *dst,			const byte bitNum,		float *src0,		float constant,	const int count) = 0;
	virtual	void VPCALL CmpLT(byte *dst,			float *src0,		float constant,	const int count) = 0;
	virtual	void VPCALL CmpLT(byte *dst,			const byte bitNum,		float *src0,		float constant,	const int count) = 0;
	virtual	void VPCALL CmpLE(byte *dst,			float *src0,		float constant,	const int count) = 0;
	virtual	void VPCALL CmpLE(byte *dst,			const byte bitNum,		float *src0,		float constant,	const int count) = 0;

	virtual	void VPCALL MinMax(float &min,			float &max,				float *src,		const int count) = 0;
	virtual	void VPCALL MinMax(xVec2 &min,		xVec2 &max,			const xVec2 *src,		const int count) = 0;
	virtual	void VPCALL MinMax(xVec3 &min,		xVec3 &max,			const xVec3 *src,		const int count) = 0;
	virtual	void VPCALL MinMax(xVec3 &min,		xVec3 &max,			const xDrawVert *src,	const int count) = 0;
	virtual	void VPCALL MinMax(xVec3 &min,		xVec3 &max,			const xDrawVert *src,	const int *indexes,		const int count) = 0;

	virtual	void VPCALL Clamp(float *dst,			float *src,		float min,		float max,		const int count) = 0;
	virtual	void VPCALL ClampMin(float *dst,		float *src,		float min,		const int count) = 0;
	virtual	void VPCALL ClampMax(float *dst,		float *src,		float max,		const int count) = 0;

	virtual void VPCALL Memcpy(void *dst,			const void *src,		const int count) = 0;
	virtual void VPCALL Memset(void *dst,			const int val,			const int count) = 0;

	// these assume 16 byte aligned and 16 byte padded memory
	virtual void VPCALL Zero16(float *dst,			const int count) = 0;
	virtual void VPCALL Negate16(float *dst,		const int count) = 0;
	virtual void VPCALL Copy16(float *dst,			float *src,		const int count) = 0;
	virtual void VPCALL Add16(float *dst,			float *src1,		float *src2,		const int count) = 0;
	virtual void VPCALL Sub16(float *dst,			float *src1,		float *src2,		const int count) = 0;
	virtual void VPCALL Mul16(float *dst,			float *src1,		float constant,	const int count) = 0;
	virtual void VPCALL AddAssign16(float *dst,	float *src,		const int count) = 0;
	virtual void VPCALL SubAssign16(float *dst,	float *src,		const int count) = 0;
	virtual void VPCALL MulAssign16(float *dst,	float constant,	const int count) = 0;

	virtual void VPCALL TracePointCull(byte *cullBits, byte &totalOr, float radius, const xPlane *planes, const xDrawVert *verts, const int numVerts) = 0;
	virtual void VPCALL DecalPointCull(byte *cullBits, const xPlane *planes, const xDrawVert *verts, const int numVerts) = 0;
	virtual void VPCALL OverlayPointCull(byte *cullBits, xVec2 *texCoords, const xPlane *planes, const xDrawVert *verts, const int numVerts) = 0;
};

#endif /* !__X_SIMD_H__ */
