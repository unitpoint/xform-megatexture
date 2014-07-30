#ifndef __X_VECTOR_H__
#define __X_VECTOR_H__

#pragma once

/*
===============================================================================

  xVec3 classes

===============================================================================
*/

#include "../common/xString.h"
#include "xSimd.h"
#include "xRandom.h"

#define VECTOR_EPSILON		0.001f

class xAngles;
class xPolar3;
class xMat3;

//===============================================================
//
//	xVec2 - 2D vector
//
//===============================================================

class xVec2
{
public:
	float			x;
	float			y;

	xVec2();
	explicit xVec2(float x, float y);

	void 			Set(float x, float y);
	void			Zero();

	float			operator[](int index) const;
	float &			operator[](int index);
	xVec2			operator-() const;
	double    operator*(const xVec2 &a) const;
	xVec2			operator*(float a) const;
	xVec2			operator/(float a) const;
	xVec2			operator+(const xVec2 &a) const;
	xVec2			operator-(const xVec2 &a) const;
	xVec2 &		operator+=(const xVec2 &a);
	xVec2 &		operator-=(const xVec2 &a);
	xVec2 &		operator/=(const xVec2 &a);
	xVec2 &		operator/=(float a);
	xVec2 &		operator*=(float a);

	friend xVec2	operator*(float a, const xVec2 b);
  xVec2     Mul(const xVec2& b) const;
  xVec2     Div(const xVec2& b) const;

	bool			Compare(const xVec2 &a) const;							// exact compare, no epsilon
	bool			Compare(const xVec2 &a, float epsilon) const;		// compare with epsilon
	bool			operator==(	const xVec2 &a) const;						// exact compare, no epsilon
	bool			operator!=(	const xVec2 &a) const;						// exact compare, no epsilon

	float			Length() const;
	float			LengthFast() const;
	float			LengthSqr() const;
	float			Normalize();			// returns length
	float			NormalizeFast();		// returns length
  xVec2     Norm();
  xVec2     NormFast();
	xVec2 &		Truncate(float length);	// cap length
	void			Clamp(const xVec2 &min, const xVec2 &max);
	void			Snap();				// snap to closest integer value
	void			SnapInt();			// snap towards integer (floor)

	int				Dimension() const;

	const float *	ToFloatPtr() const;
	float *	ToFloatPtr();

	xString ToString(int precision = 2) const;

	void			Lerp(const xVec2 &v1, const xVec2 &v2, float l);
};

extern const xVec2 vec2_origin;
#define vec2_zero vec2_origin

X_INLINE xVec2::xVec2() {
}

X_INLINE xVec2::xVec2(float x, float y) {
	this->x = x;
	this->y = y;
}

X_INLINE void xVec2::Set(float x, float y) {
	this->x = x;
	this->y = y;
}

X_INLINE void xVec2::Zero() {
	x = y = 0.0f;
}

X_INLINE bool xVec2::Compare(const xVec2 &a) const {
	return ((x == a.x) && (y == a.y));
}

X_INLINE bool xVec2::Compare(const xVec2 &a, float epsilon) const {
	if (xMath::Fabs(x - a.x) > epsilon) {
		return false;
	}
			
	if (xMath::Fabs(y - a.y) > epsilon) {
		return false;
	}

	return true;
}

X_INLINE bool xVec2::operator==(const xVec2 &a) const {
	return Compare(a);
}

X_INLINE bool xVec2::operator!=(const xVec2 &a) const {
	return !Compare(a);
}

X_INLINE float xVec2::operator[](int index) const {
	return (&x)[ index ];
}

X_INLINE float& xVec2::operator[](int index) {
	return (&x)[ index ];
}

X_INLINE float xVec2::Length() const {
	return (float)xMath::Sqrt(x * x + y * y);
}

X_INLINE float xVec2::LengthFast() const {
	float sqrLength;

	sqrLength = x * x + y * y;
	return sqrLength * xMath::RSqrt(sqrLength);
}

X_INLINE float xVec2::LengthSqr() const {
	return (x * x + y * y);
}

X_INLINE float xVec2::Normalize()
{
	double sqrLength = x * x + y * y;
	double invLength = xMath::InvSqrt64(sqrLength);
	x *= invLength;
	y *= invLength;
	return invLength * sqrLength;
}

X_INLINE float xVec2::NormalizeFast() {
	float lengthSqr, invLength;

	lengthSqr = x * x + y * y;
	invLength = xMath::RSqrt(lengthSqr);
	x *= invLength;
	y *= invLength;
	return invLength * lengthSqr;
}

X_INLINE xVec2 xVec2::Norm()
{
	double invLength = xMath::InvSqrt64(x * x + y * y);
	return xVec2(x * invLength, y * invLength);
}
X_INLINE xVec2 xVec2::NormFast()
{
	float invLength = xMath::RSqrt(x * x + y * y);
	return xVec2(x * invLength, y * invLength);
}

X_INLINE xVec2 &xVec2::Truncate(float length)
{
	if(!length)
		Zero();
	else{
		double length2 = LengthSqr();
		if (length2 > length * length) {
			double ilength = length * xMath::InvSqrt64(length2);
			x *= ilength;
			y *= ilength;
		}
	}

	return *this;
}

X_INLINE void xVec2::Clamp(const xVec2 &min, const xVec2 &max) {
	if (x < min.x) {
		x = min.x;
	} else if (x > max.x) {
		x = max.x;
	}
	if (y < min.y) {
		y = min.y;
	} else if (y > max.y) {
		y = max.y;
	}
}

X_INLINE void xVec2::Snap() {
	x = floor(x + 0.5f);
	y = floor(y + 0.5f);
}

X_INLINE void xVec2::SnapInt() {
	x = float(int(x));
	y = float(int(y));
}

X_INLINE xVec2 xVec2::operator-() const {
	return xVec2(-x, -y);
}
	
X_INLINE xVec2 xVec2::operator-(const xVec2 &a) const {
	return xVec2(x - a.x, y - a.y);
}

X_INLINE double xVec2::operator*(const xVec2 &a) const {
	return x * a.x + y * a.y;
}

X_INLINE xVec2 xVec2::operator*(float a) const {
	return xVec2(x * a, y * a);
}

X_INLINE xVec2 xVec2::operator/(float a) const {
	float inva = 1.0f / a;
	return xVec2(x * inva, y * inva);
}

X_INLINE xVec2 operator*(float a, const xVec2 b) {
	return xVec2(b.x * a, b.y * a);
}

X_INLINE xVec2 xVec2::Mul(const xVec2& b) const {
	return xVec2(x * b.x, y * b.y);
}
X_INLINE xVec2 xVec2::Div(const xVec2& b) const {
	return xVec2(x / b.x, y / b.y);
}

X_INLINE xVec2 xVec2::operator+(const xVec2 &a) const {
	return xVec2(x + a.x, y + a.y);
}

X_INLINE xVec2 &xVec2::operator+=(const xVec2 &a) {
	x += a.x;
	y += a.y;

	return *this;
}

X_INLINE xVec2 &xVec2::operator/=(const xVec2 &a) {
	x /= a.x;
	y /= a.y;

	return *this;
}

X_INLINE xVec2 &xVec2::operator/=(float a) {
	float inva = 1.0f / a;
	x *= inva;
	y *= inva;

	return *this;
}

X_INLINE xVec2 &xVec2::operator-=(const xVec2 &a) {
	x -= a.x;
	y -= a.y;

	return *this;
}

X_INLINE xVec2 &xVec2::operator*=(float a) {
	x *= a;
	y *= a;

	return *this;
}

X_INLINE int xVec2::Dimension() const {
	return 2;
}

X_INLINE const float *xVec2::ToFloatPtr() const {
	return &x;
}

X_INLINE float *xVec2::ToFloatPtr() {
	return &x;
}

//===============================================================
//
//	xVec3 - 3D vector
//
//===============================================================

class xVec3d;

class xVec3 {
public:	
	float			x;
	float			y;
	float			z;

	xVec3();
	explicit xVec3(float x, float y, float z);

	void 			Set(float x, float y, float z);
	void			Zero();

	float			operator[](const int index) const;
	float &			operator[](const int index);
	xVec3			operator-() const;
	xVec3 &		operator=(const xVec3 &a);		// required because of a msvc 6 & 7 bug
	double 		operator*(const xVec3 &a) const;
	xVec3			operator*(float a) const;
	xVec3			operator/(float a) const;
	xVec3			operator+(const xVec3 &a) const;
	xVec3			operator-(const xVec3 &a) const;
	xVec3 &		operator+=(const xVec3 &a);
	xVec3 &		operator-=(const xVec3 &a);
	xVec3 &		operator/=(const xVec3 &a);
	xVec3 &		operator/=(float a);
	xVec3 &		operator*=(float a);

	friend xVec3	operator*(float a, const xVec3 b);
  xVec3     Mul(const xVec3& b) const;
  xVec3     Div(const xVec3& b) const;

	bool			Compare(const xVec3 &a) const;							// exact compare, no epsilon
	bool			Compare(const xVec3 &a, float epsilon) const;		// compare with epsilon
	bool			operator==(	const xVec3 &a) const;						// exact compare, no epsilon
	bool			operator!=(	const xVec3 &a) const;						// exact compare, no epsilon

	bool			FixDegenerateNormal();	// fix degenerate axial cases
	bool			FixDenormals();			// change tiny numbers to zero

	xVec3			Cross(const xVec3 &a) const;
	xVec3 &		Cross(const xVec3 &a, const xVec3 &b);
	float			Length() const;
	float			LengthSqr() const;
	float			LengthFast() const;
	float			Normalize();				// returns length
	float			NormalizeFast();			// returns length
  xVec3     Norm() const;
  xVec3     NormFast() const;
  xVec3     Fabs() const;
	xVec3 &		Truncate(float length);		// cap length
	void			Clamp(const xVec3 &min, const xVec3 &max);
	void			Snap();					// snap to closest integer value
	void			SnapInt();				// snap towards integer (floor)

	int				Dimension() const;

	float			ToYaw() const;
	float			ToPitch() const;
	xAngles		ToAngles() const;
	xPolar3		ToPolar() const;
	xMat3			ToMat3() const;		// vector should be normalized
  xVec3d    ToVec3d() const;
	const xVec2 &	ToVec2() const;
	xVec2 &		ToVec2();
	const float *	ToFloatPtr() const;
	float *			ToFloatPtr();
	xString ToString(int precision = 2) const;

	void			NormalVectors(xVec3 &left, xVec3 &down) const;	// vector should be normalized
	void			OrthogonalBasis(xVec3 &left, xVec3 &up) const;

	void			ProjectOntoPlane(const xVec3 &normal, float overBounce = 1.0f);
	bool			ProjectAlongPlane(const xVec3 &normal, float epsilon, float overBounce = 1.0f);
	void			ProjectSelfOntoSphere(float radius);

	void			Lerp(const xVec3 &v1, const xVec3 &v2, float l);
	void			SLerp(const xVec3 &v1, const xVec3 &v2, float l);
};

extern const xVec3 vec3_origin;
#define vec3_zero vec3_origin

extern const xVec3 vec3_forward;  // 1, 0, 0
extern const xVec3 vec3_right;    // 0, -1, 0 => left: 0, 1, 0
extern const xVec3 vec3_up;       // 0, 0, 1

X_INLINE xVec3::xVec3() {
}

X_INLINE xVec3::xVec3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

X_INLINE float xVec3::operator[](const int index) const {
	return (&x)[ index ];
}

X_INLINE float &xVec3::operator[](const int index) {
	return (&x)[ index ];
}

X_INLINE void xVec3::Set(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

X_INLINE void xVec3::Zero() {
	x = y = z = 0.0f;
}

X_INLINE xVec3 xVec3::operator-() const {
	return xVec3(-x, -y, -z);
}

X_INLINE xVec3 &xVec3::operator=(const xVec3 &a) {
	x = a.x;
	y = a.y;
	z = a.z;
	return *this;
}

X_INLINE xVec3 xVec3::operator-(const xVec3 &a) const {
	return xVec3(x - a.x, y - a.y, z - a.z);
}

X_INLINE double xVec3::operator*(const xVec3 &a) const {
	return x * a.x + y * a.y + z * a.z;
}

X_INLINE xVec3 xVec3::operator*(float a) const {
	return xVec3(x * a, y * a, z * a);
}

X_INLINE xVec3 xVec3::operator/(float a) const {
	float inva = 1.0f / a;
	return xVec3(x * inva, y * inva, z * inva);
}

X_INLINE xVec3 operator*(float a, const xVec3 b) {
	return xVec3(b.x * a, b.y * a, b.z * a);
}

X_INLINE xVec3 xVec3::operator+(const xVec3 &a) const {
	return xVec3(x + a.x, y + a.y, z + a.z);
}

X_INLINE xVec3 &xVec3::operator+=(const xVec3 &a) {
	x += a.x;
	y += a.y;
	z += a.z;

	return *this;
}

X_INLINE xVec3 &xVec3::operator/=(const xVec3 &a) {
	x /= a.x;
	y /= a.y;
	z /= a.z;

	return *this;
}

X_INLINE xVec3 &xVec3::operator/=(float a) {
	float inva = 1.0f / a;
	x *= inva;
	y *= inva;
	z *= inva;

	return *this;
}

X_INLINE xVec3 &xVec3::operator-=(const xVec3 &a) {
	x -= a.x;
	y -= a.y;
	z -= a.z;

	return *this;
}

X_INLINE xVec3 &xVec3::operator*=(float a) {
	x *= a;
	y *= a;
	z *= a;

	return *this;
}

X_INLINE xVec3 xVec3::Mul(const xVec3& b) const
{
  return xVec3(x*b.x, y*b.y, z*b.z);
}

X_INLINE xVec3 xVec3::Div(const xVec3& b) const
{
  return xVec3(x/b.x, y/b.y, z/b.z);
}

X_INLINE bool xVec3::Compare(const xVec3 &a) const {
	return ((x == a.x) && (y == a.y) && (z == a.z));
}

X_INLINE bool xVec3::Compare(const xVec3 &a, float epsilon) const {
	if (xMath::Fabs(x - a.x) > epsilon) {
		return false;
	}
			
	if (xMath::Fabs(y - a.y) > epsilon) {
		return false;
	}

	if (xMath::Fabs(z - a.z) > epsilon) {
		return false;
	}

	return true;
}

X_INLINE bool xVec3::operator==(const xVec3 &a) const {
	return Compare(a);
}

X_INLINE bool xVec3::operator!=(const xVec3 &a) const {
	return !Compare(a);
}

X_INLINE float xVec3::NormalizeFast() {
	float sqrLength, invLength;

	sqrLength = x * x + y * y + z * z;
	invLength = xMath::RSqrt(sqrLength);
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return invLength * sqrLength;
}

X_INLINE xVec3 xVec3::Norm() const
{
	double invLength = xMath::InvSqrt64(x * x + y * y + z * z);
	return xVec3(x * invLength, y * invLength, z * invLength);
}
X_INLINE xVec3 xVec3::NormFast() const
{
	float invLength = xMath::RSqrt(x * x + y * y + z * z);
	return xVec3(x * invLength, y * invLength, z * invLength);
}
X_INLINE xVec3 xVec3::Fabs() const
{
  return xVec3(xMath::Fabs(x), xMath::Fabs(y), xMath::Fabs(z));
}

X_INLINE bool xVec3::FixDegenerateNormal() {
	if (x == 0.0f) {
		if (y == 0.0f) {
			if (z > 0.0f) {
				if (z != 1.0f) {
					z = 1.0f;
					return true;
				}
			} else {
				if (z != -1.0f) {
					z = -1.0f;
					return true;
				}
			}
			return false;
		} else if (z == 0.0f) {
			if (y > 0.0f) {
				if (y != 1.0f) {
					y = 1.0f;
					return true;
				}
			} else {
				if (y != -1.0f) {
					y = -1.0f;
					return true;
				}
			}
			return false;
		}
	} else if (y == 0.0f) {
		if (z == 0.0f) {
			if (x > 0.0f) {
				if (x != 1.0f) {
					x = 1.0f;
					return true;
				}
			} else {
				if (x != -1.0f) {
					x = -1.0f;
					return true;
				}
			}
			return false;
		}
	}
	if (xMath::Fabs(x) == 1.0f) {
		if (y != 0.0f || z != 0.0f) {
			y = z = 0.0f;
			return true;
		}
		return false;
	} else if (xMath::Fabs(y) == 1.0f) {
		if (x != 0.0f || z != 0.0f) {
			x = z = 0.0f;
			return true;
		}
		return false;
	} else if (xMath::Fabs(z) == 1.0f) {
		if (x != 0.0f || y != 0.0f) {
			x = y = 0.0f;
			return true;
		}
		return false;
	}
	return false;
}

X_INLINE bool xVec3::FixDenormals() {
	bool denormal = false;
	if (fabs(x) < 1e-30f) {
		x = 0.0f;
		denormal = true;
	}
	if (fabs(y) < 1e-30f) {
		y = 0.0f;
		denormal = true;
	}
	if (fabs(z) < 1e-30f) {
		z = 0.0f;
		denormal = true;
	}
	return denormal;
}

X_INLINE xVec3 xVec3::Cross(const xVec3 &a) const {
	return xVec3(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x);
}

X_INLINE xVec3 &xVec3::Cross(const xVec3 &a, const xVec3 &b) {
	x = a.y * b.z - a.z * b.y;
	y = a.z * b.x - a.x * b.z;
	z = a.x * b.y - a.y * b.x;

	return *this;
}

X_INLINE float xVec3::Length() const {
	return (float)xMath::Sqrt(x * x + y * y + z * z);
}

X_INLINE float xVec3::LengthSqr() const {
	return (x * x + y * y + z * z);
}

X_INLINE float xVec3::LengthFast() const {
	float sqrLength;

	sqrLength = x * x + y * y + z * z;
	return sqrLength * xMath::RSqrt(sqrLength);
}

X_INLINE float xVec3::Normalize() {
	double sqrLength = x * x + y * y + z * z;
	double invLength = xMath::InvSqrt64(sqrLength);
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return invLength * sqrLength;
}

X_INLINE xVec3 &xVec3::Truncate(float length) {
	if (!length) {
		Zero();
	}
	else {
		double length2 = LengthSqr();
		if (length2 > length * length) {
			double ilength = length * xMath::InvSqrt64(length2);
			x *= ilength;
			y *= ilength;
			z *= ilength;
		}
	}

	return *this;
}

X_INLINE void xVec3::Clamp(const xVec3 &min, const xVec3 &max) {
	if (x < min.x) {
		x = min.x;
	} else if (x > max.x) {
		x = max.x;
	}
	if (y < min.y) {
		y = min.y;
	} else if (y > max.y) {
		y = max.y;
	}
	if (z < min.z) {
		z = min.z;
	} else if (z > max.z) {
		z = max.z;
	}
}

X_INLINE void xVec3::Snap() {
	x = floor(x + 0.5f);
	y = floor(y + 0.5f);
	z = floor(z + 0.5f);
}

X_INLINE void xVec3::SnapInt() {
	x = float(int(x));
	y = float(int(y));
	z = float(int(z));
}

X_INLINE int xVec3::Dimension() const {
	return 3;
}

X_INLINE const xVec2 &xVec3::ToVec2() const {
	return *(const xVec2*)this;
}

X_INLINE xVec2 &xVec3::ToVec2() {
	return *(xVec2*)this;
}

X_INLINE const float *xVec3::ToFloatPtr() const {
	return &x;
}

X_INLINE float *xVec3::ToFloatPtr() {
	return &x;
}

X_INLINE void xVec3::NormalVectors(xVec3 &left, xVec3 &down) const {
	double d = x * x + y * y;
	if (!d) {
		left[0] = 1;
		left[1] = 0;
		left[2] = 0;
	} else {
		d = xMath::InvSqrt64(d);
		left[0] = -y * d;
		left[1] = x * d;
		left[2] = 0;
	}
	down = left.Cross(*this);
}

X_INLINE void xVec3::OrthogonalBasis(xVec3 &left, xVec3 &up) const {
	double l, s;

	if (xMath::Fabs(z) > 0.7f) {
		l = y * y + z * z;
		s = xMath::InvSqrt64(l);
		up[0] = 0;
		up[1] = z * s;
		up[2] = -y * s;
		left[0] = l * s;
		left[1] = -x * up[2];
		left[2] = x * up[1];
	}
	else {
		l = x * x + y * y;
		s = xMath::InvSqrt64(l);
		left[0] = -y * s;
		left[1] = x * s;
		left[2] = 0;
		up[0] = -z * left[1];
		up[1] = z * left[0];
		up[2] = l * s;
	}
}

X_INLINE void xVec3::ProjectOntoPlane(const xVec3 &normal, float overBounce) {
	float backoff;
	
	backoff = *this * normal;
	
	if (overBounce != 1.0) {
		if (backoff < 0) {
			backoff *= overBounce;
		} else {
			backoff /= overBounce;
		}
	}

	*this -= backoff * normal;
}

X_INLINE bool xVec3::ProjectAlongPlane(const xVec3 &normal, float epsilon, float overBounce) {
	xVec3 cross;
	float len;

	cross = this->Cross(normal).Cross((*this));
	// normalize so a fixed epsilon can be used
	cross.Normalize();
	len = normal * cross;
	if (xMath::Fabs(len) < epsilon) {
		return false;
	}
	cross *= overBounce * (normal * (*this)) / len;
	(*this) -= cross;
	return true;
}


//===============================================================
//
//	xVec3d - 3D vector
//
//===============================================================

class xVec3d {
public:	
	double			x;
	double			y;
	double			z;

	xVec3d();
	explicit xVec3d(double x, double y, double z);

	void 			Set(double x, double y, double z);
	void			Zero();

	double			operator[](const int index) const;
	double &		operator[](const int index);
	xVec3d			operator-() const;
	xVec3d &		operator=(const xVec3d &a);		// required because of a msvc 6 & 7 bug
	double 		  operator*(const xVec3d &a) const;
	xVec3d			operator*(double a) const;
	xVec3d			operator/(double a) const;
	xVec3d			operator+(const xVec3d &a) const;
	xVec3d			operator-(const xVec3d &a) const;
	xVec3d &		operator+=(const xVec3d &a);
	xVec3d &		operator-=(const xVec3d &a);
	xVec3d &		operator/=(const xVec3d &a);
	xVec3d &		operator/=(double a);
	xVec3d &		operator*=(double a);

	friend xVec3d	operator*(double a, const xVec3d b);
  xVec3d     Mul(const xVec3d& b) const;
  xVec3d     Div(const xVec3d& b) const;

	bool			Compare(const xVec3d &a) const;							// exact compare, no epsilon
	bool			Compare(const xVec3d &a, double epsilon) const;		// compare with epsilon
	bool			operator==(	const xVec3d &a) const;						// exact compare, no epsilon
	bool			operator!=(	const xVec3d &a) const;						// exact compare, no epsilon

	bool			FixDegenerateNormal();	// fix degenerate axial cases
	bool			FixDenormals();			// change tiny numbers to zero

	xVec3d		Cross(const xVec3d &a) const;
	xVec3d &	Cross(const xVec3d &a, const xVec3d &b);
	double		Length() const;
	double		LengthSqr() const;
	double		LengthFast() const;
	double		Normalize();				// returns length
	double		NormalizeFast();			// returns length
  xVec3d    Norm() const;
  xVec3d    NormFast() const;
  xVec3d    Fabs() const;
	xVec3d &	Truncate(double length);		// cap length
	void			Clamp(const xVec3d &min, const xVec3d &max);
	void			Snap();					// snap to closest integer value
	void			SnapInt();				// snap towards integer (floor)

	int				Dimension() const;

	float		ToYaw() const;
	float		ToPitch() const;
	xAngles		ToAngles() const;
	xPolar3		ToPolar() const;
	xMat3			ToMat3() const;		// vector should be normalized
  xVec3     ToVec3() const { return xVec3(x,y,z); }
	const double *	ToFloatPtr() const;
	double *			ToFloatPtr();

	void			NormalVectors(xVec3d &left, xVec3d &down) const;	// vector should be normalized
  void			NormalVectors(xVec3 &left, xVec3 &down) const;	// vector should be normalized
	void			OrthogonalBasis(xVec3d &left, xVec3d &up) const;

	void			ProjectOntoPlane(const xVec3d &normal, double overBounce = 1.0f);
	bool			ProjectAlongPlane(const xVec3d &normal, double epsilon, double overBounce = 1.0f);
	void			ProjectSelfOntoSphere(double radius);

	void			Lerp(const xVec3d &v1, const xVec3d &v2, double l);
	void			SLerp(const xVec3d &v1, const xVec3d &v2, double l);
};

extern const xVec3d vec3d_origin;
#define vec3d_zero vec3d_origin

X_INLINE xVec3d::xVec3d() {
}

X_INLINE xVec3d::xVec3d(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

X_INLINE double xVec3d::operator[](const int index) const {
	return (&x)[ index ];
}

X_INLINE double &xVec3d::operator[](const int index) {
	return (&x)[ index ];
}

X_INLINE void xVec3d::Set(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

X_INLINE void xVec3d::Zero() {
	x = y = z = 0.0f;
}

X_INLINE xVec3d xVec3d::operator-() const {
	return xVec3d(-x, -y, -z);
}

X_INLINE xVec3d &xVec3d::operator=(const xVec3d &a) {
	x = a.x;
	y = a.y;
	z = a.z;
	return *this;
}

X_INLINE xVec3d xVec3d::operator-(const xVec3d &a) const {
	return xVec3d(x - a.x, y - a.y, z - a.z);
}

X_INLINE double xVec3d::operator*(const xVec3d &a) const {
	return x * a.x + y * a.y + z * a.z;
}

X_INLINE xVec3d xVec3d::operator*(double a) const {
	return xVec3d(x * a, y * a, z * a);
}

X_INLINE xVec3d xVec3d::operator/(double a) const {
	double inva = 1.0f / a;
	return xVec3d(x * inva, y * inva, z * inva);
}

X_INLINE xVec3d operator*(double a, const xVec3d b) {
	return xVec3d(b.x * a, b.y * a, b.z * a);
}

X_INLINE xVec3d xVec3d::operator+(const xVec3d &a) const {
	return xVec3d(x + a.x, y + a.y, z + a.z);
}

X_INLINE xVec3d &xVec3d::operator+=(const xVec3d &a) {
	x += a.x;
	y += a.y;
	z += a.z;

	return *this;
}

X_INLINE xVec3d &xVec3d::operator/=(const xVec3d &a) {
	x /= a.x;
	y /= a.y;
	z /= a.z;

	return *this;
}

X_INLINE xVec3d &xVec3d::operator/=(double a) {
	double inva = 1.0f / a;
	x *= inva;
	y *= inva;
	z *= inva;

	return *this;
}

X_INLINE xVec3d &xVec3d::operator-=(const xVec3d &a) {
	x -= a.x;
	y -= a.y;
	z -= a.z;

	return *this;
}

X_INLINE xVec3d &xVec3d::operator*=(double a) {
	x *= a;
	y *= a;
	z *= a;

	return *this;
}

X_INLINE xVec3d xVec3d::Mul(const xVec3d& b) const
{
  return xVec3d(x*b.x, y*b.y, z*b.z);
}

X_INLINE xVec3d xVec3d::Div(const xVec3d& b) const
{
  return xVec3d(x/b.x, y/b.y, z/b.z);
}

X_INLINE bool xVec3d::Compare(const xVec3d &a) const {
	return ((x == a.x) && (y == a.y) && (z == a.z));
}

X_INLINE bool xVec3d::Compare(const xVec3d &a, double epsilon) const {
	if (xMath::Fabs(x - a.x) > epsilon) {
		return false;
	}
			
	if (xMath::Fabs(y - a.y) > epsilon) {
		return false;
	}

	if (xMath::Fabs(z - a.z) > epsilon) {
		return false;
	}

	return true;
}

X_INLINE bool xVec3d::operator==(const xVec3d &a) const {
	return Compare(a);
}

X_INLINE bool xVec3d::operator!=(const xVec3d &a) const {
	return !Compare(a);
}

X_INLINE xVec3d xVec3d::Norm() const
{
	double invLength = 1.0 / Length();
	return xVec3d(x * invLength, y * invLength, z * invLength);
}
X_INLINE xVec3d xVec3d::Fabs() const
{
  return xVec3d(fabs(x), fabs(y), fabs(z));
}

X_INLINE bool xVec3d::FixDegenerateNormal() {
	if (x == 0.0f) {
		if (y == 0.0f) {
			if (z > 0.0f) {
				if (z != 1.0f) {
					z = 1.0f;
					return true;
				}
			} else {
				if (z != -1.0f) {
					z = -1.0f;
					return true;
				}
			}
			return false;
		} else if (z == 0.0f) {
			if (y > 0.0f) {
				if (y != 1.0f) {
					y = 1.0f;
					return true;
				}
			} else {
				if (y != -1.0f) {
					y = -1.0f;
					return true;
				}
			}
			return false;
		}
	} else if (y == 0.0f) {
		if (z == 0.0f) {
			if (x > 0.0f) {
				if (x != 1.0f) {
					x = 1.0f;
					return true;
				}
			} else {
				if (x != -1.0f) {
					x = -1.0f;
					return true;
				}
			}
			return false;
		}
	}
	if (fabs(x) == 1.0f) {
		if (y != 0.0f || z != 0.0f) {
			y = z = 0.0f;
			return true;
		}
		return false;
	} else if (fabs(y) == 1.0f) {
		if (x != 0.0f || z != 0.0f) {
			x = z = 0.0f;
			return true;
		}
		return false;
	} else if (fabs(z) == 1.0f) {
		if (x != 0.0f || y != 0.0f) {
			x = y = 0.0f;
			return true;
		}
		return false;
	}
	return false;
}

X_INLINE bool xVec3d::FixDenormals() {
	bool denormal = false;
	if (fabs(x) < 1e-30f) {
		x = 0.0f;
		denormal = true;
	}
	if (fabs(y) < 1e-30f) {
		y = 0.0f;
		denormal = true;
	}
	if (fabs(z) < 1e-30f) {
		z = 0.0f;
		denormal = true;
	}
	return denormal;
}

X_INLINE xVec3d xVec3d::Cross(const xVec3d &a) const {
	return xVec3d(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x);
}

X_INLINE xVec3d &xVec3d::Cross(const xVec3d &a, const xVec3d &b) {
	x = a.y * b.z - a.z * b.y;
	y = a.z * b.x - a.x * b.z;
	z = a.x * b.y - a.y * b.x;

	return *this;
}

X_INLINE double xVec3d::Length() const {
	return sqrt(x * x + y * y + z * z);
}

X_INLINE double xVec3d::LengthSqr() const {
	return x * x + y * y + z * z;
}

X_INLINE double xVec3d::Normalize() {
	double length = Length();
	double invLength = 1.0 / length;
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return length;
}

X_INLINE xVec3d &xVec3d::Truncate(double length) {
	if (!length) {
		Zero();
	}
	else {
		double length2 = LengthSqr();
		if (length2 > length * length) {
			double ilength = length / sqrt(length2);
			x *= ilength;
			y *= ilength;
			z *= ilength;
		}
	}

	return *this;
}

X_INLINE void xVec3d::Clamp(const xVec3d &min, const xVec3d &max) {
	if (x < min.x) {
		x = min.x;
	} else if (x > max.x) {
		x = max.x;
	}
	if (y < min.y) {
		y = min.y;
	} else if (y > max.y) {
		y = max.y;
	}
	if (z < min.z) {
		z = min.z;
	} else if (z > max.z) {
		z = max.z;
	}
}

X_INLINE void xVec3d::Snap() {
	x = floor(x + 0.5f);
	y = floor(y + 0.5f);
	z = floor(z + 0.5f);
}

X_INLINE void xVec3d::SnapInt() {
	x = double(int(x));
	y = double(int(y));
	z = double(int(z));
}

X_INLINE int xVec3d::Dimension() const {
	return 3;
}

X_INLINE const double *xVec3d::ToFloatPtr() const {
	return &x;
}

X_INLINE double *xVec3d::ToFloatPtr() {
	return &x;
}

X_INLINE void xVec3d::NormalVectors(xVec3d &left, xVec3d &down) const {
	double d = x * x + y * y;
	if (!d) {
		left[0] = 1;
		left[1] = 0;
		left[2] = 0;
	} else {
		d = 1.0 / sqrt(d);
		left[0] = -y * d;
		left[1] = x * d;
		left[2] = 0;
	}
	down = left.Cross(*this);
}

X_INLINE void xVec3d::NormalVectors(xVec3 &left, xVec3 &down) const {
  double d = x * x + y * y;
  if (!d) {
    left[0] = 1;
    left[1] = 0;
    left[2] = 0;
  } else {
    d = 1.0 / sqrt(d);
    left[0] = -y * d;
    left[1] = x * d;
    left[2] = 0;
  }
  down = left.Cross(this->ToVec3());
}

X_INLINE void xVec3d::OrthogonalBasis(xVec3d &left, xVec3d &up) const {
	double l, s;

	if (xMath::Fabs(z) > 0.7f) {
		l = y * y + z * z;
		s = 1.0 / sqrt(l);
		up[0] = 0;
		up[1] = z * s;
		up[2] = -y * s;
		left[0] = l * s;
		left[1] = -x * up[2];
		left[2] = x * up[1];
	}
	else {
		l = x * x + y * y;
		s = 1.0 / sqrt(l);
		left[0] = -y * s;
		left[1] = x * s;
		left[2] = 0;
		up[0] = -z * left[1];
		up[1] = z * left[0];
		up[2] = l * s;
	}
}

X_INLINE void xVec3d::ProjectOntoPlane(const xVec3d &normal, double overBounce) {
	double backoff;
	
	backoff = *this * normal;
	
	if (overBounce != 1.0) {
		if (backoff < 0) {
			backoff *= overBounce;
		} else {
			backoff /= overBounce;
		}
	}

	*this -= backoff * normal;
}

X_INLINE bool xVec3d::ProjectAlongPlane(const xVec3d &normal, double epsilon, double overBounce) {
	xVec3d cross;
	double len;

	cross = this->Cross(normal).Cross((*this));
	// normalize so a fixed epsilon can be used
	cross.Normalize();
	len = normal * cross;
	if (fabs(len) < epsilon) {
		return false;
	}
	cross *= overBounce * (normal * (*this)) / len;
	(*this) -= cross;
	return true;
}


//===============================================================
//
//	xVec4 - 4D vector
//
//===============================================================

class xVec4 {
public:	
	float			x;
	float			y;
	float			z;
	float			w;

					xVec4();
					explicit xVec4(float x, float y, float z, float w);

	void 			Set(float x, float y, float z, float w);
	void			Zero();

	float			operator[](const int index) const;
	float &			operator[](const int index);
	xVec4			operator-() const;
	double 		operator*(const xVec4 &a) const;
	xVec4			operator*(float a) const;
	xVec4			operator/(float a) const;
	xVec4			operator+(const xVec4 &a) const;
	xVec4			operator-(const xVec4 &a) const;
	xVec4 &		operator+=(const xVec4 &a);
	xVec4 &		operator-=(const xVec4 &a);
	xVec4 &		operator/=(const xVec4 &a);
	xVec4 &		operator/=(float a);
	xVec4 &		operator*=(float a);

	friend xVec4	operator*(float a, const xVec4 b);

	bool			Compare(const xVec4 &a) const;							// exact compare, no epsilon
	bool			Compare(const xVec4 &a, float epsilon) const;		// compare with epsilon
	bool			operator==(	const xVec4 &a) const;						// exact compare, no epsilon
	bool			operator!=(	const xVec4 &a) const;						// exact compare, no epsilon

	float			Length() const;
	float			LengthSqr() const;
	float			Normalize();			// returns length
	float			NormalizeFast();		// returns length
  xVec4     Norm();
  xVec4     NormFast();
	int				Dimension() const;

	const xVec2 &	ToVec2() const;
	xVec2 &		ToVec2();
	const xVec3 &	ToVec3() const;
	xVec3 &		ToVec3();
	const float *	ToFloatPtr() const;
	float *	ToFloatPtr();
	xString ToString(int precision = 2) const;

	void			Lerp(const xVec4 &v1, const xVec4 &v2, float l);
};

extern const xVec4 vec4_origin;
#define vec4_zero vec4_origin

X_INLINE xVec4::xVec4() {
}

X_INLINE xVec4::xVec4(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

X_INLINE void xVec4::Set(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

X_INLINE void xVec4::Zero() {
	x = y = z = w = 0.0f;
}

X_INLINE float xVec4::operator[](int index) const {
	return (&x)[ index ];
}

X_INLINE float& xVec4::operator[](int index) {
	return (&x)[ index ];
}

X_INLINE xVec4 xVec4::operator-() const {
	return xVec4(-x, -y, -z, -w);
}

X_INLINE xVec4 xVec4::operator-(const xVec4 &a) const {
	return xVec4(x - a.x, y - a.y, z - a.z, w - a.w);
}

X_INLINE double xVec4::operator*(const xVec4 &a) const {
	return x * a.x + y * a.y + z * a.z + w * a.w;
}

X_INLINE xVec4 xVec4::operator*(float a) const {
	return xVec4(x * a, y * a, z * a, w * a);
}

X_INLINE xVec4 xVec4::operator/(float a) const {
	float inva = 1.0f / a;
	return xVec4(x * inva, y * inva, z * inva, w * inva);
}

X_INLINE xVec4 operator*(float a, const xVec4 b) {
	return xVec4(b.x * a, b.y * a, b.z * a, b.w * a);
}

X_INLINE xVec4 xVec4::operator+(const xVec4 &a) const {
	return xVec4(x + a.x, y + a.y, z + a.z, w + a.w);
}

X_INLINE xVec4 &xVec4::operator+=(const xVec4 &a) {
	x += a.x;
	y += a.y;
	z += a.z;
	w += a.w;

	return *this;
}

X_INLINE xVec4 &xVec4::operator/=(const xVec4 &a) {
	x /= a.x;
	y /= a.y;
	z /= a.z;
	w /= a.w;

	return *this;
}

X_INLINE xVec4 &xVec4::operator/=(float a) {
	float inva = 1.0f / a;
	x *= inva;
	y *= inva;
	z *= inva;
	w *= inva;

	return *this;
}

X_INLINE xVec4 &xVec4::operator-=(const xVec4 &a) {
	x -= a.x;
	y -= a.y;
	z -= a.z;
	w -= a.w;

	return *this;
}

X_INLINE xVec4 &xVec4::operator*=(float a) {
	x *= a;
	y *= a;
	z *= a;
	w *= a;

	return *this;
}

X_INLINE bool xVec4::Compare(const xVec4 &a) const {
	return ((x == a.x) && (y == a.y) && (z == a.z) && w == a.w);
}

X_INLINE bool xVec4::Compare(const xVec4 &a, float epsilon) const {
	if (xMath::Fabs(x - a.x) > epsilon) {
		return false;
	}
			
	if (xMath::Fabs(y - a.y) > epsilon) {
		return false;
	}

	if (xMath::Fabs(z - a.z) > epsilon) {
		return false;
	}

	if (xMath::Fabs(w - a.w) > epsilon) {
		return false;
	}

	return true;
}

X_INLINE bool xVec4::operator==(const xVec4 &a) const {
	return Compare(a);
}

X_INLINE bool xVec4::operator!=(const xVec4 &a) const {
	return !Compare(a);
}

X_INLINE float xVec4::Length() const {
	return (float)xMath::Sqrt(x * x + y * y + z * z + w * w);
}

X_INLINE float xVec4::LengthSqr() const {
	return (x * x + y * y + z * z + w * w);
}

X_INLINE float xVec4::Normalize() {
	double sqrLength, invLength;

	sqrLength = x * x + y * y + z * z + w * w;
	invLength = xMath::InvSqrt64(sqrLength);
	x *= invLength;
	y *= invLength;
	z *= invLength;
	w *= invLength;
	return invLength * sqrLength;
}

X_INLINE float xVec4::NormalizeFast() {
	float sqrLength, invLength;

	sqrLength = x * x + y * y + z * z + w * w;
	invLength = xMath::RSqrt(sqrLength);
	x *= invLength;
	y *= invLength;
	z *= invLength;
	w *= invLength;
	return invLength * sqrLength;
}

X_INLINE xVec4 xVec4::Norm()
{
	double invLength = xMath::InvSqrt64(x * x + y * y + z * z + w * w);
	return xVec4(x * invLength, y * invLength, z * invLength, w * invLength);
}
X_INLINE xVec4 xVec4::NormFast()
{
	float invLength = xMath::RSqrt(x * x + y * y + z * z + w * w);
	return xVec4(x * invLength, y * invLength, z * invLength, w * invLength);
}

X_INLINE int xVec4::Dimension() const {
	return 4;
}

X_INLINE const xVec2 &xVec4::ToVec2() const {
	return *(const xVec2*)this;
}

X_INLINE xVec2 &xVec4::ToVec2() {
	return *(xVec2*)this;
}

X_INLINE const xVec3 &xVec4::ToVec3() const {
	return *(const xVec3*)this;
}

X_INLINE xVec3 &xVec4::ToVec3() {
	return *(xVec3*)this;
}

X_INLINE const float *xVec4::ToFloatPtr() const {
	return &x;
}

X_INLINE float *xVec4::ToFloatPtr() {
	return &x;
}


//===============================================================
//
//	xVec5 - 5D vector
//
//===============================================================

class xVec5 {
public:
	float			x;
	float			y;
	float			z;
	float			s;
	float			t;

					xVec5();
					explicit xVec5(const xVec3 &xyz, const xVec2 &st);
					explicit xVec5(float x, float y, float z, float s, float t);

	float			operator[](int index) const;
	float &			operator[](int index);
	xVec5 &		operator=(const xVec3 &a);

	int				Dimension() const;

	const xVec3 &	ToVec3() const;
	xVec3 &		ToVec3();
	const float *	ToFloatPtr() const;
	float *			ToFloatPtr();
	xString ToString(int precision = 2) const;

	void			Lerp(const xVec5 &v1, const xVec5 &v2, float l);
};

extern const xVec5 vec5_origin;
#define vec5_zero vec5_origin

X_INLINE xVec5::xVec5() {
}

X_INLINE xVec5::xVec5(const xVec3 &xyz, const xVec2 &st) {
	x = xyz.x;
	y = xyz.y;
	z = xyz.z;
	s = st[0];
	t = st[1];
}

X_INLINE xVec5::xVec5(float x, float y, float z, float s, float t) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->s = s;
	this->t = t;
}

X_INLINE float xVec5::operator[](int index) const {
	return (&x)[ index ];
}

X_INLINE float& xVec5::operator[](int index) {
	return (&x)[ index ];
}

X_INLINE xVec5 &xVec5::operator=(const xVec3 &a) { 
	x = a.x;
	y = a.y;
	z = a.z;
	s = t = 0;
	return *this;
}

X_INLINE int xVec5::Dimension() const {
	return 5;
}

X_INLINE const xVec3 &xVec5::ToVec3() const {
	return *(const xVec3*)this;
}

X_INLINE xVec3 &xVec5::ToVec3() {
	return *(xVec3*)this;
}

X_INLINE const float *xVec5::ToFloatPtr() const {
	return &x;
}

X_INLINE float *xVec5::ToFloatPtr() {
	return &x;
}


//===============================================================
//
//	xVec6 - 6D vector
//
//===============================================================

class xVec6 {
public:	
					xVec6();
					explicit xVec6(float *a);
					explicit xVec6(float a1, float a2, float a3, float a4, float a5, float a6);

	void 			Set(float a1, float a2, float a3, float a4, float a5, float a6);
	void			Zero();

	float			operator[](const int index) const;
	float &			operator[](const int index);
	xVec6			operator-() const;
	xVec6			operator*(float a) const;
	xVec6			operator/(float a) const;
	double 		operator*(const xVec6 &a) const;
	xVec6			operator-(const xVec6 &a) const;
	xVec6			operator+(const xVec6 &a) const;
	xVec6 &		operator*=(float a);
	xVec6 &		operator/=(float a);
	xVec6 &		operator+=(const xVec6 &a);
	xVec6 &		operator-=(const xVec6 &a);

	friend xVec6	operator*(float a, const xVec6 b);

	bool			Compare(const xVec6 &a) const;							// exact compare, no epsilon
	bool			Compare(const xVec6 &a, float epsilon) const;		// compare with epsilon
	bool			operator==(	const xVec6 &a) const;						// exact compare, no epsilon
	bool			operator!=(	const xVec6 &a) const;						// exact compare, no epsilon

	float			Length() const;
	float			LengthSqr() const;
	float			Normalize();			// returns length
	float			NormalizeFast();		// returns length

	int				Dimension() const;

	const xVec3 &	SubVec3(int index) const;
	xVec3 &		SubVec3(int index);
	const float *	ToFloatPtr() const;
	float *			ToFloatPtr();
	xString ToString(int precision = 2) const;

private:
	float			p[6];
};

extern const xVec6 vec6_origin;
#define vec6_zero vec6_origin
extern const xVec6 vec6_infinity;

X_INLINE xVec6::xVec6() {
}

X_INLINE xVec6::xVec6(float *a) {
	memcpy(p, a, 6 * sizeof(float));
}

X_INLINE xVec6::xVec6(float a1, float a2, float a3, float a4, float a5, float a6) {
	p[0] = a1;
	p[1] = a2;
	p[2] = a3;
	p[3] = a4;
	p[4] = a5;
	p[5] = a6;
}

X_INLINE xVec6 xVec6::operator-() const {
	return xVec6(-p[0], -p[1], -p[2], -p[3], -p[4], -p[5]);
}

X_INLINE float xVec6::operator[](const int index) const {
	return p[index];
}

X_INLINE float &xVec6::operator[](const int index) {
	return p[index];
}

X_INLINE xVec6 xVec6::operator*(float a) const {
	return xVec6(p[0]*a, p[1]*a, p[2]*a, p[3]*a, p[4]*a, p[5]*a);
}

X_INLINE double xVec6::operator*(const xVec6 &a) const {
	return p[0] * a[0] + p[1] * a[1] + p[2] * a[2] + p[3] * a[3] + p[4] * a[4] + p[5] * a[5];
}

X_INLINE xVec6 xVec6::operator/(float a) const {
	float inva;

	assert(a != 0.0f);
	inva = 1.0f / a;
	return xVec6(p[0]*inva, p[1]*inva, p[2]*inva, p[3]*inva, p[4]*inva, p[5]*inva);
}

X_INLINE xVec6 xVec6::operator+(const xVec6 &a) const {
	return xVec6(p[0] + a[0], p[1] + a[1], p[2] + a[2], p[3] + a[3], p[4] + a[4], p[5] + a[5]);
}

X_INLINE xVec6 xVec6::operator-(const xVec6 &a) const {
	return xVec6(p[0] - a[0], p[1] - a[1], p[2] - a[2], p[3] - a[3], p[4] - a[4], p[5] - a[5]);
}

X_INLINE xVec6 &xVec6::operator*=(float a) {
	p[0] *= a;
	p[1] *= a;
	p[2] *= a;
	p[3] *= a;
	p[4] *= a;
	p[5] *= a;
	return *this;
}

X_INLINE xVec6 &xVec6::operator/=(float a) {
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

X_INLINE xVec6 &xVec6::operator+=(const xVec6 &a) {
	p[0] += a[0];
	p[1] += a[1];
	p[2] += a[2];
	p[3] += a[3];
	p[4] += a[4];
	p[5] += a[5];
	return *this;
}

X_INLINE xVec6 &xVec6::operator-=(const xVec6 &a) {
	p[0] -= a[0];
	p[1] -= a[1];
	p[2] -= a[2];
	p[3] -= a[3];
	p[4] -= a[4];
	p[5] -= a[5];
	return *this;
}

X_INLINE xVec6 operator*(float a, const xVec6 b) {
	return b * a;
}

X_INLINE bool xVec6::Compare(const xVec6 &a) const {
	return ((p[0] == a[0]) && (p[1] == a[1]) && (p[2] == a[2]) &&
			(p[3] == a[3]) && (p[4] == a[4]) && (p[5] == a[5]));
}

X_INLINE bool xVec6::Compare(const xVec6 &a, float epsilon) const {
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

X_INLINE bool xVec6::operator==(const xVec6 &a) const {
	return Compare(a);
}

X_INLINE bool xVec6::operator!=(const xVec6 &a) const {
	return !Compare(a);
}

X_INLINE void xVec6::Set(float a1, float a2, float a3, float a4, float a5, float a6) {
	p[0] = a1;
	p[1] = a2;
	p[2] = a3;
	p[3] = a4;
	p[4] = a5;
	p[5] = a6;
}

X_INLINE void xVec6::Zero() {
	p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = 0.0f;
}

X_INLINE float xVec6::Length() const {
	return (float)xMath::Sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5]);
}

X_INLINE float xVec6::LengthSqr() const {
	return (p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5]);
}

X_INLINE float xVec6::Normalize() {
	double sqrLength, invLength;

	sqrLength = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5];
	invLength = xMath::InvSqrt64(sqrLength);
	p[0] *= invLength;
	p[1] *= invLength;
	p[2] *= invLength;
	p[3] *= invLength;
	p[4] *= invLength;
	p[5] *= invLength;
	return invLength * sqrLength;
}

X_INLINE float xVec6::NormalizeFast() {
	float sqrLength, invLength;

	sqrLength = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5];
	invLength = xMath::RSqrt(sqrLength);
	p[0] *= invLength;
	p[1] *= invLength;
	p[2] *= invLength;
	p[3] *= invLength;
	p[4] *= invLength;
	p[5] *= invLength;
	return invLength * sqrLength;
}

X_INLINE int xVec6::Dimension() const {
	return 6;
}

X_INLINE const xVec3 &xVec6::SubVec3(int index) const {
	return *(const xVec3*)(p + index * 3);
}

X_INLINE xVec3 &xVec6::SubVec3(int index) {
	return *(xVec3*)(p + index * 3);
}

X_INLINE const float *xVec6::ToFloatPtr() const {
	return p;
}

X_INLINE float *xVec6::ToFloatPtr() {
	return p;
}

//===============================================================
//
//	xPolar3
//
//===============================================================

class xPolar3 {
public:	
	float			radius, theta, phi;

					xPolar3();
					explicit xPolar3(float radius, float theta, float phi);

	void 			Set(float radius, float theta, float phi);

	float			operator[](const int index) const;
	float &			operator[](const int index);
	xPolar3		operator-() const;
	xPolar3 &		operator=(const xPolar3 &a);

	xVec3			ToVec3() const;
};

X_INLINE xPolar3::xPolar3() {
}

X_INLINE xPolar3::xPolar3(float radius, float theta, float phi) {
	assert(radius > 0);
	this->radius = radius;
	this->theta = theta;
	this->phi = phi;
}
	
X_INLINE void xPolar3::Set(float radius, float theta, float phi) {
	assert(radius > 0);
	this->radius = radius;
	this->theta = theta;
	this->phi = phi;
}

X_INLINE float xPolar3::operator[](const int index) const {
	return (&radius)[ index ];
}

X_INLINE float &xPolar3::operator[](const int index) {
	return (&radius)[ index ];
}

X_INLINE xPolar3 xPolar3::operator-() const {
	return xPolar3(radius, -theta, -phi);
}

X_INLINE xPolar3 &xPolar3::operator=(const xPolar3 &a) { 
	radius = a.radius;
	theta = a.theta;
	phi = a.phi;
	return *this;
}

X_INLINE xVec3 xPolar3::ToVec3() const {
	float sp, cp, st, ct;
	xMath::SinCos(phi, sp, cp);
	xMath::SinCos(theta, st, ct);
 	return xVec3(cp * radius * ct, cp * radius * st, radius * sp);
}


/*
===============================================================================

	Old 3D vector macros, should no longer be used.

===============================================================================
*/

#define DotProduct(a, b)			((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VectorSubtract(a, b, c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd(a, b, c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define	VectorScale(v, s, o)		((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define	VectorMA(v, s, b, o)		((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))
#define VectorCopy(a, b)			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])


#endif /* !__X_VECTOR_H__ */
