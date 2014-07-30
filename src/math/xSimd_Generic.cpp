#include <xForm.h>

//===============================================================
//
//	Generic implementation of xSIMDProcessor
//
//===============================================================

#define UNROLL1(Y) { int _IX; for (_IX=0;_IX<count;_IX++) {Y(_IX);} }
#define UNROLL2(Y) { int _IX, _NM = count&0xfffffffe; for (_IX=0;_IX<_NM;_IX+=2){Y(_IX+0);Y(_IX+1);} if (_IX < count) {Y(_IX);}}
#define UNROLL4(Y) { int _IX, _NM = count&0xfffffffc; for (_IX=0;_IX<_NM;_IX+=4){Y(_IX+0);Y(_IX+1);Y(_IX+2);Y(_IX+3);}for(;_IX<count;_IX++){Y(_IX);}}
#define UNROLL8(Y) { int _IX, _NM = count&0xfffffff8; for (_IX=0;_IX<_NM;_IX+=8){Y(_IX+0);Y(_IX+1);Y(_IX+2);Y(_IX+3);Y(_IX+4);Y(_IX+5);Y(_IX+6);Y(_IX+7);} _NM = count&0xfffffffe; for(;_IX<_NM;_IX+=2){Y(_IX); Y(_IX+1);} if (_IX < count) {Y(_IX);} }

#ifdef _DEBUG
#define NODEFAULT	default: assert(0)
#elif _WIN32
#define NODEFAULT	default: __assume(0)
#else
#define NODEFAULT
#endif


/*
============
xSIMD_Generic::GetName
============
*/
const TCHAR * xSIMD_Generic::Name() const {
	return _T("generic code");
}

/*
============
xSIMD_Generic::Add

  dst[i] = constant + src[i];
============
*/
void VPCALL xSIMD_Generic::Add(float *dst, float constant, float *src, const int count) {
#define OPER(X) dst[(X)] = src[(X)] + constant;
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Add

  dst[i] = src0[i] + src1[i];
============
*/
void VPCALL xSIMD_Generic::Add(float *dst, float *src0, float *src1, const int count) {
#define OPER(X) dst[(X)] = src0[(X)] + src1[(X)];
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Sub


  dst[i] = constant - src[i];
============
*/
void VPCALL xSIMD_Generic::Sub(float *dst, float constant, float *src, const int count) {
	double c = constant;
#define OPER(X) dst[(X)] = c - src[(X)];
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Sub

  dst[i] = src0[i] - src1[i];
============
*/
void VPCALL xSIMD_Generic::Sub(float *dst, float *src0, float *src1, const int count) {
#define OPER(X) dst[(X)] = src0[(X)] - src1[(X)];
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Mul

  dst[i] = constant * src[i];
============
*/
void VPCALL xSIMD_Generic::Mul(float *dst, float constant, float *src0, const int count) {
	double c = constant;
#define OPER(X) (dst[(X)] = (c * src0[(X)]))
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Mul

  dst[i] = src0[i] * src1[i];
============
*/
void VPCALL xSIMD_Generic::Mul(float *dst, float *src0, float *src1, const int count) {
#define OPER(X) (dst[(X)] = src0[(X)] * src1[(X)])
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Div

  dst[i] = constant / divisor[i];
============
*/
void VPCALL xSIMD_Generic::Div(float *dst, float constant, float *divisor, const int count) {
	double c = constant;
#define OPER(X) (dst[(X)] = (c / divisor[(X)]))
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Div

  dst[i] = src0[i] / src1[i];
============
*/
void VPCALL xSIMD_Generic::Div(float *dst, float *src0, float *src1, const int count) {
#define OPER(X) (dst[(X)] = src0[(X)] / src1[(X)])
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::MulAdd

  dst[i] += constant * src[i];
============
*/
void VPCALL xSIMD_Generic::MulAdd(float *dst, float constant, float *src, const int count) {
	double c = constant;
#define OPER(X) (dst[(X)] += c * src[(X)])
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::MulAdd

  dst[i] += src0[i] * src1[i];
============
*/
void VPCALL xSIMD_Generic::MulAdd(float *dst, float *src0, float *src1, const int count) {
#define OPER(X) (dst[(X)] += src0[(X)] * src1[(X)])
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::MulSub

  dst[i] -= constant * src[i];
============
*/
void VPCALL xSIMD_Generic::MulSub(float *dst, float constant, float *src, const int count) {
	double c = constant;
#define OPER(X) (dst[(X)] -= c * src[(X)])
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::MulSub

  dst[i] -= src0[i] * src1[i];
============
*/
void VPCALL xSIMD_Generic::MulSub(float *dst, float *src0, float *src1, const int count) {
#define OPER(X) (dst[(X)] -= src0[(X)] * src1[(X)])
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Dot

  dst[i] = constant * src[i];
============
*/
void VPCALL xSIMD_Generic::Dot(float *dst, const xVec3 &constant, const xVec3 *src, const int count) {
#define OPER(X) dst[(X)] = constant * src[(X)];
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Dot

  dst[i] = constant * src[i].Normal() + src[i][3];
============
*/
void VPCALL xSIMD_Generic::Dot(float *dst, const xVec3 &constant, const xPlane *src, const int count) {
#define OPER(X) dst[(X)] = constant * src[(X)].Normal() + src[(X)][3];
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Dot

  dst[i] = constant * src[i].xyz;
============
*/
void VPCALL xSIMD_Generic::Dot(float *dst, const xVec3 &constant, const xDrawVert *src, const int count) {
#define OPER(X) dst[(X)] = constant * src[(X)].xyz;
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Dot

  dst[i] = constant.Normal() * src[i] + constant[3];
============
*/
void VPCALL xSIMD_Generic::Dot(float *dst, const xPlane &constant, const xVec3 *src, const int count) {
#define OPER(X) dst[(X)] = constant.Normal() * src[(X)] + constant[3];
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Dot

  dst[i] = constant.Normal() * src[i].Normal() + constant[3] * src[i][3];
============
*/
void VPCALL xSIMD_Generic::Dot(float *dst, const xPlane &constant, const xPlane *src, const int count) {
#define OPER(X) dst[(X)] = constant.Normal() * src[(X)].Normal() + constant[3] * src[(X)][3];
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Dot

  dst[i] = constant.Normal() * src[i].xyz + constant[3];
============
*/
void VPCALL xSIMD_Generic::Dot(float *dst, const xPlane &constant, const xDrawVert *src, const int count) {
#define OPER(X) dst[(X)] = constant.Normal() * src[(X)].xyz + constant[3];
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Dot

  dst[i] = src0[i] * src1[i];
============
*/
void VPCALL xSIMD_Generic::Dot(float *dst, const xVec3 *src0, const xVec3 *src1, const int count) {
#define OPER(X) dst[(X)] = src0[(X)] * src1[(X)];
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Dot

  dot = src1[0] * src2[0] + src1[1] * src2[1] + src1[2] * src2[2] + ...
============
*/
void VPCALL xSIMD_Generic::Dot(float &dot, float *src1, float *src2, const int count) {
#if 1

	switch(count) {
		case 0: {
			dot = 0.0f;
			return;
		}
		case 1: {
			dot = src1[0] * src2[0];
			return;
		}
		case 2: {
			dot = src1[0] * src2[0] + src1[1] * src2[1];
			return;
		}
		case 3: {
			dot = src1[0] * src2[0] + src1[1] * src2[1] + src1[2] * src2[2];
			return;
		}
		default: {
			int i;
			double s0, s1, s2, s3;
			s0 = src1[0] * src2[0];
			s1 = src1[1] * src2[1];
			s2 = src1[2] * src2[2];
			s3 = src1[3] * src2[3];
			for (i = 4; i < count-7; i += 8) {
				s0 += src1[i+0] * src2[i+0];
				s1 += src1[i+1] * src2[i+1];
				s2 += src1[i+2] * src2[i+2];
				s3 += src1[i+3] * src2[i+3];
				s0 += src1[i+4] * src2[i+4];
				s1 += src1[i+5] * src2[i+5];
				s2 += src1[i+6] * src2[i+6];
				s3 += src1[i+7] * src2[i+7];
			}
			switch(count - i) {
				NODEFAULT;
				case 7: s0 += src1[i+6] * src2[i+6];
				case 6: s1 += src1[i+5] * src2[i+5];
				case 5: s2 += src1[i+4] * src2[i+4];
				case 4: s3 += src1[i+3] * src2[i+3];
				case 3: s0 += src1[i+2] * src2[i+2];
				case 2: s1 += src1[i+1] * src2[i+1];
				case 1: s2 += src1[i+0] * src2[i+0];
				case 0: break;
			}
			double sum;
			sum = s3;
			sum += s2;
			sum += s1;
			sum += s0;
			dot = sum;
		}
	}

#else

	dot = 0.0f;
	for (i = 0; i < count; i++) {
		dot += src1[i] * src2[i];
	}

#endif
}

/*
============
xSIMD_Generic::CmpGT

  dst[i] = src0[i] > constant;
============
*/
void VPCALL xSIMD_Generic::CmpGT(byte *dst, float *src0, float constant, const int count) {
#define OPER(X) dst[(X)] = src0[(X)] > constant;
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::CmpGT

  dst[i] |= (src0[i] > constant) << bitNum;
============
*/
void VPCALL xSIMD_Generic::CmpGT(byte *dst, const byte bitNum, float *src0, float constant, const int count) {
#define OPER(X) dst[(X)] |= (src0[(X)] > constant) << bitNum;
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::CmpGE

  dst[i] = src0[i] >= constant;
============
*/
void VPCALL xSIMD_Generic::CmpGE(byte *dst, float *src0, float constant, const int count) {
#define OPER(X) dst[(X)] = src0[(X)] >= constant;
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::CmpGE

  dst[i] |= (src0[i] >= constant) << bitNum;
============
*/
void VPCALL xSIMD_Generic::CmpGE(byte *dst, const byte bitNum, float *src0, float constant, const int count) {
#define OPER(X) dst[(X)] |= (src0[(X)] >= constant) << bitNum;
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::CmpLT

  dst[i] = src0[i] < constant;
============
*/
void VPCALL xSIMD_Generic::CmpLT(byte *dst, float *src0, float constant, const int count) {
#define OPER(X) dst[(X)] = src0[(X)] < constant;
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::CmpLT

  dst[i] |= (src0[i] < constant) << bitNum;
============
*/
void VPCALL xSIMD_Generic::CmpLT(byte *dst, const byte bitNum, float *src0, float constant, const int count) {
#define OPER(X) dst[(X)] |= (src0[(X)] < constant) << bitNum;
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::CmpLE

  dst[i] = src0[i] <= constant;
============
*/
void VPCALL xSIMD_Generic::CmpLE(byte *dst, float *src0, float constant, const int count) {
#define OPER(X) dst[(X)] = src0[(X)] <= constant;
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::CmpLE

  dst[i] |= (src0[i] <= constant) << bitNum;
============
*/
void VPCALL xSIMD_Generic::CmpLE(byte *dst, const byte bitNum, float *src0, float constant, const int count) {
#define OPER(X) dst[(X)] |= (src0[(X)] <= constant) << bitNum;
	UNROLL4(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::MinMax
============
*/
void VPCALL xSIMD_Generic::MinMax(float &min, float &max, float *src, const int count) {
	min = xMath::WORLD_INFINITY; 
  max = -xMath::WORLD_INFINITY;
#define OPER(X) if (src[(X)] < min) {min = src[(X)];} if (src[(X)] > max) {max = src[(X)];}
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::MinMax
============
*/
void VPCALL xSIMD_Generic::MinMax(xVec2 &min, xVec2 &max, const xVec2 *src, const int count) {
	min[0] = min[1] = xMath::WORLD_INFINITY; 
  max[0] = max[1] = -xMath::WORLD_INFINITY;
#define OPER(X) const xVec2 &v = src[(X)]; if (v[0] < min[0]) { min[0] = v[0]; } if (v[0] > max[0]) { max[0] = v[0]; } if (v[1] < min[1]) { min[1] = v[1]; } if (v[1] > max[1]) { max[1] = v[1]; }
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::MinMax
============
*/
void VPCALL xSIMD_Generic::MinMax(xVec3 &min, xVec3 &max, const xVec3 *src, const int count) {
	min[0] = min[1] = min[2] = xMath::WORLD_INFINITY; 
  max[0] = max[1] = max[2] = -xMath::WORLD_INFINITY;
#define OPER(X) const xVec3 &v = src[(X)]; if (v[0] < min[0]) { min[0] = v[0]; } if (v[0] > max[0]) { max[0] = v[0]; } if (v[1] < min[1]) { min[1] = v[1]; } if (v[1] > max[1]) { max[1] = v[1]; } if (v[2] < min[2]) { min[2] = v[2]; } if (v[2] > max[2]) { max[2] = v[2]; }
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::MinMax
============
*/
void VPCALL xSIMD_Generic::MinMax(xVec3 &min, xVec3 &max, const xDrawVert *src, const int count) {
	min[0] = min[1] = min[2] = xMath::WORLD_INFINITY; 
  max[0] = max[1] = max[2] = -xMath::WORLD_INFINITY;
#define OPER(X) const xVec3 &v = src[(X)].xyz; if (v[0] < min[0]) { min[0] = v[0]; } if (v[0] > max[0]) { max[0] = v[0]; } if (v[1] < min[1]) { min[1] = v[1]; } if (v[1] > max[1]) { max[1] = v[1]; } if (v[2] < min[2]) { min[2] = v[2]; } if (v[2] > max[2]) { max[2] = v[2]; }
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::MinMax
============
*/
void VPCALL xSIMD_Generic::MinMax(xVec3 &min, xVec3 &max, const xDrawVert *src, const int *indexes, const int count) {
	min[0] = min[1] = min[2] = xMath::WORLD_INFINITY; 
  max[0] = max[1] = max[2] = -xMath::WORLD_INFINITY;
#define OPER(X) const xVec3 &v = src[indexes[(X)]].xyz; if (v[0] < min[0]) { min[0] = v[0]; } if (v[0] > max[0]) { max[0] = v[0]; } if (v[1] < min[1]) { min[1] = v[1]; } if (v[1] > max[1]) { max[1] = v[1]; } if (v[2] < min[2]) { min[2] = v[2]; } if (v[2] > max[2]) { max[2] = v[2]; }
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Clamp
============
*/
void VPCALL xSIMD_Generic::Clamp(float *dst, float *src, float min, float max, const int count) {
#define OPER(X) dst[(X)] = src[(X)] < min ? min : src[(X)] > max ? max : src[(X)];
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::ClampMin
============
*/
void VPCALL xSIMD_Generic::ClampMin(float *dst, float *src, float min, const int count) {
#define OPER(X) dst[(X)] = src[(X)] < min ? min : src[(X)];
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::ClampMax
============
*/
void VPCALL xSIMD_Generic::ClampMax(float *dst, float *src, float max, const int count) {
#define OPER(X) dst[(X)] = src[(X)] > max ? max : src[(X)];
	UNROLL1(OPER)
#undef OPER
}

/*
================
xSIMD_Generic::Memcpy
================
*/
void VPCALL xSIMD_Generic::Memcpy(void *dst, const void *src, const int count) {
	memcpy(dst, src, count);
}

/*
================
xSIMD_Generic::Memset
================
*/
void VPCALL xSIMD_Generic::Memset(void *dst, const int val, const int count) {
	memset(dst, val, count);
}

/*
============
xSIMD_Generic::Zero16
============
*/
void VPCALL xSIMD_Generic::Zero16(float *dst, const int count) {
	memset(dst, 0, count * sizeof(float));
}

/*
============
xSIMD_Generic::Negate16
============
*/
void VPCALL xSIMD_Generic::Negate16(float *dst, const int count) {
	unsigned int *ptr = (unsigned int*)dst;
#define OPER(X) ptr[(X)] ^= (1 << 31)		// IEEE 32 bits float sign bit
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Copy16
============
*/
void VPCALL xSIMD_Generic::Copy16(float *dst, float *src, const int count) {
#define OPER(X) dst[(X)] = src[(X)]
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Add16
============
*/
void VPCALL xSIMD_Generic::Add16(float *dst, float *src1, float *src2, const int count) {
#define OPER(X) dst[(X)] = src1[(X)] + src2[(X)]
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Sub16
============
*/
void VPCALL xSIMD_Generic::Sub16(float *dst, float *src1, float *src2, const int count) {
#define OPER(X) dst[(X)] = src1[(X)] - src2[(X)]
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::Mul16
============
*/
void VPCALL xSIMD_Generic::Mul16(float *dst, float *src1, float constant, const int count) {
#define OPER(X) dst[(X)] = src1[(X)] * constant
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::AddAssign16
============
*/
void VPCALL xSIMD_Generic::AddAssign16(float *dst, float *src, const int count) {
#define OPER(X) dst[(X)] += src[(X)]
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::SubAssign16
============
*/
void VPCALL xSIMD_Generic::SubAssign16(float *dst, float *src, const int count) {
#define OPER(X) dst[(X)] -= src[(X)]
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::MulAssign16
============
*/
void VPCALL xSIMD_Generic::MulAssign16(float *dst, float constant, const int count) {
#define OPER(X) dst[(X)] *= constant
	UNROLL1(OPER)
#undef OPER
}

/*
============
xSIMD_Generic::TracePointCull
============
*/
void VPCALL xSIMD_Generic::TracePointCull(byte *cullBits, byte &totalOr, float radius, const xPlane *planes, const xDrawVert *verts, const int numVerts) {
	int i;
	byte tOr;

	tOr = 0;

	for (i = 0; i < numVerts; i++) {
		byte bits;
		float d0, d1, d2, d3, t;
		const xVec3 &v = verts[i].xyz;

		d0 = planes[0].Distance(v);
		d1 = planes[1].Distance(v);
		d2 = planes[2].Distance(v);
		d3 = planes[3].Distance(v);

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
}

/*
============
xSIMD_Generic::DecalPointCull
============
*/
void VPCALL xSIMD_Generic::DecalPointCull(byte *cullBits, const xPlane *planes, const xDrawVert *verts, const int numVerts) {
	int i;

	for (i = 0; i < numVerts; i++) {
		byte bits;
		float d0, d1, d2, d3, d4, d5;
		const xVec3 &v = verts[i].xyz;

		d0 = planes[0].Distance(v);
		d1 = planes[1].Distance(v);
		d2 = planes[2].Distance(v);
		d3 = planes[3].Distance(v);
		d4 = planes[4].Distance(v);
		d5 = planes[5].Distance(v);

		bits  = FLOATSIGNBITSET(d0) << 0;
		bits |= FLOATSIGNBITSET(d1) << 1;
		bits |= FLOATSIGNBITSET(d2) << 2;
		bits |= FLOATSIGNBITSET(d3) << 3;
		bits |= FLOATSIGNBITSET(d4) << 4;
		bits |= FLOATSIGNBITSET(d5) << 5;

		cullBits[i] = bits ^ 0x3F;		// flip lower 6 bits
	}
}

/*
============
xSIMD_Generic::OverlayPointCull
============
*/
void VPCALL xSIMD_Generic::OverlayPointCull(byte *cullBits, xVec2 *texCoords, const xPlane *planes, const xDrawVert *verts, const int numVerts) {
	int i;

	for (i = 0; i < numVerts; i++) {
		byte bits;
		float d0, d1;
		const xVec3 &v = verts[i].xyz;

		texCoords[i][0] = d0 = planes[0].Distance(v);
		texCoords[i][1] = d1 = planes[1].Distance(v);

		bits  = FLOATSIGNBITSET(d0) << 0;
		d0 = 1.0f - d0;
		bits |= FLOATSIGNBITSET(d1) << 1;
		d1 = 1.0f - d1;
		bits |= FLOATSIGNBITSET(d0) << 2;
		bits |= FLOATSIGNBITSET(d1) << 3;

		cullBits[i] = bits;
	}
}

