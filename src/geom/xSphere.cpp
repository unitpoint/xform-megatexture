#include <xForm.h>

const xSphere sphere_zero(vec3_zero, 0.0f);


/*
================
xSphere::PlaneDistance
================
*/
float xSphere::PlaneDistance(const xPlane &plane) const {
	float d;

	d = plane.Distance(origin);
	if (d > radius) {
		return d - radius;
	}
	if (d < -radius) {
		return d + radius;
	}
	return 0.0f;
}

/*
================
xSphere::PlaneSide
================
*/
int xSphere::PlaneSide(const xPlane &plane, const float epsilon) const {
	float d;

	d = plane.Distance(origin);
	if (d > radius + epsilon) {
		return SIDE_FRONT;
	}
	if (d < -radius - epsilon) {
		return SIDE_BACK;
	}
	return SIDE_CROSS;
}

/*
============
xSphere::LineIntersection

  Returns true if the line intersects the sphere between the start and end point.
============
*/
bool xSphere::LineIntersection(const xVec3 &start, const xVec3 &end) const {
	xVec3 r, s, e;
	float a;

	s = start - origin;
	e = end - origin;
	r = e - s;
	a = -s * r;
	if (a <= 0) {
		return (s * s < radius * radius);
	}
	else if (a >= r * r) {
		return (e * e < radius * radius);
	}
	else {
		r = s + (a / (r * r)) * r;
		return (r * r < radius * radius);
	}
}

/*
============
xSphere::RayIntersection

  Returns true if the ray intersects the sphere.
  The ray can intersect the sphere in both directions from the start point.
  If start is inside the sphere then scale1 < 0 and scale2 > 0.
============
*/
bool xSphere::RayIntersection(const xVec3 &start, const xVec3 &dir, float &scale1, float &scale2) const {
	double a, b, c, d, sqrtd;
	xVec3 p;

	p = start - origin;
	a = dir * dir;
	b = dir * p;
	c = p * p - radius * radius;
	d = b * b - c * a;

	if (d < 0.0f) {
		return false;
	}

	sqrtd = xMath::Sqrt(d);
	a = 1.0f / a;

	scale1 = (-b + sqrtd) * a;
	scale2 = (-b - sqrtd) * a;

	return true;
}

/*
============
xSphere::FromPoints

  Tight sphere for a point set.
============
*/
void xSphere::FromPoints(const xVec3 *points, const int numPoints) {
	int i;
	float radiusSqr, dist;
	xVec3 mins, maxs;

	xSIMD::Processor->MinMax(mins, maxs, points, numPoints);

	origin = (mins + maxs) * 0.5f;

	radiusSqr = 0.0f;
	for (i = 0; i < numPoints; i++) {
		dist = (points[i] - origin).LengthSqr();
		if (dist > radiusSqr) {
			radiusSqr = dist;
		}
	}
	radius = xMath::Sqrt(radiusSqr);
}
