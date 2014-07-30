#ifndef __X_SIMD_GENERIC_H__
#define __X_SIMD_GENERIC_H__

#pragma once

/*
===============================================================================

	Generic implementation of xSIMDProcessor

===============================================================================
*/

class xSIMD_Generic : public xSIMDProcessor {
public:
	virtual const TCHAR * VPCALL Name() const;

	virtual void VPCALL Add(float *dst,			float constant,	float *src,		const int count);
	virtual void VPCALL Add(float *dst,			float *src0,		float *src1,		const int count);
	virtual void VPCALL Sub(float *dst,			float constant,	float *src,		const int count);
	virtual void VPCALL Sub(float *dst,			float *src0,		float *src1,		const int count);
	virtual void VPCALL Mul(float *dst,			float constant,	float *src,		const int count);
	virtual void VPCALL Mul(float *dst,			float *src0,		float *src1,		const int count);
	virtual void VPCALL Div(float *dst,			float constant,	float *src,		const int count);
	virtual void VPCALL Div(float *dst,			float *src0,		float *src1,		const int count);
	virtual void VPCALL MulAdd(float *dst,			float constant,	float *src,		const int count);
	virtual void VPCALL MulAdd(float *dst,			float *src0,		float *src1,		const int count);
	virtual void VPCALL MulSub(float *dst,			float constant,	float *src,		const int count);
	virtual void VPCALL MulSub(float *dst,			float *src0,		float *src1,		const int count);

	virtual void VPCALL Dot(float *dst,			const xVec3 &constant,	const xVec3 *src,		const int count);
	virtual void VPCALL Dot(float *dst,			const xVec3 &constant,	const xPlane *src,		const int count);
	virtual void VPCALL Dot(float *dst,			const xVec3 &constant,	const xDrawVert *src,	const int count);
	virtual void VPCALL Dot(float *dst,			const xPlane &constant,const xVec3 *src,		const int count);
	virtual void VPCALL Dot(float *dst,			const xPlane &constant,const xPlane *src,		const int count);
	virtual void VPCALL Dot(float *dst,			const xPlane &constant,const xDrawVert *src,	const int count);
	virtual void VPCALL Dot(float *dst,			const xVec3 *src0,		const xVec3 *src1,		const int count);
	virtual void VPCALL Dot(float &dot,			float *src1,		float *src2,		const int count);

	virtual void VPCALL CmpGT(byte *dst,			float *src0,		float constant,	const int count);
	virtual void VPCALL CmpGT(byte *dst,			const byte bitNum,		float *src0,		float constant,	const int count);
	virtual void VPCALL CmpGE(byte *dst,			float *src0,		float constant,	const int count);
	virtual void VPCALL CmpGE(byte *dst,			const byte bitNum,		float *src0,		float constant,	const int count);
	virtual void VPCALL CmpLT(byte *dst,			float *src0,		float constant,	const int count);
	virtual void VPCALL CmpLT(byte *dst,			const byte bitNum,		float *src0,		float constant,	const int count);
	virtual void VPCALL CmpLE(byte *dst,			float *src0,		float constant,	const int count);
	virtual void VPCALL CmpLE(byte *dst,			const byte bitNum,		float *src0,		float constant,	const int count);

	virtual void VPCALL MinMax(float &min,			float &max,				float *src,		const int count);
	virtual	void VPCALL MinMax(xVec2 &min,		xVec2 &max,			const xVec2 *src,		const int count);
	virtual void VPCALL MinMax(xVec3 &min,		xVec3 &max,			const xVec3 *src,		const int count);
	virtual	void VPCALL MinMax(xVec3 &min,		xVec3 &max,			const xDrawVert *src,	const int count);
	virtual	void VPCALL MinMax(xVec3 &min,		xVec3 &max,			const xDrawVert *src,	const int *indexes,		const int count);

	virtual void VPCALL Clamp(float *dst,			float *src,		float min,		float max,		const int count);
	virtual void VPCALL ClampMin(float *dst,		float *src,		float min,		const int count);
	virtual void VPCALL ClampMax(float *dst,		float *src,		float max,		const int count);

	virtual void VPCALL Memcpy(void *dst,			const void *src,		const int count);
	virtual void VPCALL Memset(void *dst,			const int val,			const int count);

	virtual void VPCALL Zero16(float *dst,			const int count);
	virtual void VPCALL Negate16(float *dst,		const int count);
	virtual void VPCALL Copy16(float *dst,			float *src,		const int count);
	virtual void VPCALL Add16(float *dst,			float *src1,		float *src2,		const int count);
	virtual void VPCALL Sub16(float *dst,			float *src1,		float *src2,		const int count);
	virtual void VPCALL Mul16(float *dst,			float *src1,		float constant,	const int count);
	virtual void VPCALL AddAssign16(float *dst,	float *src,		const int count);
	virtual void VPCALL SubAssign16(float *dst,	float *src,		const int count);
	virtual void VPCALL MulAssign16(float *dst,	float constant,	const int count);

	virtual void VPCALL TracePointCull(byte *cullBits, byte &totalOr, float radius, const xPlane *planes, const xDrawVert *verts, const int numVerts);
	virtual void VPCALL DecalPointCull(byte *cullBits, const xPlane *planes, const xDrawVert *verts, const int numVerts);
	virtual void VPCALL OverlayPointCull(byte *cullBits, xVec2 *texCoords, const xPlane *planes, const xDrawVert *verts, const int numVerts);
};

#endif /* !__X_SIMD_GENERIC_H__ */
