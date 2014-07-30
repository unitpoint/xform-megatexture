#ifndef __X_BOX_H__
#define __X_BOX_H__

#pragma once

class xBox
{
public:
	xBox();
	explicit xBox(const xVec3 &center, const xVec3 &extents, const xMat3 &axis);
	explicit xBox(const xVec3 &point);
	explicit xBox(const xBounds &bounds);
	explicit xBox(const xBounds &bounds, const xVec3 &origin, const xMat3 &axis);

	xBox			operator+(const xVec3 &t) const;				// returns translated box
	xBox &			operator+=(const xVec3 &t);					// translate the box
	xBox			operator*(const xMat3 &r) const;				// returns rotated box
	xBox &			operator*=(const xMat3 &r);					// rotate the box
	xBox			operator+(const xBox &a) const;
	xBox &			operator+=(const xBox &a);
	xBox			operator-(const xBox &a) const;
	xBox &			operator-=(const xBox &a);

	bool			Compare(const xBox &a) const;						// exact compare, no epsilon
	bool			Compare(const xBox &a, const float epsilon) const;	// compare with epsilon
	bool			operator==(	const xBox &a) const;						// exact compare, no epsilon
	bool			operator!=(	const xBox &a) const;						// exact compare, no epsilon

	void			Clear();									// inside out box
	void			Zero();									// single point at origin

	const xVec3 &	Center() const;						// returns center of the box
	const xVec3 &	Extents() const;						// returns extents of the box
	const xMat3 &	Axis() const;							// returns the axis of the box
	float			Volume() const;						// returns the volume of the box
	bool			IsCleared() const;						// returns true if box are inside out

	bool			Add(const xVec3 &v);					// add the point, returns true if the box expanded
	bool			Add(const xBox &a);						// add the box, returns true if the box expanded
	xBox			Expand(const float d) const;					// return box expanded in all directions with the given value
	xBox &			ExpandSelf(const float d);					// expand box in all directions with the given value
	xBox			Translate(const xVec3 &translation) const;	// return translated box
	xBox &			TranslateSelf(const xVec3 &translation);		// translate this box
	xBox			Rotate(const xMat3 &rotation) const;			// return rotated box
	xBox &			RotateSelf(const xMat3 &rotation);			// rotate this box

	float			PlaneDistance(const xPlane &plane) const;
	int				PlaneSide(const xPlane &plane, const float epsilon = ON_EPSILON) const;

	bool			ContainsPoint(const xVec3 &p) const;			// includes touching
	bool			IntersectsBox(const xBox &a) const;			// includes touching
	bool			LineIntersection(const xVec3 &start, const xVec3 &end) const;
					// intersection points are (start + dir * scale1) and (start + dir * scale2)
	bool			RayIntersection(const xVec3 &start, const xVec3 &dir, float &scale1, float &scale2) const;

					// tight box for a collection of points
	// void			FromPoints(const xVec3 *points, const int numPoints);
					// most tight box for a translation
	void			FromPointTranslation(const xVec3 &point, const xVec3 &translation);
	void			FromBoxTranslation(const xBox &box, const xVec3 &translation);
					// most tight box for a rotation
	void			FromPointRotation(const xVec3 &point, const xRotation &rotation);
	void			FromBoxRotation(const xBox &box, const xRotation &rotation);

	void			ToPoints(xVec3 points[8]) const;
  void			ToPlanes(xPlane planes[6]) const;
  void			ToPlanes(xPlaneExact planes[6]) const;
	xSphere		ToSphere() const;

					// calculates the projection of this box onto the given axis
	void			AxisProjection(const xVec3 &dir, float &min, float &max) const;
	void			AxisProjection(const xMat3 &ax, xBounds &bounds) const;

					// calculates the silhouette of the box
	int				GetProjectionSilhouetteVerts(const xVec3 &projectionOrigin, xVec3 silVerts[6]) const;
	int				GetParallelProjectionSilhouetteVerts(const xVec3 &projectionDir, xVec3 silVerts[6]) const;

private:
	xVec3			center;
	xVec3			extents;
	xMat3			axis;
};

extern xBox	box_zero;

X_INLINE xBox::xBox() {
}

X_INLINE xBox::xBox(const xVec3 &center, const xVec3 &extents, const xMat3 &axis) {
	this->center = center;
	this->extents = extents;
	this->axis = axis;
}

X_INLINE xBox::xBox(const xVec3 &point) {
	this->center = point;
	this->extents.Zero();
	this->axis.Identity();
}

X_INLINE xBox::xBox(const xBounds &bounds) {
	this->center = (bounds[0] + bounds[1]) * 0.5f;
	this->extents = bounds[1] - this->center;
	this->axis.Identity();
}

X_INLINE xBox::xBox(const xBounds &bounds, const xVec3 &origin, const xMat3 &axis) {
	this->center = (bounds[0] + bounds[1]) * 0.5f;
	this->extents = bounds[1] - this->center;
	this->center = origin + this->center * axis;
	this->axis = axis;
}

X_INLINE xBox xBox::operator+(const xVec3 &t) const {
	return xBox(center + t, extents, axis);
}

X_INLINE xBox &xBox::operator+=(const xVec3 &t) {
	center += t;
	return *this;
}

X_INLINE xBox xBox::operator*(const xMat3 &r) const {
	return xBox(center * r, extents, axis * r);
}

X_INLINE xBox &xBox::operator*=(const xMat3 &r) {
	center *= r;
	axis *= r;
	return *this;
}

X_INLINE xBox xBox::operator+(const xBox &a) const {
	xBox newBox;
	newBox = *this;
	newBox.Add(a);
	return newBox;
}

X_INLINE xBox &xBox::operator+=(const xBox &a) {
	xBox::Add(a);
	return *this;
}

X_INLINE xBox xBox::operator-(const xBox &a) const {
	return xBox(center, extents - a.extents, axis);
}

X_INLINE xBox &xBox::operator-=(const xBox &a) {
	extents -= a.extents;
	return *this;
}

X_INLINE bool xBox::Compare(const xBox &a) const {
	return (center.Compare(a.center) && extents.Compare(a.extents) && axis.Compare(a.axis));
}

X_INLINE bool xBox::Compare(const xBox &a, const float epsilon) const {
	return (center.Compare(a.center, epsilon) && extents.Compare(a.extents, epsilon) && axis.Compare(a.axis, epsilon));
}

X_INLINE bool xBox::operator==(const xBox &a) const {
	return Compare(a);
}

X_INLINE bool xBox::operator!=(const xBox &a) const {
	return !Compare(a);
}

X_INLINE void xBox::Clear() {
	center.Zero();
	extents[0] = extents[1] = extents[2] = -xMath::WORLD_INFINITY;
	axis.Identity();
}

X_INLINE void xBox::Zero() {
	center.Zero();
	extents.Zero();
	axis.Identity();
}

X_INLINE const xVec3 &xBox::Center() const {
	return center;
}

X_INLINE const xVec3 &xBox::Extents() const {
	return extents;
}

X_INLINE const xMat3 &xBox::Axis() const {
	return axis;
}

X_INLINE float xBox::Volume() const {
	return (extents * 2.0f).LengthSqr();
}

X_INLINE bool xBox::IsCleared() const {
	return extents[0] < 0.0f;
}

X_INLINE xBox xBox::Expand(const float d) const {
	return xBox(center, extents + xVec3(d, d, d), axis);
}

X_INLINE xBox &xBox::ExpandSelf(const float d) {
	extents[0] += d;
	extents[1] += d;
	extents[2] += d;
	return *this;
}

X_INLINE xBox xBox::Translate(const xVec3 &translation) const {
	return xBox(center + translation, extents, axis);
}

X_INLINE xBox &xBox::TranslateSelf(const xVec3 &translation) {
	center += translation;
	return *this;
}

X_INLINE xBox xBox::Rotate(const xMat3 &rotation) const {
	return xBox(center * rotation, extents, axis * rotation);
}

X_INLINE xBox &xBox::RotateSelf(const xMat3 &rotation) {
	center *= rotation;
	axis *= rotation;
	return *this;
}

X_INLINE bool xBox::ContainsPoint(const xVec3 &p) const {
	xVec3 lp = p - center;
	if (xMath::Fabs(lp * axis[0]) > extents[0] ||
			xMath::Fabs(lp * axis[1]) > extents[1] ||
				xMath::Fabs(lp * axis[2]) > extents[2]) {
		return false;
	}
	return true;
}

X_INLINE xSphere xBox::ToSphere() const {
	return xSphere(center, extents.Length());
}

X_INLINE void xBox::AxisProjection(const xVec3 &dir, float &min, float &max) const {
	float d1 = dir * center;
	float d2 = xMath::Fabs(extents[0] * (dir * axis[0])) +
				xMath::Fabs(extents[1] * (dir * axis[1])) +
				xMath::Fabs(extents[2] * (dir * axis[2]));
	min = d1 - d2;
	max = d1 + d2;
}

X_INLINE void xBox::AxisProjection(const xMat3 &ax, xBounds &bounds) const {
	for (int i = 0; i < 3; i++) {
		float d1 = ax[i] * center;
		float d2 = xMath::Fabs(extents[0] * (ax[i] * axis[0])) +
					xMath::Fabs(extents[1] * (ax[i] * axis[1])) +
					xMath::Fabs(extents[2] * (ax[i] * axis[2]));
		bounds[0][i] = d1 - d2;
		bounds[1][i] = d1 + d2;
	}
}

#endif /* !__X_BOX_H__ */
