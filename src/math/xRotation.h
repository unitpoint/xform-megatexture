#ifndef __X_ROTATION_H__
#define __X_ROTATION_H__

#pragma once

/*
===============================================================================

	Describes a complete rotation in degrees about an abritray axis.
	A local rotation matrix is stored for fast rotation of multiple points.

===============================================================================
*/


class xAngles;
class xQuat;
class xMat3;

class xRotation {

	friend class xAngles;
	friend class xQuat;
	friend class xMat3;

public:
						xRotation();
						xRotation(const xVec3 &rotationOrigin, const xVec3 &rotationVec, float rotationAngle);

	void				Set(const xVec3 &rotationOrigin, const xVec3 &rotationVec, float rotationAngle);
	void				SetOrigin(const xVec3 &rotationOrigin);
	void				SetVec(const xVec3 &rotationVec);					// has to be normalized
	void				SetVec(float x, float y, float z);	// has to be normalized
	void				SetAngle(float rotationAngle);
	void				Scale(float s);
	void				ReCalculateMatrix();
	const xVec3 &		Origin() const;
	const xVec3 &		Vec() const;
	float				Angle() const;

	xRotation			operator-() const;										// flips rotation
	xRotation			operator*(float s) const;						// scale rotation
	xRotation			operator/(float s) const;						// scale rotation
	xRotation &		operator*=(float s);							// scale rotation
	xRotation &		operator/=(float s);							// scale rotation
	xVec3				operator*(const xVec3 &v) const;						// rotate vector

	friend xRotation	operator*(float s, const xRotation &r);		// scale rotation
	friend xVec3		operator*(const xVec3 &v, const xRotation &r);		// rotate vector
	friend xVec3 &		operator*=(xVec3 &v, const xRotation &r);			// rotate vector

	xAngles			ToAngles() const;
	xQuat				ToQuat() const;
	const xMat3 &		ToMat3() const;
	xMat4				ToMat4() const;
	xVec3				ToAngularVelocity() const;

	void				RotatePoint(xVec3 &point) const;

	void				Normalize180();
	void				Normalize360();

private:
	xVec3				origin;			// origin of rotation
	xVec3				vec;			// normalized vector to rotate around
	float				angle;			// angle of rotation in degrees
	mutable xMat3		axis;			// rotation axis
	mutable bool		axisValid;		// true if rotation axis is valid
};


X_INLINE xRotation::xRotation() {
}

X_INLINE xRotation::xRotation(const xVec3 &rotationOrigin, const xVec3 &rotationVec, float rotationAngle) {
	origin = rotationOrigin;
	vec = rotationVec;
	angle = rotationAngle;
	axisValid = false;
}

X_INLINE void xRotation::Set(const xVec3 &rotationOrigin, const xVec3 &rotationVec, float rotationAngle) {
	origin = rotationOrigin;
	vec = rotationVec;
	angle = rotationAngle;
	axisValid = false;
}

X_INLINE void xRotation::SetOrigin(const xVec3 &rotationOrigin) {
	origin = rotationOrigin;
}

X_INLINE void xRotation::SetVec(const xVec3 &rotationVec) {
	vec = rotationVec;
	axisValid = false;
}

X_INLINE void xRotation::SetVec(float x, float y, float z) {
	vec[0] = x;
	vec[1] = y;
	vec[2] = z;
	axisValid = false;
}

X_INLINE void xRotation::SetAngle(float rotationAngle) {
	angle = rotationAngle;
	axisValid = false;
}

X_INLINE void xRotation::Scale(float s) {
	angle *= s;
	axisValid = false;
}

X_INLINE void xRotation::ReCalculateMatrix() {
	axisValid = false;
	ToMat3();
}

X_INLINE const xVec3 &xRotation::Origin() const {
	return origin;
}

X_INLINE const xVec3 &xRotation::Vec() const  {
	return vec;
}

X_INLINE float xRotation::Angle() const  {
	return angle;
}

X_INLINE xRotation xRotation::operator-() const {
	return xRotation(origin, vec, -angle);
}

X_INLINE xRotation xRotation::operator*(float s) const {
	return xRotation(origin, vec, angle * s);
}

X_INLINE xRotation xRotation::operator/(float s) const {
	assert(s != 0.0f);
	return xRotation(origin, vec, angle / s);
}

X_INLINE xRotation &xRotation::operator*=(float s) {
	angle *= s;
	axisValid = false;
	return *this;
}

X_INLINE xRotation &xRotation::operator/=(float s) {
	assert(s != 0.0f);
	angle /= s;
	axisValid = false;
	return *this;
}

X_INLINE xVec3 xRotation::operator*(const xVec3 &v) const {
	if (!axisValid) {
		ToMat3();
	}
	return ((v - origin) * axis + origin);
}

X_INLINE xRotation operator*(float s, const xRotation &r) {
	return r * s;
}

X_INLINE xVec3 operator*(const xVec3 &v, const xRotation &r) {
	return r * v;
}

X_INLINE xVec3 &operator*=(xVec3 &v, const xRotation &r) {
	v = r * v;
	return v;
}

X_INLINE void xRotation::RotatePoint(xVec3 &point) const {
	if (!axisValid) {
		ToMat3();
	}
	point = ((point - origin) * axis + origin);
}

#endif /* !__X_ROTATION_H__ */
