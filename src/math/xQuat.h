#ifndef __X_QUAT_H__
#define __X_QUAT_H__

#pragma once

/*
===============================================================================

	Quaternion

===============================================================================
*/


class xVec3;
class xAngles;
class xRotation;
class xMat3;
class xMat4;
class xCQuat;

class xQuat {
public:
	float			x;
	float			y;
	float			z;
	float			w;

					xQuat();
					xQuat(float x, float y, float z, float w);

	void 			Set(float x, float y, float z, float w);

	float			operator[](int index) const;
	float &			operator[](int index);
	xQuat			operator-() const;
	xQuat &		operator=(const xQuat &a);
	xQuat			operator+(const xQuat &a) const;
	xQuat &		operator+=(const xQuat &a);
	xQuat			operator-(const xQuat &a) const;
	xQuat &		operator-=(const xQuat &a);
	xQuat			operator*(const xQuat &a) const;
	xVec3			operator*(const xVec3 &a) const;
	xQuat			operator*(float a) const;
	xQuat &		operator*=(const xQuat &a);
	xQuat &		operator*=(float a);

	friend xQuat	operator*(float a, const xQuat &b);
	friend xVec3	operator*(const xVec3 &a, const xQuat &b);

	bool			Compare(const xQuat &a) const;						// exact compare, no epsilon
	bool			Compare(const xQuat &a, float epsilon) const;	// compare with epsilon
	bool			operator==(	const xQuat &a) const;					// exact compare, no epsilon
	bool			operator!=(	const xQuat &a) const;					// exact compare, no epsilon

	xQuat			Inverse() const;
	float			Length() const;
	xQuat &		Normalize();

	float			CalcW() const;
	int				Dimension() const;

	xAngles		ToAngles() const;
	xRotation	ToRotation() const;
	xMat3			ToMat3() const;
	xMat4			ToMat4() const;
	xCQuat		ToCQuat() const;
	xVec3			ToAngularVelocity() const;
	const float *	ToFloatPtr() const;
	float *		ToFloatPtr();
	xString ToString(int precision = 2) const;

	xQuat     SlerpTo(const xQuat &to, float t) const;
	xQuat &		SlerpFrom(const xQuat &from, const xQuat &to, float t);
	static xQuat Slerp(const xQuat &from, const xQuat &to, float t);
};

X_INLINE xQuat::xQuat() {
}

X_INLINE xQuat::xQuat(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

X_INLINE float xQuat::operator[](int index) const {
	assert((index >= 0) && (index < 4));
	return (&x)[ index ];
}

X_INLINE float& xQuat::operator[](int index) {
	assert((index >= 0) && (index < 4));
	return (&x)[ index ];
}

X_INLINE xQuat xQuat::operator-() const {
	return xQuat(-x, -y, -z, -w);
}

X_INLINE xQuat &xQuat::operator=(const xQuat &a) {
	x = a.x;
	y = a.y;
	z = a.z;
	w = a.w;

	return *this;
}

X_INLINE xQuat xQuat::operator+(const xQuat &a) const {
	return xQuat(x + a.x, y + a.y, z + a.z, w + a.w);
}

X_INLINE xQuat& xQuat::operator+=(const xQuat &a) {
	x += a.x;
	y += a.y;
	z += a.z;
	w += a.w;

	return *this;
}

X_INLINE xQuat xQuat::operator-(const xQuat &a) const {
	return xQuat(x - a.x, y - a.y, z - a.z, w - a.w);
}

X_INLINE xQuat& xQuat::operator-=(const xQuat &a) {
	x -= a.x;
	y -= a.y;
	z -= a.z;
	w -= a.w;

	return *this;
}

X_INLINE xQuat xQuat::operator*(const xQuat &a) const {
	return xQuat(	w*a.x + x*a.w + y*a.z - z*a.y,
					w*a.y + y*a.w + z*a.x - x*a.z,
					w*a.z + z*a.w + x*a.y - y*a.x,
					w*a.w - x*a.x - y*a.y - z*a.z);
}

X_INLINE xVec3 xQuat::operator*(const xVec3 &a) const {
#if 0
	// it's faster to do the conversion to a 3x3 matrix and multiply the vector by this 3x3 matrix
	return (ToMat3() * a);
#else
	// result = this->Inverse() * xQuat(a.x, a.y, a.z, 0.0f) * (*this)
	float xxzz = x*x - z*z;
	float wwyy = w*w - y*y;

	float xw2 = x*w*2.0f;
	float xy2 = x*y*2.0f;
	float xz2 = x*z*2.0f;
	float yw2 = y*w*2.0f;
	float yz2 = y*z*2.0f;
	float zw2 = z*w*2.0f;

	return xVec3(
		(xxzz + wwyy)*a.x		+ (xy2 + zw2)*a.y		+ (xz2 - yw2)*a.z,
		(xy2 - zw2)*a.x			+ (xxzz - wwyy)*a.y		+ (yz2 + xw2)*a.z,
		(xz2 + yw2)*a.x			+ (yz2 - xw2)*a.y		+ (wwyy - xxzz)*a.z
	);
#endif
}

X_INLINE xQuat xQuat::operator*(float a) const {
	return xQuat(x * a, y * a, z * a, w * a);
}

X_INLINE xQuat operator*(float a, const xQuat &b) {
	return b * a;
}

X_INLINE xVec3 operator*(const xVec3 &a, const xQuat &b) {
	return b * a;
}

X_INLINE xQuat& xQuat::operator*=(const xQuat &a) {
	*this = *this * a;

	return *this;
}

X_INLINE xQuat& xQuat::operator*=(float a) {
	x *= a;
	y *= a;
	z *= a;
	w *= a;

	return *this;
}

X_INLINE bool xQuat::Compare(const xQuat &a) const {
	return ((x == a.x) && (y == a.y) && (z == a.z) && (w == a.w));
}

X_INLINE bool xQuat::Compare(const xQuat &a, float epsilon) const {
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

X_INLINE bool xQuat::operator==(const xQuat &a) const {
	return Compare(a);
}

X_INLINE bool xQuat::operator!=(const xQuat &a) const {
	return !Compare(a);
}

X_INLINE void xQuat::Set(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

X_INLINE xQuat xQuat::Inverse() const {
	return xQuat(-x, -y, -z, w);
}

X_INLINE float xQuat::Length() const {
	float len;

	len = x * x + y * y + z * z + w * w;
	return xMath::Sqrt(len);
}

X_INLINE xQuat& xQuat::Normalize() {
	float len;
	float ilength;

	len = this->Length();
	if (len) {
		ilength = 1 / len;
		x *= ilength;
		y *= ilength;
		z *= ilength;
		w *= ilength;
	}
	return *this;
}

X_INLINE float xQuat::CalcW() const {
	// take the absolute value because floating point rounding may cause the dot of x,y,z to be larger than 1
	return sqrt(fabs(1.0f - (x * x + y * y + z * z)));
}

X_INLINE int xQuat::Dimension() const {
	return 4;
}

X_INLINE const float *xQuat::ToFloatPtr() const {
	return &x;
}

X_INLINE float *xQuat::ToFloatPtr() {
	return &x;
}


/*
===============================================================================

	Compressed quaternion

===============================================================================
*/

class xCQuat {
public:
	float			x;
	float			y;
	float			z;

					xCQuat();
					xCQuat(float x, float y, float z);

	void 			Set(float x, float y, float z);

	float			operator[](int index) const;
	float &			operator[](int index);

	bool			Compare(const xCQuat &a) const;						// exact compare, no epsilon
	bool			Compare(const xCQuat &a, float epsilon) const;	// compare with epsilon
	bool			operator==(	const xCQuat &a) const;					// exact compare, no epsilon
	bool			operator!=(	const xCQuat &a) const;					// exact compare, no epsilon

	int				Dimension() const;

	xAngles		ToAngles() const;
	xRotation		ToRotation() const;
	xMat3			ToMat3() const;
	xMat4			ToMat4() const;
	xQuat			ToQuat() const;
	const float *	ToFloatPtr() const;
	float *			ToFloatPtr();
	xString ToString(int precision = 2) const;
};

X_INLINE xCQuat::xCQuat() {
}

X_INLINE xCQuat::xCQuat(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

X_INLINE void xCQuat::Set(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

X_INLINE float xCQuat::operator[](int index) const {
	assert((index >= 0) && (index < 3));
	return (&x)[ index ];
}

X_INLINE float& xCQuat::operator[](int index) {
	assert((index >= 0) && (index < 3));
	return (&x)[ index ];
}

X_INLINE bool xCQuat::Compare(const xCQuat &a) const {
	return ((x == a.x) && (y == a.y) && (z == a.z));
}

X_INLINE bool xCQuat::Compare(const xCQuat &a, float epsilon) const {
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

X_INLINE bool xCQuat::operator==(const xCQuat &a) const {
	return Compare(a);
}

X_INLINE bool xCQuat::operator!=(const xCQuat &a) const {
	return !Compare(a);
}

X_INLINE int xCQuat::Dimension() const {
	return 3;
}

X_INLINE xQuat xCQuat::ToQuat() const {
	// take the absolute value because floating point rounding may cause the dot of x,y,z to be larger than 1
	return xQuat(x, y, z, sqrt(fabs(1.0f - (x * x + y * y + z * z))));
}

X_INLINE const float *xCQuat::ToFloatPtr() const {
	return &x;
}

X_INLINE float *xCQuat::ToFloatPtr() {
	return &x;
}

#endif /* !__X_QUAT_H__ */
