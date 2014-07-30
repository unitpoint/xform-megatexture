#ifndef __X_PLUECKER_H__
#define __X_PLUECKER_H__

#pragma once

/*
===============================================================================

	Pluecker coordinate

===============================================================================
*/

class xPluecker {
public:	
	xPluecker();
	explicit xPluecker(const float *a);
	explicit xPluecker(const xVec3 &start, const xVec3 &end);
	explicit xPluecker(const float a1, const float a2, const float a3, const float a4, const float a5, const float a6);

	float			operator[](const int index) const;
	float &			operator[](const int index);
	xPluecker		operator-() const;											// flips the direction
	xPluecker		operator*(const float a) const;
	xPluecker		operator/(const float a) const;
	float			operator*(const xPluecker &a) const;						// permuted inner product
	xPluecker		operator-(const xPluecker &a) const;
	xPluecker		operator+(const xPluecker &a) const;
	xPluecker &	operator*=(const float a);
	xPluecker &	operator/=(const float a);
	xPluecker &	operator+=(const xPluecker &a);
	xPluecker &	operator-=(const xPluecker &a);

	bool			Compare(const xPluecker &a) const;						// exact compare, no epsilon
	bool			Compare(const xPluecker &a, const float epsilon) const;	// compare with epsilon
	bool			operator==(	const xPluecker &a) const;					// exact compare, no epsilon
	bool			operator!=(	const xPluecker &a) const;					// exact compare, no epsilon

	void 			Set(const float a1, const float a2, const float a3, const float a4, const float a5, const float a6);
	void			Zero();

	void			FromLine(const xVec3 &start, const xVec3 &end);			// pluecker from line
	void			FromRay(const xVec3 &start, const xVec3 &dir);			// pluecker from ray
	bool			FromPlanes(const xPlane &p1, const xPlane &p2);			// pluecker from intersection of planes
	bool			ToLine(xVec3 &start, xVec3 &end) const;					// pluecker to line
	bool			ToRay(xVec3 &start, xVec3 &dir) const;					// pluecker to ray
	void			ToDir(xVec3 &dir) const;									// pluecker to direction
	float			PermutedInnerProduct(const xPluecker &a) const;			// pluecker permuted inner product
	float			Distance3DSqr(const xPluecker &a) const;					// pluecker line distance

	float			Length() const;										// pluecker length
	float			LengthSqr() const;									// pluecker squared length
	xPluecker		Normalize() const;									// pluecker normalize
	float			NormalizeSelf();										// pluecker normalize

	int				Dimension() const;

	const float *	ToFloatPtr() const;
	float *	ToFloatPtr();
	xString ToString(int precision = 2) const;

private:
	float			p[6];
};

extern const xPluecker pluecker_origin;
#define pluecker_zero pluecker_origin

X_INLINE xPluecker::xPluecker() {
}

X_INLINE xPluecker::xPluecker(const float *a) {
	memcpy(p, a, 6 * sizeof(float));
}

X_INLINE xPluecker::xPluecker(const xVec3 &start, const xVec3 &end) {
	FromLine(start, end);
}

X_INLINE xPluecker::xPluecker(const float a1, const float a2, const float a3, const float a4, const float a5, const float a6) {
	p[0] = a1;
	p[1] = a2;
	p[2] = a3;
	p[3] = a4;
	p[4] = a5;
	p[5] = a6;
}

X_INLINE xPluecker xPluecker::operator-() const {
	return xPluecker(-p[0], -p[1], -p[2], -p[3], -p[4], -p[5]);
}

X_INLINE float xPluecker::operator[](const int index) const {
	return p[index];
}

X_INLINE float &xPluecker::operator[](const int index) {
	return p[index];
}

X_INLINE xPluecker xPluecker::operator*(const float a) const {
	return xPluecker(p[0]*a, p[1]*a, p[2]*a, p[3]*a, p[4]*a, p[5]*a);
}

X_INLINE float xPluecker::operator*(const xPluecker &a) const {
	return p[0] * a.p[4] + p[1] * a.p[5] + p[2] * a.p[3] + p[4] * a.p[0] + p[5] * a.p[1] + p[3] * a.p[2];
}

X_INLINE xPluecker xPluecker::operator/(const float a) const {
	float inva;

	assert(a != 0.0f);
	inva = 1.0f / a;
	return xPluecker(p[0]*inva, p[1]*inva, p[2]*inva, p[3]*inva, p[4]*inva, p[5]*inva);
}

X_INLINE xPluecker xPluecker::operator+(const xPluecker &a) const {
	return xPluecker(p[0] + a[0], p[1] + a[1], p[2] + a[2], p[3] + a[3], p[4] + a[4], p[5] + a[5]);
}

X_INLINE xPluecker xPluecker::operator-(const xPluecker &a) const {
	return xPluecker(p[0] - a[0], p[1] - a[1], p[2] - a[2], p[3] - a[3], p[4] - a[4], p[5] - a[5]);
}

X_INLINE xPluecker &xPluecker::operator*=(const float a) {
	p[0] *= a;
	p[1] *= a;
	p[2] *= a;
	p[3] *= a;
	p[4] *= a;
	p[5] *= a;
	return *this;
}

X_INLINE xPluecker &xPluecker::operator/=(const float a) {
	float inva;

	assert(a != 0.0f);
	inva = 1.0f / a;
	p[0] *= inva;
	p[1] *= inva;
	p[2] *= inva;
	p[3] *= inva;
	p[4] *= inva;
	p[5] *= inva;
	return *this;
}

X_INLINE xPluecker &xPluecker::operator+=(const xPluecker &a) {
	p[0] += a[0];
	p[1] += a[1];
	p[2] += a[2];
	p[3] += a[3];
	p[4] += a[4];
	p[5] += a[5];
	return *this;
}

X_INLINE xPluecker &xPluecker::operator-=(const xPluecker &a) {
	p[0] -= a[0];
	p[1] -= a[1];
	p[2] -= a[2];
	p[3] -= a[3];
	p[4] -= a[4];
	p[5] -= a[5];
	return *this;
}

X_INLINE bool xPluecker::Compare(const xPluecker &a) const {
	return ((p[0] == a[0]) && (p[1] == a[1]) && (p[2] == a[2]) &&
			(p[3] == a[3]) && (p[4] == a[4]) && (p[5] == a[5]));
}

X_INLINE bool xPluecker::Compare(const xPluecker &a, const float epsilon) const {
	if (xMath::Fabs(p[0] - a[0]) > epsilon) {
		return false;
	}
			
	if (xMath::Fabs(p[1] - a[1]) > epsilon) {
		return false;
	}

	if (xMath::Fabs(p[2] - a[2]) > epsilon) {
		return false;
	}

	if (xMath::Fabs(p[3] - a[3]) > epsilon) {
		return false;
	}

	if (xMath::Fabs(p[4] - a[4]) > epsilon) {
		return false;
	}

	if (xMath::Fabs(p[5] - a[5]) > epsilon) {
		return false;
	}

	return true;
}

X_INLINE bool xPluecker::operator==(const xPluecker &a) const {
	return Compare(a);
}

X_INLINE bool xPluecker::operator!=(const xPluecker &a) const {
	return !Compare(a);
}

X_INLINE void xPluecker::Set(const float a1, const float a2, const float a3, const float a4, const float a5, const float a6) {
	p[0] = a1;
	p[1] = a2;
	p[2] = a3;
	p[3] = a4;
	p[4] = a5;
	p[5] = a6;
}

X_INLINE void xPluecker::Zero() {
	p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = 0.0f;
}

X_INLINE void xPluecker::FromLine(const xVec3 &start, const xVec3 &end) {
	p[0] = start[0] * end[1] - end[0] * start[1];
	p[1] = start[0] * end[2] - end[0] * start[2];
	p[2] = start[0] - end[0];
	p[3] = start[1] * end[2] - end[1] * start[2];
	p[4] = start[2] - end[2];
	p[5] = end[1] - start[1];
}

X_INLINE void xPluecker::FromRay(const xVec3 &start, const xVec3 &dir) {
	p[0] = start[0] * dir[1] - dir[0] * start[1];
	p[1] = start[0] * dir[2] - dir[0] * start[2];
	p[2] = -dir[0];
	p[3] = start[1] * dir[2] - dir[1] * start[2];
	p[4] = -dir[2];
	p[5] = dir[1];
}

X_INLINE bool xPluecker::ToLine(xVec3 &start, xVec3 &end) const {
	xVec3 dir1, dir2;
	float d;

	dir1[0] = p[3];
	dir1[1] = -p[1];
	dir1[2] = p[0];

	dir2[0] = -p[2];
	dir2[1] = p[5];
	dir2[2] = -p[4];

	d = dir2 * dir2;
	if (d == 0.0f) {
		return false; // pluecker coordinate does not represent a line
	}

	start = dir2.Cross(dir1) * (1.0f / d);
	end = start + dir2;
	return true;
}

X_INLINE bool xPluecker::ToRay(xVec3 &start, xVec3 &dir) const {
	xVec3 dir1;
	float d;

	dir1[0] = p[3];
	dir1[1] = -p[1];
	dir1[2] = p[0];

	dir[0] = -p[2];
	dir[1] = p[5];
	dir[2] = -p[4];

	d = dir * dir;
	if (d == 0.0f) {
		return false; // pluecker coordinate does not represent a line
	}

	start = dir.Cross(dir1) * (1.0f / d);
	return true;
}

X_INLINE void xPluecker::ToDir(xVec3 &dir) const {
	dir[0] = -p[2];
	dir[1] = p[5];
	dir[2] = -p[4];
}

X_INLINE float xPluecker::PermutedInnerProduct(const xPluecker &a) const {
	return p[0] * a.p[4] + p[1] * a.p[5] + p[2] * a.p[3] + p[4] * a.p[0] + p[5] * a.p[1] + p[3] * a.p[2];
}

X_INLINE float xPluecker::Length() const {
	return (float)xMath::Sqrt(p[5] * p[5] + p[4] * p[4] + p[2] * p[2]);
}

X_INLINE float xPluecker::LengthSqr() const {
	return (p[5] * p[5] + p[4] * p[4] + p[2] * p[2]);
}

X_INLINE float xPluecker::NormalizeSelf() {
	float l, d;

	l = LengthSqr();
	if (l == 0.0f) {
		return l; // pluecker coordinate does not represent a line
	}
	d = xMath::InvSqrt(l);
	p[0] *= d;
	p[1] *= d;
	p[2] *= d;
	p[3] *= d;
	p[4] *= d;
	p[5] *= d;
	return d * l;
}

X_INLINE xPluecker xPluecker::Normalize() const {
	float d;

	d = LengthSqr();
	if (d == 0.0f) {
		return *this; // pluecker coordinate does not represent a line
	}
	d = xMath::InvSqrt(d);
	return xPluecker(p[0]*d, p[1]*d, p[2]*d, p[3]*d, p[4]*d, p[5]*d);
}

X_INLINE int xPluecker::Dimension() const {
	return 6;
}

X_INLINE const float *xPluecker::ToFloatPtr() const {
	return p;
}

X_INLINE float *xPluecker::ToFloatPtr() {
	return p;
}

#endif /* !__X_PLUECKER_H__ */
