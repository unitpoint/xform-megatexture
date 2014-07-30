#ifndef __X_FRUSTUM_H__
#define __X_FRUSTUM_H__

#pragma once

class xFrustum
{
public:
	xFrustum();

  void SetPerspectiveByFovxZ(float fovx, float aspect, float zNear, float zFar);
  void SetPerspectiveByWidthZ(float width, float height, float zNear, float zFar);
  void SetPerspectiveByFovxAspectWidth(float fovx, float aspect, float width, float zFar);
  void SetPerspectiveByFovxWidth(float fovx, float width, float height, float zFar);

	void			SetOrigin(const xVec3 &origin);
	void			SetAxis(const xMat3 &axis);
  void      SetAngles(const xAngles& angles);
  void      SetPosition(const xVec3 &origin, const xAngles& angles);

	// void			SetSize(float dNear, float dFar, float farLeft, float farUp);
	void			SetPyramid(float dNear, float dFar);
	void			MoveNearDistance(float dNear);
	void			MoveFarDistance(float dFar);

	const xVec3 &	Origin() const;						// returns frustum origin
	const xMat3 &	Axis() const;							// returns frustum orientation
	xVec3			Center() const;						// returns center of frustum

	const xVec3 &	ForwardDir() const;
	const xVec3 &	LeftDir() const;
	const xVec3 &	UpDir() const;

	bool			IsValid() const;							// returns true if the frustum is valid
	float			NearDistance() const;					// returns distance to near plane
	float			FarDistance() const;					// returns distance to far plane
	
	float			NearLeft() const;
	float			NearUp() const;
  float			FarLeft() const;
	float			FarUp() const;

	xFrustum		Expand(const float d) const;					// returns frustum expanded in all directions with the given value
	xFrustum &		ExpandSelf(const float d);					// expands frustum in all directions with the given value
	xFrustum		Translate(const xVec3 &translation) const;	// returns translated frustum
	xFrustum &		TranslateSelf(const xVec3 &translation);		// translates frustum
	xFrustum		Rotate(const xMat3 &rotation) const;			// returns rotated frustum
	xFrustum &		RotateSelf(const xMat3 &rotation);			// rotates frustum

	float			PlaneDistance(const xPlane &plane) const;
	int				PlaneSide(const xPlane &plane, const float epsilon = ON_EPSILON) const;

					// fast culling but might not cull everything outside the frustum
	bool			CullPoint(const xVec3 &point) const;
	bool			CullBounds(const xBounds &bounds) const;
	bool			CullBox(const xBox &box) const;
	bool			CullSphere(const xSphere &sphere) const;
	bool			CullFrustum(const xFrustum &frustum) const;
	// bool			CullWinding(const class xWinding &winding) const;

					// exact intersection tests
	bool			ContainsPoint(const xVec3 &point) const;
	bool			IntersectsBounds(const xBounds &bounds) const;
	bool			IntersectsBox(const xBox &box) const;
	bool			IntersectsSphere(const xSphere &sphere) const;
	bool			IntersectsFrustum(const xFrustum &frustum) const;
	// bool			IntersectsWinding(const xWinding &winding) const;
	bool			LineIntersection(const xVec3 &start, const xVec3 &end) const;
	bool			RayIntersection(const xVec3 &start, const xVec3 &dir, float &scale1, float &scale2) const;

					// returns true if the projection origin is far enough away from the bounding volume to create a valid frustum
	bool			FromProjection(const xBounds &bounds, const xVec3 &projectionOrigin, const float dFar);
	bool			FromProjection(const xBox &box, const xVec3 &projectionOrigin, const float dFar);
	bool			FromProjection(const xSphere &sphere, const xVec3 &projectionOrigin, const float dFar);

					// moves the far plane so it extends just beyond the bounding volume
	bool			ConstrainToBounds(const xBounds &bounds);
	bool			ConstrainToBox(const xBox &box);
	bool			ConstrainToSphere(const xSphere &sphere);
	bool			ConstrainToFrustum(const xFrustum &frustum);

	void			ToPlanes(xPlane planes[6]) const;			// planes point outwards
	void			ToPoints(xVec3 points[8]) const;				// 8 corners of the frustum

					// calculates the projection of this frustum onto the given axis
	void			AxisProjection(const xVec3 &dir, float &min, float &max) const;
	void			AxisProjection(const xMat3 &ax, xBounds &bounds) const;

					// calculates the bounds for the projection in this frustum
	bool			ProjectionBounds(const xBounds &bounds, xBounds &projectionBounds) const;
	bool			ProjectionBounds(const xBox &box, xBounds &projectionBounds) const;
	bool			ProjectionBounds(const xSphere &sphere, xBounds &projectionBounds) const;
	bool			ProjectionBounds(const xFrustum &frustum, xBounds &projectionBounds) const;
	// bool			ProjectionBounds(const xWinding &winding, xBounds &projectionBounds) const;

					// calculates the bounds for the projection in this frustum of the given frustum clipped to the given box
	bool			ClippedProjectionBounds(const xFrustum &frustum, const xBox &clipBox, xBounds &projectionBounds) const;

  xVec3     ProjectPoint(const xVec3& p);

private:
	xVec3			origin;		// frustum origin
	xMat3			axis;		// frustum orientation
	float			dNear;		// distance of near plane, dNear >= 0.0f
	float			dFar;		// distance of far plane, dFar > dNear
	float			farLeft;		// half the width at the far plane
	float			farUp;		// half the height at the far plane
	float			invFar;		// 1.0f / dFar

private:
	bool			CullLocalBox(const xVec3 &localOrigin, const xVec3 &extents, const xMat3 &localAxis) const;
	bool			CullLocalFrustum(const xFrustum &localFrustum, const xVec3 indexPoints[8], const xVec3 cornerVecs[4]) const;
	bool			CullLocalWinding(const xVec3 *points, const int numPoints, int *pointCull) const;
	bool			BoundsCullLocalFrustum(const xBounds &bounds, const xFrustum &localFrustum, const xVec3 indexPoints[8], const xVec3 cornerVecs[4]) const;
	bool			LocalLineIntersection(const xVec3 &start, const xVec3 &end) const;
	bool			LocalRayIntersection(const xVec3 &start, const xVec3 &dir, float &scale1, float &scale2) const;
	bool			LocalFrustumIntersectsFrustum(const xVec3 points[8], const bool testFirstSide) const;
	bool			LocalFrustumIntersectsBounds(const xVec3 points[8], const xBounds &bounds) const;
	void			ToClippedPoints(const float fractions[4], xVec3 points[8]) const;
	void			ToIndexPoints(xVec3 indexPoints[8]) const;
	void			ToIndexPointsAndCornerVecs(xVec3 indexPoints[8], xVec3 cornerVecs[4]) const;
	void			AxisProjection(const xVec3 indexPoints[8], const xVec3 cornerVecs[4], const xVec3 &dir, float &min, float &max) const;
	void			AddLocalLineToProjectionBoundsSetCull(const xVec3 &start, const xVec3 &end, int &startCull, int &endCull, xBounds &bounds) const;
	void			AddLocalLineToProjectionBoundsUseCull(const xVec3 &start, const xVec3 &end, int startCull, int endCull, xBounds &bounds) const;
	bool			AddLocalCapsToProjectionBounds(const xVec3 endPoints[4], const int endPointCull[4], const xVec3 &point, int pointCull, int pointClip, xBounds &projectionBounds) const;
	bool			BoundsRayIntersection(const xBounds &bounds, const xVec3 &start, const xVec3 &dir, float &scale1, float &scale2) const;
	void			ClipFrustumToBox(const xBox &box, float clipFractions[4], int clipPlanes[4]) const;
	bool			ClipLine(const xVec3 localPoints[8], const xVec3 points[8], int startIndex, int endIndex, xVec3 &start, xVec3 &end, int &startClip, int &endClip) const;
};


X_INLINE xFrustum::xFrustum() {
	dNear = dFar = 0.0f;
}

X_INLINE void xFrustum::SetOrigin(const xVec3 &origin) {
	this->origin = origin;
}

X_INLINE void xFrustum::SetAxis(const xMat3 &axis) {
	this->axis = axis;
}

/*
X_INLINE void xFrustum::SetSize(float dNear, float dFar, float dLeft, float farUp) {
	assert(dNear >= 0.0f && dFar > dNear && farLeft > 0.0f && farUp > 0.0f);
	this->dNear = dNear;
	this->dFar = dFar;
	this->farLeft = farLeft;
	this->farUp = farUp;
	this->invFar = 1.0f / dFar;
}
*/

X_INLINE void xFrustum::SetPyramid(float dNear, float dFar) {
	assert(dNear >= 0.0f && dFar > dNear);
	this->dNear = dNear;
	this->dFar = dFar;
	this->farLeft = dFar;
	this->farUp = dFar;
	this->invFar = 1.0f / dFar;
}

X_INLINE void xFrustum::MoveNearDistance(float dNear) {
	assert(dNear >= 0.0f);
	this->dNear = dNear;
}

X_INLINE void xFrustum::MoveFarDistance(float dFar) {
	assert(dFar > this->dNear);
	float scale = dFar / this->dFar;
	this->dFar = dFar;
	this->farLeft *= scale;
	this->farUp *= scale;
	this->invFar = 1.0f / dFar;
}

X_INLINE const xVec3 &xFrustum::Origin() const {
	return origin;
}

X_INLINE const xMat3 &xFrustum::Axis() const {
	return axis;
}

X_INLINE const xVec3& xFrustum::ForwardDir() const
{
  return axis[0];
}
X_INLINE const xVec3& xFrustum::LeftDir() const
{
  return axis[1];
}
X_INLINE const xVec3& xFrustum::UpDir() const
{
  return axis[2];
}

X_INLINE xVec3 xFrustum::Center() const {
	return (origin + axis[0] * ((dFar - dNear) * 0.5f));
}

X_INLINE bool xFrustum::IsValid() const {
	return (dFar > dNear);
}

X_INLINE float xFrustum::NearDistance() const {
	return dNear;
}

X_INLINE float xFrustum::FarDistance() const {
	return dFar;
}

X_INLINE float xFrustum::NearLeft() const {
	return farLeft * dNear * invFar;
}

X_INLINE float xFrustum::NearUp() const {
	return farUp * dNear * invFar;
}

X_INLINE float xFrustum::FarLeft() const {
	return farLeft;
}

X_INLINE float xFrustum::FarUp() const {
	return farUp;
}

X_INLINE xFrustum xFrustum::Expand(const float d) const {
	xFrustum f = *this;
	f.origin -= d * f.axis[0];
	f.dFar += 2.0f * d;
	f.farLeft = f.dFar * farLeft * invFar;
	f.farUp = f.dFar * farUp * invFar;
	f.invFar = 1.0f / dFar;
	return f;
}

X_INLINE xFrustum &xFrustum::ExpandSelf(const float d) {
	origin -= d * axis[0];
	dFar += 2.0f * d;
	farLeft = dFar * farLeft * invFar;
	farUp = dFar * farUp * invFar;
	invFar = 1.0f / dFar;
	return *this;
}

X_INLINE xFrustum xFrustum::Translate(const xVec3 &translation) const {
	xFrustum f = *this;
	f.origin += translation;
	return f;
}

X_INLINE xFrustum &xFrustum::TranslateSelf(const xVec3 &translation) {
	origin += translation;
	return *this;
}

X_INLINE xFrustum xFrustum::Rotate(const xMat3 &rotation) const {
	xFrustum f = *this;
	f.axis *= rotation;
	return f;
}

X_INLINE xFrustum &xFrustum::RotateSelf(const xMat3 &rotation) {
	axis *= rotation;
	return *this;
}

#endif /* !__X_FRUSTUM_H__ */
