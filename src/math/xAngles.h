#ifndef __X_ANGLES_H__
#define __X_ANGLES_H__

#pragma once

/*
===============================================================================

	Euler angles

===============================================================================
*/

#include "../common/xString.h"
#include "xVector.h"

// angle indexes
#define	PITCH				0		// up / down
#define	YAW					1		// left / right
#define	ROLL				2		// fall over

class xVec3;
class xQuat;
class xRotation;
class xMat3;
class xMat4;

class xAngles {
public:
	float			pitch;
	float			yaw;
	float			roll;

	xAngles();
	xAngles(float pitch, float yaw, float roll);
	explicit xAngles(const xVec3 &v);
  xAngles(const xVec3& forwardDir, const xVec3& upDir);

	void 			Set(float pitch, float yaw, float roll);
	xAngles &		Zero();

	float			operator[](int index) const;
	float &			operator[](int index);
	xAngles		operator-() const;			// negate angles, in general not the inverse rotation
	xAngles &		operator=(const xAngles &a);
	xAngles		operator+(const xAngles &a) const;
	xAngles &		operator+=(const xAngles &a);
	xAngles		operator-(const xAngles &a) const;
	xAngles &		operator-=(const xAngles &a);
	xAngles		operator*(float a) const;
	xAngles &		operator*=(float a);
	xAngles		operator/(float a) const;
	xAngles &		operator/=(float a);

	friend xAngles	operator*(float a, const xAngles &b);

	bool			Compare(const xAngles &a) const;							// exact compare, no epsilon
	bool			Compare(const xAngles &a, float epsilon) const;	// compare with epsilon
	bool			operator==(	const xAngles &a) const;						// exact compare, no epsilon
	bool			operator!=(	const xAngles &a) const;						// exact compare, no epsilon

  static float Norm360(float a);
  static float Norm180(float a);

	xAngles&	Normalize360();	// normalizes 'this'
	xAngles&	Normalize180();	// normalizes 'this'
  
  xAngles   Norm360();
  xAngles   Norm180();

	void			Clamp(const xAngles &min, const xAngles &max);

	int				Dimension() const;

	void			ToVectors(xVec3 *forward, xVec3 *right = NULL, xVec3 *up = NULL) const;
  void      ToTexMaping(xVec3& xVec, xVec3& yVec, xVec3& up) const;
	xVec3			ToForward() const;
	xQuat			ToQuat() const;
	xRotation		ToRotation() const;
	xMat3			ToMat3() const;
	xMat4			ToMat4() const;
	xVec3			ToAngularVelocity() const;
	
  const float *	ToFloatPtr() const;
	float *			ToFloatPtr();
	xString ToString(int precision = 2) const;
};

extern const xAngles ang_zero;

X_INLINE xAngles::xAngles() {
}

X_INLINE xAngles::xAngles(float pitch, float yaw, float roll) {
	this->pitch = pitch;
	this->yaw	= yaw;
	this->roll	= roll;
}

X_INLINE xAngles::xAngles(const xVec3 &v) {
	this->pitch = v[0];
	this->yaw	= v[1];
	this->roll	= v[2];
}

X_INLINE void xAngles::Set(float pitch, float yaw, float roll) {
	this->pitch = pitch;
	this->yaw	= yaw;
	this->roll	= roll;
}

X_INLINE xAngles &xAngles::Zero() {
	pitch = yaw = roll = 0.0f;
	return *this;
}

X_INLINE float xAngles::operator[](int index) const {
	assert((index >= 0) && (index < 3));
	return (&pitch)[ index ];
}

X_INLINE float &xAngles::operator[](int index) {
	assert((index >= 0) && (index < 3));
	return (&pitch)[ index ];
}

X_INLINE xAngles xAngles::operator-() const {
	return xAngles(-pitch, -yaw, -roll);
}

X_INLINE xAngles &xAngles::operator=(const xAngles &a) {
	pitch	= a.pitch;
	yaw		= a.yaw;
	roll	= a.roll;
	return *this;
}

X_INLINE xAngles xAngles::operator+(const xAngles &a) const {
	return xAngles(pitch + a.pitch, yaw + a.yaw, roll + a.roll);
}

X_INLINE xAngles& xAngles::operator+=(const xAngles &a) {
	pitch	+= a.pitch;
	yaw		+= a.yaw;
	roll	+= a.roll;

	return *this;
}

X_INLINE xAngles xAngles::operator-(const xAngles &a) const {
	return xAngles(pitch - a.pitch, yaw - a.yaw, roll - a.roll);
}

X_INLINE xAngles& xAngles::operator-=(const xAngles &a) {
	pitch	-= a.pitch;
	yaw		-= a.yaw;
	roll	-= a.roll;

	return *this;
}

X_INLINE xAngles xAngles::operator*(float a) const {
	return xAngles(pitch * a, yaw * a, roll * a);
}

X_INLINE xAngles& xAngles::operator*=(float a) {
	pitch	*= a;
	yaw		*= a;
	roll	*= a;
	return *this;
}

X_INLINE xAngles xAngles::operator/(float a) const {
	float inva = 1.0f / a;
	return xAngles(pitch * inva, yaw * inva, roll * inva);
}

X_INLINE xAngles& xAngles::operator/=(float a) {
	float inva = 1.0f / a;
	pitch	*= inva;
	yaw		*= inva;
	roll	*= inva;
	return *this;
}

X_INLINE xAngles operator*(float a, const xAngles &b) {
	return xAngles(a * b.pitch, a * b.yaw, a * b.roll);
}

X_INLINE bool xAngles::Compare(const xAngles &a) const {
	return ((a.pitch == pitch) && (a.yaw == yaw) && (a.roll == roll));
}

X_INLINE bool xAngles::Compare(const xAngles &a, float epsilon) const {
	if (xMath::Fabs(pitch - a.pitch) > epsilon) {
		return false;
	}
			
	if (xMath::Fabs(yaw - a.yaw) > epsilon) {
		return false;
	}

	if (xMath::Fabs(roll - a.roll) > epsilon) {
		return false;
	}

	return true;
}

X_INLINE bool xAngles::operator==(const xAngles &a) const {
	return Compare(a);
}

X_INLINE bool xAngles::operator!=(const xAngles &a) const {
	return !Compare(a);
}

X_INLINE void xAngles::Clamp(const xAngles &min, const xAngles &max) {
	if (pitch < min.pitch) {
		pitch = min.pitch;
	} else if (pitch > max.pitch) {
		pitch = max.pitch;
	}
	if (yaw < min.yaw) {
		yaw = min.yaw;
	} else if (yaw > max.yaw) {
		yaw = max.yaw;
	}
	if (roll < min.roll) {
		roll = min.roll;
	} else if (roll > max.roll) {
		roll = max.roll;
	}
}

X_INLINE int xAngles::Dimension() const {
	return 3;
}

X_INLINE const float *xAngles::ToFloatPtr() const {
	return &pitch;
}

X_INLINE float *xAngles::ToFloatPtr() {
	return &pitch;
}

#endif /* !__X_ANGLES_H__ */
