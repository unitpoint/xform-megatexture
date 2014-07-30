#ifndef __X_SPHERE_H__
#define __X_SPHERE_H__

#pragma once

class xSphere
{
public:
	xSphere();
	explicit xSphere(const xVec3 &point);
	explicit xSphere(const xVec3 &point, const float r);

	float			operator[](const int index) const;
	float &			operator[](const int index);
	xSphere		operator+(const xVec3 &t) const;				// returns tranlated sphere
	xSphere &		operator+=(const xVec3 &t);					// translate the sphere
	xSphere		operator+(const xSphere &s) const;
	xSphere &		operator+=(const xSphere &s);

	bool			Compare(const xSphere &a) const;							// exact compare, no epsilon
	bool			Compare(const xSphere &a, const float epsilon) const;	// compare with epsilon
	bool			operator==(	const xSphere &a) const;						// exact compare, no epsilon
	bool			operator!=(	const xSphere &a) const;						// exact compare, no epsilon

	void			Clear();									// inside out sphere
	void			Zero();									// single point at origin
	void			SetOrigin(const xVec3 &o);					// set origin of sphere
	void			SetRadius(const float r);						// set square radius

	const xVec3 &	Origin() const;						// returns origin of sphere
	float			Radius() const;						// returns sphere radius
	bool			IsCleared() const;						// returns true if sphere is inside out

	bool			Add(const xVec3 &p);					// add the point, returns true if the sphere expanded
	bool			Add(const xSphere &s);					// add the sphere, returns true if the sphere expanded
	xSphere		Expand(const float d) const;					// return bounds expanded in all directions with the given value
	xSphere &		ExpandSelf(const float d);					// expand bounds in all directions with the given value
	xSphere		Translate(const xVec3 &translation) const;
	xSphere &		TranslateSelf(const xVec3 &translation);

	float			PlaneDistance(const xPlane &plane) const;
	int				PlaneSide(const xPlane &plane, const float epsilon = ON_EPSILON) const;

	bool			ContainsPoint(const xVec3 &p) const;			// includes touching
	bool			IntersectsSphere(const xSphere &s) const;	// includes touching
	bool			LineIntersection(const xVec3 &start, const xVec3 &end) const;
					// intersection points are (start + dir * scale1) and (start + dir * scale2)
	bool			RayIntersection(const xVec3 &start, const xVec3 &dir, float &scale1, float &scale2) const;

					// Tight sphere for a point set.
	void			FromPoints(const xVec3 *points, const int numPoints);
					// Most tight sphere for a translation.
	void			FromPointTranslation(const xVec3 &point, const xVec3 &translation);
	void			FromSphereTranslation(const xSphere &sphere, const xVec3 &start, const xVec3 &translation);
					// Most tight sphere for a rotation.
	void			FromPointRotation(const xVec3 &point, const xRotation &rotation);
	void			FromSphereRotation(const xSphere &sphere, const xVec3 &start, const xRotation &rotation);

	void			AxisProjection(const xVec3 &dir, float &min, float &max) const;

private:
	xVec3			origin;
	float			radius;
};

extern const xSphere sphere_zero;

X_INLINE xSphere::xSphere() {
}

X_INLINE xSphere::xSphere(const xVec3 &point) {
	origin = point;
	radius = 0.0f;
}

X_INLINE xSphere::xSphere(const xVec3 &point, const float r) {
	origin = point;
	radius = r;
}

X_INLINE float xSphere::operator[](const int index) const {
	return ((float *) &origin)[index];
}

X_INLINE float &xSphere::operator[](const int index) {
	return ((float *) &origin)[index];
}

X_INLINE xSphere xSphere::operator+(const xVec3 &t) const {
	return xSphere(origin + t, radius);
}

X_INLINE xSphere &xSphere::operator+=(const xVec3 &t) {
	origin += t;
	return *this;
}

X_INLINE bool xSphere::Compare(const xSphere &a) const {
	return (origin.Compare(a.origin) && radius == a.radius);
}

X_INLINE bool xSphere::Compare(const xSphere &a, const float epsilon) const {
	return (origin.Compare(a.origin, epsilon) && xMath::Fabs(radius - a.radius) <= epsilon);
}

X_INLINE bool xSphere::operator==(const xSphere &a) const {
	return Compare(a);
}

X_INLINE bool xSphere::operator!=(const xSphere &a) const {
	return !Compare(a);
}

X_INLINE void xSphere::Clear() {
	origin.Zero();
	radius = -1.0f;
}

X_INLINE void xSphere::Zero() {
	origin.Zero();
	radius = 0.0f;
}

X_INLINE void xSphere::SetOrigin(const xVec3 &o) {
	origin = o;
}

X_INLINE void xSphere::SetRadius(const float r) {
	radius = r;
}

X_INLINE const xVec3 &xSphere::Origin() const {
	return origin;
}

X_INLINE float xSphere::Radius() const {
	return radius;
}

X_INLINE bool xSphere::IsCleared() const {
	return (radius < 0.0f);
}

X_INLINE bool xSphere::Add(const xVec3 &p) {
	if (radius < 0.0f) {
		origin = p;
		radius = 0.0f;
		return true;
	}
	else {
		float r = (p - origin).LengthSqr();
		if (r > radius * radius) {
			r = xMath::Sqrt(r);
			origin += (p - origin) * 0.5f * (1.0f - radius / r);
			radius += 0.5f * (r - radius);
			return true;
		}
		return false;
	}
}

X_INLINE bool xSphere::Add(const xSphere &s) {
	if (radius < 0.0f) {
		origin = s.origin;
		radius = s.radius;
		return true;
	}
	else {
		float r = (s.origin - origin).LengthSqr();
		if (r > (radius + s.radius) * (radius + s.radius)) {
			r = xMath::Sqrt(r);
			origin += (s.origin - origin) * 0.5f * (1.0f - radius / (r + s.radius));
			radius += 0.5f * ((r + s.radius) - radius);
			return true;
		}
		return false;
	}
}

X_INLINE xSphere xSphere::Expand(const float d) const {
	return xSphere(origin, radius + d);
}

X_INLINE xSphere &xSphere::ExpandSelf(const float d) {
	radius += d;
	return *this;
}

X_INLINE xSphere xSphere::Translate(const xVec3 &translation) const {
	return xSphere(origin + translation, radius);
}

X_INLINE xSphere &xSphere::TranslateSelf(const xVec3 &translation) {
	origin += translation;
	return *this;
}

X_INLINE bool xSphere::ContainsPoint(const xVec3 &p) const {
	if ((p - origin).LengthSqr() > radius * radius) {
		return false;
	}
	return true;
}

X_INLINE bool xSphere::IntersectsSphere(const xSphere &s) const {
	float r = s.radius + radius;
	if ((s.origin - origin).LengthSqr() > r * r) {
		return false;
	}
	return true;
}

X_INLINE void xSphere::FromPointTranslation(const xVec3 &point, const xVec3 &translation) {
	origin = point + 0.5f * translation;
	radius = xMath::Sqrt(0.5f * translation.LengthSqr());
}

X_INLINE void xSphere::FromSphereTranslation(const xSphere &sphere, const xVec3 &start, const xVec3 &translation) {
	origin = start + sphere.origin + 0.5f * translation;
	radius = xMath::Sqrt(0.5f * translation.LengthSqr()) + sphere.radius;
}

X_INLINE void xSphere::FromPointRotation(const xVec3 &point, const xRotation &rotation) {
	xVec3 end = rotation * point;
	origin = (point + end) * 0.5f;
	radius = xMath::Sqrt(0.5f * (end - point).LengthSqr());
}

X_INLINE void xSphere::FromSphereRotation(const xSphere &sphere, const xVec3 &start, const xRotation &rotation) {
	xVec3 end = rotation * sphere.origin;
	origin = start + (sphere.origin + end) * 0.5f;
	radius = xMath::Sqrt(0.5f * (end - sphere.origin).LengthSqr()) + sphere.radius;
}

X_INLINE void xSphere::AxisProjection(const xVec3 &dir, float &min, float &max) const {
	float d;
	d = dir * origin;
	min = d - radius;
	max = d + radius;
}

#endif /* !__X_SPHERE_H__ */
