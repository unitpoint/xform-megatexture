#ifndef __X_BOUNDS_H__
#define __X_BOUNDS_H__

#pragma once

class xBounds
{
public:
	xBounds();
	explicit xBounds(const xVec3 &mins, const xVec3 &maxs);
	explicit xBounds(const xVec3 &point);

	const xVec3 &	operator[](const int index) const;
	xVec3 &		operator[](const int index);
	xBounds		operator+(const xVec3 &t) const;				// returns translated bounds
	xBounds &		operator+=(const xVec3 &t);					// translate the bounds
	xBounds		operator*(const xMat3 &r) const;				// returns rotated bounds
	xBounds &		operator*=(const xMat3 &r);					// rotate the bounds
	xBounds		operator+(const xBounds &a) const;
	xBounds &		operator+=(const xBounds &a);
	xBounds		operator-(const xBounds &a) const;
	xBounds &		operator-=(const xBounds &a);

	bool			Compare(const xBounds &a) const;							// exact compare, no epsilon
	bool			Compare(const xBounds &a, const float epsilon) const;	// compare with epsilon
	bool			operator==(	const xBounds &a) const;						// exact compare, no epsilon
	bool			operator!=(	const xBounds &a) const;						// exact compare, no epsilon

	void			Clear();									// inside out bounds
	void			Zero();									// single point at origin

	xVec3			Center() const;						// returns center of bounds
	float			Radius() const;						// returns the radius relative to the bounds origin
	float			Radius(const xVec3 &center) const;		// returns the radius relative to the given center
	float			Volume() const;						// returns the volume of the bounds
	bool			IsCleared() const;						// returns true if bounds are inside out

	bool			Add(const xVec3 &v);					// add the point, returns true if the bounds expanded
	bool			Add(const xBounds &a);					// add the bounds, returns true if the bounds expanded
	xBounds		Intersect(const xBounds &a) const;			// return intersection of this bounds with the given bounds
	xBounds &		IntersectSelf(const xBounds &a);				// intersect this bounds with the given bounds
	xBounds		Expand(const float d) const;					// return bounds expanded in all directions with the given value
	xBounds &		ExpandSelf(const float d);					// expand bounds in all directions with the given value
	xBounds		Translate(const xVec3 &translation) const;	// return translated bounds
	xBounds &		TranslateSelf(const xVec3 &translation);		// translate this bounds
	xBounds		Rotate(const xMat3 &rotation) const;			// return rotated bounds
	xBounds &		RotateSelf(const xMat3 &rotation);			// rotate this bounds

	float			PlaneDistance(const xPlane &plane) const;
	int				PlaneSide(const xPlane &plane, const float epsilon = ON_EPSILON) const;
	int				PlaneSide(const xPlaneExact &plane, const float epsilon = ON_EPSILON) const;

	bool			ContainsPoint(const xVec3 &p) const;			// includes touching
	bool			IntersectsBounds(const xBounds &a) const;	// includes touching
	bool			LineIntersection(const xVec3 &start, const xVec3 &end) const;
					// intersection point is start + dir * scale
	bool			RayIntersection(const xVec3 &start, const xVec3 &dir, float &scale) const;

					// most tight bounds for the given transformed bounds
	void			FromTransformedBounds(const xBounds &bounds, const xVec3 &origin, const xMat3 &axis);
					// most tight bounds for a point set
	void			FromPoints(const xVec3 *points, const int numPoints);
					// most tight bounds for a translation
	void			FromPointTranslation(const xVec3 &point, const xVec3 &translation);
	void			FromBoundsTranslation(const xBounds &bounds, const xVec3 &origin, const xMat3 &axis, const xVec3 &translation);
					// most tight bounds for a rotation
	void			FromPointRotation(const xVec3 &point, const xRotation &rotation);
	void			FromBoundsRotation(const xBounds &bounds, const xVec3 &origin, const xMat3 &axis, const xRotation &rotation);

	void			ToPoints(xVec3 points[8]) const;
  void			ToPlanes(xPlane planes[6]) const;
  void			ToPlanes(xPlaneExact planes[6]) const;
	xSphere		ToSphere() const;

	void			AxisProjection(const xVec3 &dir, float &min, float &max) const;
	void			AxisProjection(const xVec3 &origin, const xMat3 &axis, const xVec3 &dir, float &min, float &max) const;

private:
	xVec3			b[2];
};

extern xBounds	bounds_zero;

X_INLINE xBounds::xBounds() {
}

X_INLINE xBounds::xBounds(const xVec3 &mins, const xVec3 &maxs) {
	b[0] = mins;
	b[1] = maxs;
}

X_INLINE xBounds::xBounds(const xVec3 &point) {
	b[0] = point;
	b[1] = point;
}

X_INLINE const xVec3 &xBounds::operator[](const int index) const {
	return b[index];
}

X_INLINE xVec3 &xBounds::operator[](const int index) {
	return b[index];
}

X_INLINE xBounds xBounds::operator+(const xVec3 &t) const {
	return xBounds(b[0] + t, b[1] + t);
}

X_INLINE xBounds &xBounds::operator+=(const xVec3 &t) {
	b[0] += t;
	b[1] += t;
	return *this;
}

X_INLINE xBounds xBounds::operator*(const xMat3 &r) const {
	xBounds bounds;
	bounds.FromTransformedBounds(*this, vec3_origin, r);
	return bounds;
}

X_INLINE xBounds &xBounds::operator*=(const xMat3 &r) {
	this->FromTransformedBounds(*this, vec3_origin, r);
	return *this;
}

X_INLINE xBounds xBounds::operator+(const xBounds &a) const {
	xBounds newBounds;
	newBounds = *this;
	newBounds.Add(a);
	return newBounds;
}

X_INLINE xBounds &xBounds::operator+=(const xBounds &a) {
	xBounds::Add(a);
	return *this;
}

X_INLINE xBounds xBounds::operator-(const xBounds &a) const {
	assert(b[1][0] - b[0][0] > a.b[1][0] - a.b[0][0] &&
				b[1][1] - b[0][1] > a.b[1][1] - a.b[0][1] &&
					b[1][2] - b[0][2] > a.b[1][2] - a.b[0][2]);
	return xBounds(xVec3(b[0][0] + a.b[1][0], b[0][1] + a.b[1][1], b[0][2] + a.b[1][2]),
					xVec3(b[1][0] + a.b[0][0], b[1][1] + a.b[0][1], b[1][2] + a.b[0][2]));
}

X_INLINE xBounds &xBounds::operator-=(const xBounds &a) {
	assert(b[1][0] - b[0][0] > a.b[1][0] - a.b[0][0] &&
				b[1][1] - b[0][1] > a.b[1][1] - a.b[0][1] &&
					b[1][2] - b[0][2] > a.b[1][2] - a.b[0][2]);
	b[0] += a.b[1];
	b[1] += a.b[0];
	return *this;
}

X_INLINE bool xBounds::Compare(const xBounds &a) const {
	return (b[0].Compare(a.b[0]) && b[1].Compare(a.b[1]));
}

X_INLINE bool xBounds::Compare(const xBounds &a, const float epsilon) const {
	return (b[0].Compare(a.b[0], epsilon) && b[1].Compare(a.b[1], epsilon));
}

X_INLINE bool xBounds::operator==(const xBounds &a) const {
	return Compare(a);
}

X_INLINE bool xBounds::operator!=(const xBounds &a) const {
	return !Compare(a);
}

X_INLINE void xBounds::Clear() {
	b[0][0] = b[0][1] = b[0][2] = xMath::WORLD_INFINITY;
	b[1][0] = b[1][1] = b[1][2] = -xMath::WORLD_INFINITY;
}

X_INLINE void xBounds::Zero() {
	b[0][0] = b[0][1] = b[0][2] =
	b[1][0] = b[1][1] = b[1][2] = 0;
}

X_INLINE xVec3 xBounds::Center() const {
	return xVec3((b[1][0] + b[0][0]) * 0.5f, (b[1][1] + b[0][1]) * 0.5f, (b[1][2] + b[0][2]) * 0.5f);
}

X_INLINE float xBounds::Volume() const {
	if (b[0][0] >= b[1][0] || b[0][1] >= b[1][1] || b[0][2] >= b[1][2]) {
		return 0.0f;
	}
	return ((b[1][0] - b[0][0]) * (b[1][1] - b[0][1]) * (b[1][2] - b[0][2]));
}

X_INLINE bool xBounds::IsCleared() const {
	return b[0][0] > b[1][0];
}

X_INLINE bool xBounds::Add(const xVec3 &v) {
	bool expanded = false;
	if (v[0] < b[0][0]) {
		b[0][0] = v[0];
		expanded = true;
	}
	if (v[0] > b[1][0]) {
		b[1][0] = v[0];
		expanded = true;
	}
	if (v[1] < b[0][1]) {
		b[0][1] = v[1];
		expanded = true;
	}
	if (v[1] > b[1][1]) {
		b[1][1] = v[1];
		expanded = true;
	}
	if (v[2] < b[0][2]) {
		b[0][2] = v[2];
		expanded = true;
	}
	if (v[2] > b[1][2]) {
		b[1][2] = v[2];
		expanded = true;
	}
	return expanded;
}

X_INLINE bool xBounds::Add(const xBounds &a) {
	bool expanded = false;
	if (a.b[0][0] < b[0][0]) {
		b[0][0] = a.b[0][0];
		expanded = true;
	}
	if (a.b[0][1] < b[0][1]) {
		b[0][1] = a.b[0][1];
		expanded = true;
	}
	if (a.b[0][2] < b[0][2]) {
		b[0][2] = a.b[0][2];
		expanded = true;
	}
	if (a.b[1][0] > b[1][0]) {
		b[1][0] = a.b[1][0];
		expanded = true;
	}
	if (a.b[1][1] > b[1][1]) {
		b[1][1] = a.b[1][1];
		expanded = true;
	}
	if (a.b[1][2] > b[1][2]) {
		b[1][2] = a.b[1][2];
		expanded = true;
	}
	return expanded;
}

X_INLINE xBounds xBounds::Intersect(const xBounds &a) const {
	xBounds n;
	n.b[0][0] = (a.b[0][0] > b[0][0]) ? a.b[0][0] : b[0][0];
	n.b[0][1] = (a.b[0][1] > b[0][1]) ? a.b[0][1] : b[0][1];
	n.b[0][2] = (a.b[0][2] > b[0][2]) ? a.b[0][2] : b[0][2];
	n.b[1][0] = (a.b[1][0] < b[1][0]) ? a.b[1][0] : b[1][0];
	n.b[1][1] = (a.b[1][1] < b[1][1]) ? a.b[1][1] : b[1][1];
	n.b[1][2] = (a.b[1][2] < b[1][2]) ? a.b[1][2] : b[1][2];
	return n;
}

X_INLINE xBounds &xBounds::IntersectSelf(const xBounds &a) {
	if (a.b[0][0] > b[0][0]) {
		b[0][0] = a.b[0][0];
	}
	if (a.b[0][1] > b[0][1]) {
		b[0][1] = a.b[0][1];
	}
	if (a.b[0][2] > b[0][2]) {
		b[0][2] = a.b[0][2];
	}
	if (a.b[1][0] < b[1][0]) {
		b[1][0] = a.b[1][0];
	}
	if (a.b[1][1] < b[1][1]) {
		b[1][1] = a.b[1][1];
	}
	if (a.b[1][2] < b[1][2]) {
		b[1][2] = a.b[1][2];
	}
	return *this;
}

X_INLINE xBounds xBounds::Expand(const float d) const {
	return xBounds(xVec3(b[0][0] - d, b[0][1] - d, b[0][2] - d),
						xVec3(b[1][0] + d, b[1][1] + d, b[1][2] + d));
}

X_INLINE xBounds &xBounds::ExpandSelf(const float d) {
	b[0][0] -= d;
	b[0][1] -= d;
	b[0][2] -= d;
	b[1][0] += d;
	b[1][1] += d;
	b[1][2] += d;
	return *this;
}

X_INLINE xBounds xBounds::Translate(const xVec3 &translation) const {
	return xBounds(b[0] + translation, b[1] + translation);
}

X_INLINE xBounds &xBounds::TranslateSelf(const xVec3 &translation) {
	b[0] += translation;
	b[1] += translation;
	return *this;
}

X_INLINE xBounds xBounds::Rotate(const xMat3 &rotation) const {
	xBounds bounds;
	bounds.FromTransformedBounds(*this, vec3_origin, rotation);
	return bounds;
}

X_INLINE xBounds &xBounds::RotateSelf(const xMat3 &rotation) {
	FromTransformedBounds(*this, vec3_origin, rotation);
	return *this;
}

X_INLINE bool xBounds::ContainsPoint(const xVec3 &p) const {
	if (p[0] < b[0][0] || p[1] < b[0][1] || p[2] < b[0][2]
		|| p[0] > b[1][0] || p[1] > b[1][1] || p[2] > b[1][2]) {
		return false;
	}
	return true;
}

X_INLINE bool xBounds::IntersectsBounds(const xBounds &a) const {
	if (a.b[1][0] < b[0][0] || a.b[1][1] < b[0][1] || a.b[1][2] < b[0][2]
		|| a.b[0][0] > b[1][0] || a.b[0][1] > b[1][1] || a.b[0][2] > b[1][2]) {
		return false;
	}
	return true;
}

X_INLINE xSphere xBounds::ToSphere() const {
	xSphere sphere;
	sphere.SetOrigin((b[0] + b[1]) * 0.5f);
	sphere.SetRadius((b[1] - sphere.Origin()).Length());
	return sphere;
}

X_INLINE void xBounds::AxisProjection(const xVec3 &dir, float &min, float &max) const {
	float d1, d2;
	xVec3 center, extents;

	center = (b[0] + b[1]) * 0.5f;
	extents = b[1] - center;

	d1 = dir * center;
	d2 = xMath::Fabs(extents[0] * dir[0]) +
			xMath::Fabs(extents[1] * dir[1]) +
				xMath::Fabs(extents[2] * dir[2]);

	min = d1 - d2;
	max = d1 + d2;
}

X_INLINE void xBounds::AxisProjection(const xVec3 &origin, const xMat3 &axis, const xVec3 &dir, float &min, float &max) const {
	float d1, d2;
	xVec3 center, extents;

	center = (b[0] + b[1]) * 0.5f;
	extents = b[1] - center;
	center = origin + center * axis;

	d1 = dir * center;
	d2 = xMath::Fabs(extents[0] * (dir * axis[0])) +
			xMath::Fabs(extents[1] * (dir * axis[1])) +
				xMath::Fabs(extents[2] * (dir * axis[2]));

	min = d1 - d2;
	max = d1 + d2;
}

// ========================================================================
// ========================================================================
// ========================================================================

class xBounds2
{
public:
  xBounds2();
  explicit xBounds2(const xVec2 &mins, const xVec2 &maxs);
  explicit xBounds2(const xVec2 &point);

  const xVec2 &	operator[](const int index) const;
  xVec2 &		operator[](const int index);
  xBounds2		operator+(const xVec2 &t) const;				// returns translated bounds
  xBounds2 &		operator+=(const xVec2 &t);					// translate the bounds
  xBounds2		operator+(const xBounds2 &a) const;
  xBounds2 &		operator+=(const xBounds2 &a);
  xBounds2		operator-(const xBounds2 &a) const;
  xBounds2 &		operator-=(const xBounds2 &a);

  bool			Compare(const xBounds2 &a) const;							// exact compare, no epsilon
  bool			Compare(const xBounds2 &a, const float epsilon) const;	// compare with epsilon
  bool			operator==(	const xBounds2 &a) const;						// exact compare, no epsilon
  bool			operator!=(	const xBounds2 &a) const;						// exact compare, no epsilon

  void			Clear();									// inside out bounds
  void			Zero();									// single point at origin

  xVec2			Center() const;						// returns center of bounds
  float			Radius() const;						// returns the radius relative to the bounds origin
  float			Radius(const xVec2 &center) const;		// returns the radius relative to the given center
  bool			IsCleared() const;						// returns true if bounds are inside out

  bool			Add(const xVec2 &v);					// add the point, returns true if the bounds expanded
  bool			Add(const xBounds2 &a);					// add the bounds, returns true if the bounds expanded
  xBounds2		Intersect(const xBounds2 &a) const;			// return intersection of this bounds with the given bounds
  xBounds2 &		IntersectSelf(const xBounds2 &a);				// intersect this bounds with the given bounds
  xBounds2		Expand(const float d) const;					// return bounds expanded in all directions with the given value
  xBounds2 &		ExpandSelf(const float d);					// expand bounds in all directions with the given value
  xBounds2		Translate(const xVec2 &translation) const;	// return translated bounds
  xBounds2 &		TranslateSelf(const xVec2 &translation);		// translate this bounds

  bool			ContainsPoint(const xVec2 &p) const;			// includes touching
  bool			IntersectsBounds(const xBounds2 &a) const;	// includes touching

private:
  xVec2			b[2];
};

extern xBounds2	bounds2_zero;

X_INLINE xBounds2::xBounds2() {
}

X_INLINE xBounds2::xBounds2(const xVec2 &mins, const xVec2 &maxs) {
  b[0] = mins;
  b[1] = maxs;
}

X_INLINE xBounds2::xBounds2(const xVec2 &point) {
  b[0] = point;
  b[1] = point;
}

X_INLINE const xVec2 &xBounds2::operator[](const int index) const {
  return b[index];
}

X_INLINE xVec2 &xBounds2::operator[](const int index) {
  return b[index];
}

X_INLINE xBounds2 xBounds2::operator+(const xVec2 &t) const {
  return xBounds2(b[0] + t, b[1] + t);
}

X_INLINE xBounds2 &xBounds2::operator+=(const xVec2 &t) {
  b[0] += t;
  b[1] += t;
  return *this;
}

X_INLINE xBounds2 xBounds2::operator+(const xBounds2 &a) const {
  xBounds2 newBounds;
  newBounds = *this;
  newBounds.Add(a);
  return newBounds;
}

X_INLINE xBounds2 &xBounds2::operator+=(const xBounds2 &a) {
  xBounds2::Add(a);
  return *this;
}

X_INLINE xBounds2 xBounds2::operator-(const xBounds2 &a) const {
  assert(b[1][0] - b[0][0] > a.b[1][0] - a.b[0][0] &&
    b[1][1] - b[0][1] > a.b[1][1] - a.b[0][1]);
  return xBounds2(xVec2(b[0][0] + a.b[1][0], b[0][1] + a.b[1][1]),
    xVec2(b[1][0] + a.b[0][0], b[1][1] + a.b[0][1]));
}

X_INLINE xBounds2 &xBounds2::operator-=(const xBounds2 &a) {
  assert(b[1][0] - b[0][0] > a.b[1][0] - a.b[0][0] &&
    b[1][1] - b[0][1] > a.b[1][1] - a.b[0][1]);
  b[0] += a.b[1];
  b[1] += a.b[0];
  return *this;
}

X_INLINE bool xBounds2::Compare(const xBounds2 &a) const {
  return (b[0].Compare(a.b[0]) && b[1].Compare(a.b[1]));
}

X_INLINE bool xBounds2::Compare(const xBounds2 &a, const float epsilon) const {
  return (b[0].Compare(a.b[0], epsilon) && b[1].Compare(a.b[1], epsilon));
}

X_INLINE bool xBounds2::operator==(const xBounds2 &a) const {
  return Compare(a);
}

X_INLINE bool xBounds2::operator!=(const xBounds2 &a) const {
  return !Compare(a);
}

X_INLINE void xBounds2::Clear() {
  b[0][0] = b[0][1] = xMath::WORLD_INFINITY;
  b[1][0] = b[1][1] = -xMath::WORLD_INFINITY;
}

X_INLINE void xBounds2::Zero() {
  b[0][0] = b[0][1] = 
    b[1][0] = b[1][1] = 0;
}

X_INLINE xVec2 xBounds2::Center() const {
  return xVec2((b[1][0] + b[0][0]) * 0.5f, (b[1][1] + b[0][1]) * 0.5f);
}

X_INLINE bool xBounds2::IsCleared() const {
  return b[0][0] > b[1][0];
}

X_INLINE bool xBounds2::Add(const xVec2 &v) {
  bool expanded = false;
  if (v[0] < b[0][0]) {
    b[0][0] = v[0];
    expanded = true;
  }
  if (v[0] > b[1][0]) {
    b[1][0] = v[0];
    expanded = true;
  }
  if (v[1] < b[0][1]) {
    b[0][1] = v[1];
    expanded = true;
  }
  if (v[1] > b[1][1]) {
    b[1][1] = v[1];
    expanded = true;
  }
  return expanded;
}

X_INLINE bool xBounds2::Add(const xBounds2 &a) {
  bool expanded = false;
  if (a.b[0][0] < b[0][0]) {
    b[0][0] = a.b[0][0];
    expanded = true;
  }
  if (a.b[0][1] < b[0][1]) {
    b[0][1] = a.b[0][1];
    expanded = true;
  }
  if (a.b[1][0] > b[1][0]) {
    b[1][0] = a.b[1][0];
    expanded = true;
  }
  if (a.b[1][1] > b[1][1]) {
    b[1][1] = a.b[1][1];
    expanded = true;
  }
  return expanded;
}

X_INLINE xBounds2 xBounds2::Intersect(const xBounds2 &a) const {
  xBounds2 n;
  n.b[0][0] = (a.b[0][0] > b[0][0]) ? a.b[0][0] : b[0][0];
  n.b[0][1] = (a.b[0][1] > b[0][1]) ? a.b[0][1] : b[0][1];
  n.b[1][0] = (a.b[1][0] < b[1][0]) ? a.b[1][0] : b[1][0];
  n.b[1][1] = (a.b[1][1] < b[1][1]) ? a.b[1][1] : b[1][1];
  return n;
}

X_INLINE xBounds2 &xBounds2::IntersectSelf(const xBounds2 &a) {
  if (a.b[0][0] > b[0][0]) {
    b[0][0] = a.b[0][0];
  }
  if (a.b[0][1] > b[0][1]) {
    b[0][1] = a.b[0][1];
  }
  if (a.b[1][0] < b[1][0]) {
    b[1][0] = a.b[1][0];
  }
  if (a.b[1][1] < b[1][1]) {
    b[1][1] = a.b[1][1];
  }
  return *this;
}

X_INLINE xBounds2 xBounds2::Expand(const float d) const {
  return xBounds2(xVec2(b[0][0] - d, b[0][1] - d),
    xVec2(b[1][0] + d, b[1][1] + d));
}

X_INLINE xBounds2 &xBounds2::ExpandSelf(const float d) {
  b[0][0] -= d;
  b[0][1] -= d;
  b[1][0] += d;
  b[1][1] += d;
  return *this;
}

X_INLINE xBounds2 xBounds2::Translate(const xVec2 &translation) const {
  return xBounds2(b[0] + translation, b[1] + translation);
}

X_INLINE xBounds2 &xBounds2::TranslateSelf(const xVec2 &translation) {
  b[0] += translation;
  b[1] += translation;
  return *this;
}

X_INLINE bool xBounds2::ContainsPoint(const xVec2 &p) const {
  if (p[0] < b[0][0] || p[1] < b[0][1]
      || p[0] > b[1][0] || p[1] > b[1][1]) {
    return false;
  }
  return true;
}

X_INLINE bool xBounds2::IntersectsBounds(const xBounds2 &a) const {
  if (a.b[1][0] < b[0][0] || a.b[1][1] < b[0][1]
      || a.b[0][0] > b[1][0] || a.b[0][1] > b[1][1]) {
    return false;
  }
  return true;
}

#endif /* !__X_BOUNDS_H__ */
