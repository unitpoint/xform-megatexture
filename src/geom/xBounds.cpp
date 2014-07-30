#include <xForm.h>

xBounds bounds_zero(vec3_zero, vec3_zero);
xBounds2 bounds2_zero(vec2_zero, vec2_zero);

/*
============
xBounds::Radius
============
*/
float xBounds::Radius() const {
	int		i;
	float	total, b0, b1;

	total = 0.0f;
	for (i = 0; i < 3; i++) {
		b0 = (float)xMath::Fabs(b[0][i]);
		b1 = (float)xMath::Fabs(b[1][i]);
		if (b0 > b1) {
			total += b0 * b0;
		} else {
			total += b1 * b1;
		}
	}
	return xMath::Sqrt(total);
}

/*
============
xBounds::Radius
============
*/
float xBounds::Radius(const xVec3 &center) const {
	int		i;
	float	total, b0, b1;

	total = 0.0f;
	for (i = 0; i < 3; i++) {
		b0 = (float)xMath::Fabs(center[i] - b[0][i]);
		b1 = (float)xMath::Fabs(b[1][i] - center[i]);
		if (b0 > b1) {
			total += b0 * b0;
		} else {
			total += b1 * b1;
		}
	}
	return xMath::Sqrt(total);
}

/*
================
xBounds::PlaneDistance
================
*/
float xBounds::PlaneDistance(const xPlane &plane) const {
	xVec3 center;
	float d1, d2;

	center = (b[0] + b[1]) * 0.5f;

	d1 = plane.Distance(center);
	d2 = xMath::Fabs((b[1][0] - center[0]) * plane.Normal()[0]) +
			xMath::Fabs((b[1][1] - center[1]) * plane.Normal()[1]) +
				xMath::Fabs((b[1][2] - center[2]) * plane.Normal()[2]);

	if (d1 - d2 > 0.0f) {
		return d1 - d2;
	}
	if (d1 + d2 < 0.0f) {
		return d1 + d2;
	}
	return 0.0f;
}

/*
================
xBounds::PlaneSide
================
*/
int xBounds::PlaneSide(const xPlane &plane, const float epsilon) const {
	xVec3 center;
	float d1, d2;

	center = (b[0] + b[1]) * 0.5f;

	d1 = plane.Distance(center);
	d2 = xMath::Fabs((b[1][0] - center[0]) * plane.Normal()[0]) +
			xMath::Fabs((b[1][1] - center[1]) * plane.Normal()[1]) +
				xMath::Fabs((b[1][2] - center[2]) * plane.Normal()[2]);

	if (d1 - d2 > epsilon) {
		return SIDE_FRONT;
	}
	if (d1 + d2 < -epsilon) {
		return SIDE_BACK;
	}
	return SIDE_CROSS;
}

/*
================
xBounds::PlaneSide xPlaneExact
================
*/
int xBounds::PlaneSide(const xPlaneExact &plane, const float epsilon) const {
	xVec3 center;
	float d1, d2;

	center = (b[0] + b[1]) * 0.5f;

	d1 = plane.Distance(center);
	d2 = fabs((b[1][0] - center[0]) * plane.Normal()[0]) +
			fabs((b[1][1] - center[1]) * plane.Normal()[1]) +
				fabs((b[1][2] - center[2]) * plane.Normal()[2]);

	if (d1 - d2 > epsilon) {
		return SIDE_FRONT;
	}
	if (d1 + d2 < -epsilon) {
		return SIDE_BACK;
	}
	return SIDE_CROSS;
}

/*
============
xBounds::LineIntersection

  Returns true if the line intersects the bounds between the start and end point.
============
*/
bool xBounds::LineIntersection(const xVec3 &start, const xVec3 &end) const {
    float ld[3];
	xVec3 center = (b[0] + b[1]) * 0.5f;
	xVec3 extents = b[1] - center;
    xVec3 lineDir = 0.5f * (end - start);
    xVec3 lineCenter = start + lineDir;
    xVec3 dir = lineCenter - center;

    ld[0] = xMath::Fabs(lineDir[0]);
	if (xMath::Fabs(dir[0]) > extents[0] + ld[0]) {
        return false;
	}

    ld[1] = xMath::Fabs(lineDir[1]);
	if (xMath::Fabs(dir[1]) > extents[1] + ld[1]) {
        return false;
	}

    ld[2] = xMath::Fabs(lineDir[2]);
	if (xMath::Fabs(dir[2]) > extents[2] + ld[2]) {
        return false;
	}

    xVec3 cross = lineDir.Cross(dir);

	if (xMath::Fabs(cross[0]) > extents[1] * ld[2] + extents[2] * ld[1]) {
        return false;
	}

	if (xMath::Fabs(cross[1]) > extents[0] * ld[2] + extents[2] * ld[0]) {
        return false;
	}

	if (xMath::Fabs(cross[2]) > extents[0] * ld[1] + extents[1] * ld[0]) {
        return false;
	}

    return true;
}

/*
============
xBounds::RayIntersection

  Returns true if the ray intersects the bounds.
  The ray can intersect the bounds in both directions from the start point.
  If start is inside the bounds it is considered an intersection with scale = 0
============
*/
bool xBounds::RayIntersection(const xVec3 &start, const xVec3 &dir, float &scale) const {
	int i, ax0, ax1, ax2, side, inside;
	float f;
	xVec3 hit;

	ax0 = -1;
	inside = 0;
	for (i = 0; i < 3; i++) {
		if (start[i] < b[0][i]) {
			side = 0;
		}
		else if (start[i] > b[1][i]) {
			side = 1;
		}
		else {
			inside++;
			continue;
		}
		if (dir[i] == 0.0f) {
			continue;
		}
		f = (start[i] - b[side][i]);
		if (ax0 < 0 || xMath::Fabs(f) > xMath::Fabs(scale * dir[i])) {
			scale = - (f / dir[i]);
			ax0 = i;
		}
	}

	if (ax0 < 0) {
		scale = 0.0f;
		// return true if the start point is inside the bounds
		return (inside == 3);
	}

	ax1 = (ax0+1)%3;
	ax2 = (ax0+2)%3;
	hit[ax1] = start[ax1] + scale * dir[ax1];
	hit[ax2] = start[ax2] + scale * dir[ax2];

	return (hit[ax1] >= b[0][ax1] && hit[ax1] <= b[1][ax1] &&
				hit[ax2] >= b[0][ax2] && hit[ax2] <= b[1][ax2]);
}

/*
============
xBounds::FromTransformedBounds
============
*/
void xBounds::FromTransformedBounds(const xBounds &bounds, const xVec3 &origin, const xMat3 &axis) {
	int i;
	xVec3 center, extents, rotatedExtents;

	center = (bounds[0] + bounds[1]) * 0.5f;
	extents = bounds[1] - center;

	for (i = 0; i < 3; i++) {
		rotatedExtents[i] = xMath::Fabs(extents[0] * axis[0][i]) +
							xMath::Fabs(extents[1] * axis[1][i]) +
							xMath::Fabs(extents[2] * axis[2][i]);
	}

	center = origin + center * axis;
	b[0] = center - rotatedExtents;
	b[1] = center + rotatedExtents;
}

/*
============
xBounds::FromPoints

  Most tight bounds for a point set.
============
*/
void xBounds::FromPoints(const xVec3 *points, const int numPoints) {
	xSIMD::Processor->MinMax(b[0], b[1], points, numPoints);
}

/*
============
xBounds::FromPointTranslation

  Most tight bounds for the translational movement of the given point.
============
*/
void xBounds::FromPointTranslation(const xVec3 &point, const xVec3 &translation) {
	int i;

	for (i = 0; i < 3; i++) {
		if (translation[i] < 0.0f) {
			b[0][i] = point[i] + translation[i];
			b[1][i] = point[i];
		}
		else {
			b[0][i] = point[i];
			b[1][i] = point[i] + translation[i];
		}
	}
}

/*
============
xBounds::FromBoundsTranslation

  Most tight bounds for the translational movement of the given bounds.
============
*/
void xBounds::FromBoundsTranslation(const xBounds &bounds, const xVec3 &origin, const xMat3 &axis, const xVec3 &translation) {
	int i;

	if (axis.IsRotated()) {
		FromTransformedBounds(bounds, origin, axis);
	}
	else {
		b[0] = bounds[0] + origin;
		b[1] = bounds[1] + origin;
	}
	for (i = 0; i < 3; i++) {
		if (translation[i] < 0.0f) {
			b[0][i] += translation[i];
		}
		else {
			b[1][i] += translation[i];
		}
	}
}

/*
================
BoundsForPointRotation

  only for rotations < 180 degrees
================
*/
xBounds BoundsForPointRotation(const xVec3 &start, const xRotation &rotation) {
	int i;
	float radiusSqr;
	xVec3 v1, v2;
	xVec3 origin, axis, end;
	xBounds bounds;

	end = start * rotation;
	axis = rotation.Vec();
	origin = rotation.Origin() + axis * (axis * (start - rotation.Origin()));
	radiusSqr = (start - origin).LengthSqr();
	v1 = (start - origin).Cross(axis);
	v2 = (end - origin).Cross(axis);

	for (i = 0; i < 3; i++) {
		// if the derivative changes sign along this axis during the rotation from start to end
		if ((v1[i] > 0.0f && v2[i] < 0.0f) || (v1[i] < 0.0f && v2[i] > 0.0f)) {
			if ((0.5f * (start[i] + end[i]) - origin[i]) > 0.0f) {
				bounds[0][i] = Min(start[i], end[i]);
				bounds[1][i] = origin[i] + xMath::Sqrt(radiusSqr * (1.0f - axis[i] * axis[i]));
			}
			else {
				bounds[0][i] = origin[i] - xMath::Sqrt(radiusSqr * (1.0f - axis[i] * axis[i]));
				bounds[1][i] = Max(start[i], end[i]);
			}
		}
		else if (start[i] > end[i]) {
			bounds[0][i] = end[i];
			bounds[1][i] = start[i];
		}
		else {
			bounds[0][i] = start[i];
			bounds[1][i] = end[i];
		}
	}

	return bounds;
}

/*
============
xBounds::FromPointRotation

  Most tight bounds for the rotational movement of the given point.
============
*/
void xBounds::FromPointRotation(const xVec3 &point, const xRotation &rotation) {
	float radius;

	if (xMath::Fabs(rotation.Angle()) < 180.0f) {
		(*this) = BoundsForPointRotation(point, rotation);
	}
	else {

		radius = (point - rotation.Origin()).Length();

		// FIXME: these bounds are usually way larger
		b[0].Set(-radius, -radius, -radius);
		b[1].Set(radius, radius, radius);
	}
}

/*
============
xBounds::FromBoundsRotation

  Most tight bounds for the rotational movement of the given bounds.
============
*/
void xBounds::FromBoundsRotation(const xBounds &bounds, const xVec3 &origin, const xMat3 &axis, const xRotation &rotation) {
	int i;
	float radius;
	xVec3 point;
	xBounds rBounds;

	if (xMath::Fabs(rotation.Angle()) < 180.0f) {

		(*this) = BoundsForPointRotation(bounds[0] * axis + origin, rotation);
		for (i = 1; i < 8; i++) {
			point[0] = bounds[(i^(i>>1))&1][0];
			point[1] = bounds[(i>>1)&1][1];
			point[2] = bounds[(i>>2)&1][2];
			(*this) += BoundsForPointRotation(point * axis + origin, rotation);
		}
	}
	else {

		point = (bounds[1] - bounds[0]) * 0.5f;
		radius = (bounds[1] - point).Length() + (point - rotation.Origin()).Length();

		// FIXME: these bounds are usually way larger
		b[0].Set(-radius, -radius, -radius);
		b[1].Set(radius, radius, radius);
	}
}

/*
============
xBounds::ToPoints
============
*/
void xBounds::ToPoints(xVec3 points[8]) const {
	for (int i = 0; i < 8; i++) {
		points[i][0] = b[(i^(i>>1))&1][0];
		points[i][1] = b[(i>>1)&1][1];
		points[i][2] = b[(i>>2)&1][2];
	}
}

/*
============
xBounds::ToPlanes
============
*/
void xBounds::ToPlanes(xPlane planes[6]) const
{
  planes[0].SetNormal(xVec3(1,0,0));
  planes[0].SetDist(b[1][0]); //FitThroughPoint(b[1]);

  planes[1].SetNormal(xVec3(0,1,0));
  planes[1].SetDist(b[1][1]); //FitThroughPoint(b[1]);

  planes[2].SetNormal(xVec3(0,0,1));
  planes[2].SetDist(b[1][2]); //FitThroughPoint(b[1]);

  planes[3].SetNormal(xVec3(-1,0,0));
  planes[3].SetDist(-b[0][0]); //FitThroughPoint(b[0]);

  planes[4].SetNormal(xVec3(0,-1,0));
  planes[4].SetDist(-b[0][1]); //FitThroughPoint(b[0]);

  planes[5].SetNormal(xVec3(0,0,-1));
  planes[5].SetDist(-b[0][2]); //FitThroughPoint(b[0]);
}
void xBounds::ToPlanes(xPlaneExact planes[6]) const
{
  planes[0].SetNormal(xVec3d(1,0,0));
  planes[0].SetDist(b[1][0]); //FitThroughPoint(b[1]);

  planes[1].SetNormal(xVec3d(0,1,0));
  planes[1].SetDist(b[1][1]); //FitThroughPoint(b[1]);

  planes[2].SetNormal(xVec3d(0,0,1));
  planes[2].SetDist(b[1][2]); //FitThroughPoint(b[1]);

  planes[3].SetNormal(xVec3d(-1,0,0));
  planes[3].SetDist(-b[0][0]); //FitThroughPoint(b[0]);

  planes[4].SetNormal(xVec3d(0,-1,0));
  planes[4].SetDist(-b[0][1]); //FitThroughPoint(b[0]);

  planes[5].SetNormal(xVec3d(0,0,-1));
  planes[5].SetDist(-b[0][2]); //FitThroughPoint(b[0]);
}

// ========================================================================
// ========================================================================
// ========================================================================

/*
============
xBounds2::Radius
============
*/
float xBounds2::Radius() const {
  int		i;
  float	total, b0, b1;

  total = 0.0f;
  for (i = 0; i < 2; i++) {
    b0 = (float)xMath::Fabs(b[0][i]);
    b1 = (float)xMath::Fabs(b[1][i]);
    if (b0 > b1) {
      total += b0 * b0;
    } else {
      total += b1 * b1;
    }
  }
  return xMath::Sqrt(total);
}

/*
============
xBounds2::Radius
============
*/
float xBounds2::Radius(const xVec2 &center) const {
  int		i;
  float	total, b0, b1;

  total = 0.0f;
  for (i = 0; i < 2; i++) {
    b0 = (float)xMath::Fabs(center[i] - b[0][i]);
    b1 = (float)xMath::Fabs(b[1][i] - center[i]);
    if (b0 > b1) {
      total += b0 * b0;
    } else {
      total += b1 * b1;
    }
  }
  return xMath::Sqrt(total);
}


