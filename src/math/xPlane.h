#ifndef __X_PLANE_H__
#define __X_PLANE_H__

#pragma once

/*
===============================================================================

	3D plane with equation: a * x + b * y + c * z + d = 0

===============================================================================
*/

class xVec3;
class xMat3;

#define	ON_EPSILON					0.001f // 1 mm
#define DEGENERATE_DIST_EPSILON		1e-4f

#define	SIDE_FRONT				0
#define	SIDE_BACK					1
#define	SIDE_ON						2
#define	SIDE_CROSS				3

// plane types
#define PLANETYPE_X					  0
#define PLANETYPE_Y					  1
#define PLANETYPE_Z					  2
#define PLANETYPE_NEGX				3
#define PLANETYPE_NEGY				4
#define PLANETYPE_NEGZ				5
#define PLANETYPE_TRUEAXIAL		6	// all types < 6 are true axial planes
#define PLANETYPE_ZEROX				6
#define PLANETYPE_ZEROY				7
#define PLANETYPE_ZEROZ				8
#define PLANETYPE_NONAXIAL		9

class xPlane
{
public:
	xPlane();
	xPlane(float a, float b, float c, float d);
	xPlane(const xVec3 &normal, float dist);

	float			operator[](int index) const;
	float &			operator[](int index);
	xPlane			operator-() const;						// flips plane
	xPlane &		operator=(const xVec3 &v);			// sets normal and sets xPlane::d to zero
	xPlane			operator+(const xPlane &p) const;	// add plane equations
	xPlane			operator-(const xPlane &p) const;	// subtract plane equations
	xPlane &		operator*=(const xMat3 &m);			// Normal() *= m

	bool			Compare(const xPlane &p) const;						// exact compare, no epsilon
	bool			Compare(const xPlane &p, float epsilon) const;	// compare with epsilon
	bool			Compare(const xPlane &p, float normalEps, float distEps) const;	// compare with epsilon
	bool			operator==(	const xPlane &p) const;					// exact compare, no epsilon
	bool			operator!=(	const xPlane &p) const;					// exact compare, no epsilon

	void			Zero();							// zero plane
	void			SetNormal(const xVec3 &normal);		// sets the normal
	const xVec3 &	Normal() const;					// reference to const normal
	xVec3 &		Normal();							// reference to normal
	float			Normalize(bool fixDegenerate = true);	// only normalizes the plane normal, does not adjust d
	bool			FixDegenerateNormal();			// fix degenerate normal
	bool			FixDegeneracies(float distEpsilon);	// fix degenerate normal and dist
	float			Dist() const;						// returns: -d
	void			SetDist(float dist);			// sets: d = -dist
	int				Type() const;						// returns plane type

	bool			FromPoints(const xVec3 &p1, const xVec3 &p2, const xVec3 &p3, bool fixDegenerate = true);
	bool			FromVecs(const xVec3 &dir1, const xVec3 &dir2, const xVec3 &p, bool fixDegenerate = true);
	void			FitThroughPoint(const xVec3 &p);	// assumes normal is valid
	bool			HeightFit(const xVec3 *points, const int numPoints);
	xPlane			Translate(const xVec3 &translation) const;
	xPlane &		TranslateSelf(const xVec3 &translation);
	xPlane			Rotate(const xVec3 &origin, const xMat3 &axis) const;
	xPlane &		RotateSelf(const xVec3 &origin, const xMat3 &axis);

	double		Distance(const xVec3 &v) const;
	double		Distance(const xVec3d &v) const;
	int				Side(const xVec3 &v, float epsilon = 0.0f) const;

	bool			LineIntersection(const xVec3 &start, const xVec3 &end) const;
					// intersection point is start + dir * scale
	bool			RayIntersection(const xVec3 &start, const xVec3 &dir, float &scale) const;
	bool			PlaneIntersection(const xPlane &plane, xVec3 &start, xVec3 &dir) const;

	int				Dimension() const;

	const xVec4 &	ToVec4() const;
	xVec4 &	ToVec4();

  const float *	ToFloatPtr() const;
	float *	ToFloatPtr();
	xString ToString(int precision = 2) const;

private:
	float	a;
	float	b;
	float	c;
	float	d;
};

extern const xPlane plane_origin;
#define plane_zero plane_origin

X_INLINE xPlane::xPlane() {
}

X_INLINE xPlane::xPlane(float a, float b, float c, float d) {
	this->a = a;
	this->b = b;
	this->c = c;
	this->d = d;
}

X_INLINE xPlane::xPlane(const xVec3 &normal, float dist) {
	this->a = normal.x;
	this->b = normal.y;
	this->c = normal.z;
	this->d = -dist;
}

X_INLINE float xPlane::operator[](int index) const {
	return (&a)[ index ];
}

X_INLINE float& xPlane::operator[](int index) {
	return (&a)[ index ];
}

X_INLINE xPlane xPlane::operator-() const {
	return xPlane(-a, -b, -c, -d);
}

X_INLINE xPlane &xPlane::operator=(const xVec3 &v) { 
	a = v.x;
	b = v.y;
	c = v.z;
	d = 0;
	return *this;
}

X_INLINE xPlane xPlane::operator+(const xPlane &p) const {
	return xPlane(a + p.a, b + p.b, c + p.c, d + p.d);
}

X_INLINE xPlane xPlane::operator-(const xPlane &p) const {
	return xPlane(a - p.a, b - p.b, c - p.c, d - p.d);
}

X_INLINE xPlane &xPlane::operator*=(const xMat3 &m) {
	Normal() *= m;
	return *this;
}

X_INLINE bool xPlane::Compare(const xPlane &p) const {
	return (a == p.a && b == p.b && c == p.c && d == p.d);
}

X_INLINE bool xPlane::Compare(const xPlane &p, float epsilon) const {
	if (xMath::Fabs(a - p.a) > epsilon) {
		return false;
	}
			
	if (xMath::Fabs(b - p.b) > epsilon) {
		return false;
	}

	if (xMath::Fabs(c - p.c) > epsilon) {
		return false;
	}

	if (xMath::Fabs(d - p.d) > epsilon) {
		return false;
	}

	return true;
}

X_INLINE bool xPlane::Compare(const xPlane &p, float normalEps, float distEps) const {
	if (xMath::Fabs(d - p.d) > distEps) {
		return false;
	}
	if (!Normal().Compare(p.Normal(), normalEps)) {
		return false;
	}
	return true;
}

X_INLINE bool xPlane::operator==(const xPlane &p) const {
	return Compare(p);
}

X_INLINE bool xPlane::operator!=(const xPlane &p) const {
	return !Compare(p);
}

X_INLINE void xPlane::Zero() {
	a = b = c = d = 0.0f;
}

X_INLINE void xPlane::SetNormal(const xVec3 &normal) {
	a = normal.x;
	b = normal.y;
	c = normal.z;
}

X_INLINE const xVec3 &xPlane::Normal() const {
	return *(const xVec3*)&a;
}

X_INLINE xVec3 &xPlane::Normal() {
	return *(xVec3*)&a;
}

X_INLINE float xPlane::Normalize(bool fixDegenerate) {
	float length = ((xVec3*)&a)->Normalize();

	if (fixDegenerate) {
		FixDegenerateNormal();
	}
	return length;
}

X_INLINE bool xPlane::FixDegenerateNormal() {
	return Normal().FixDegenerateNormal();
}

X_INLINE bool xPlane::FixDegeneracies(float distEpsilon) {
	bool fixedNormal = FixDegenerateNormal();
	// only fix dist if the normal was degenerate
	if (fixedNormal) {
		if (xMath::Fabs(d - xMath::Rint(d)) < distEpsilon) {
			d = xMath::Rint(d);
		}
	}
	return fixedNormal;
}

X_INLINE float xPlane::Dist() const {
	return -d;
}

X_INLINE void xPlane::SetDist(float dist) {
	d = -dist;
}

X_INLINE bool xPlane::FromPoints(const xVec3 &p1, const xVec3 &p2, const xVec3 &p3, bool fixDegenerate) {
	Normal() = (p1 - p2).Cross(p3 - p2);
	if (Normalize(fixDegenerate) == 0.0f) {
		return false;
	}
	d = -(Normal() * p2);
	return true;
}

X_INLINE bool xPlane::FromVecs(const xVec3 &dir1, const xVec3 &dir2, const xVec3 &p, bool fixDegenerate) {
	Normal() = dir1.Cross(dir2);
	if (Normalize(fixDegenerate) == 0.0f) {
		return false;
	}
	d = -(Normal() * p);
	return true;
}

X_INLINE void xPlane::FitThroughPoint(const xVec3 &p) {
	d = -(Normal() * p);
}

X_INLINE xPlane xPlane::Translate(const xVec3 &translation) const {
	return xPlane(a, b, c, d - translation * Normal());
}

X_INLINE xPlane &xPlane::TranslateSelf(const xVec3 &translation) {
	d -= translation * Normal();
	return *this;
}

X_INLINE xPlane xPlane::Rotate(const xVec3 &origin, const xMat3 &axis) const {
	xPlane p;
	p.Normal() = Normal() * axis;
	p.d = d + origin * Normal() - origin * p.Normal();
	return p;
}

X_INLINE xPlane &xPlane::RotateSelf(const xVec3 &origin, const xMat3 &axis) {
	d += origin * Normal();
	Normal() *= axis;
	d -= origin * Normal();
	return *this;
}

X_INLINE double xPlane::Distance(const xVec3 &v) const {
	return (double)a * v.x + b * v.y + c * v.z + d;
}

X_INLINE double xPlane::Distance(const xVec3d &v) const {
	return (double)a * v.x + b * v.y + c * v.z + d;
}

X_INLINE int xPlane::Side(const xVec3 &v, float epsilon) const {
	double dist = Distance(v);
	if (dist > epsilon) {
		return SIDE_FRONT;
	}
	else if (dist < -epsilon) {
		return SIDE_BACK;
	}
	else {
		return SIDE_ON;
	}
}

X_INLINE bool xPlane::LineIntersection(const xVec3 &start, const xVec3 &end) const {
	float d1, d2, fraction;

	d1 = Normal() * start + d;
	d2 = Normal() * end + d;
	if (d1 == d2) {
		return false;
	}
	if (d1 > 0.0f && d2 > 0.0f) {
		return false;
	}
	if (d1 < 0.0f && d2 < 0.0f) {
		return false;
	}
	fraction = (d1 / (d1 - d2));
	return (fraction >= 0.0f && fraction <= 1.0f);
}

X_INLINE bool xPlane::RayIntersection(const xVec3 &start, const xVec3 &dir, float &scale) const {
	float d1, d2;

	d1 = Normal() * start + d;
	d2 = Normal() * dir;
	if (d2 == 0.0f) {
		return false;
	}
	scale = -(d1 / d2);
	return true;
}

X_INLINE int xPlane::Dimension() const {
	return 4;
}

X_INLINE const xVec4 &xPlane::ToVec4() const {
	return *(const xVec4*)(&a);
}

X_INLINE xVec4 &xPlane::ToVec4() {
	return *(xVec4*)&a;
}

X_INLINE const float *xPlane::ToFloatPtr() const {
	return (const float*)&a;
}

X_INLINE float *xPlane::ToFloatPtr() {
	return (float*)&a;
}

/*
===============================================================================

	3D plane with equation: a * x + b * y + c * z + d = 0

===============================================================================
*/

class xPlaneExact
{
public:
	xPlaneExact();
	xPlaneExact(const xPlane& p);
	xPlaneExact(double a, double b, double c, double d);
	xPlaneExact(const xVec3d &normal, double dist);

	double			operator[](int index) const;
	double &			operator[](int index);
	xPlaneExact			operator-() const;						// flips plane
	xPlaneExact &		operator=(const xVec3d &v);			// sets normal and sets xPlaneExact::d to zero
	xPlaneExact			operator+(const xPlaneExact &p) const;	// add plane equations
	xPlaneExact			operator-(const xPlaneExact &p) const;	// subtract plane equations
	xPlaneExact &		operator*=(const xMat3 &m);			// Normal() *= m

	bool			Compare(const xPlaneExact &p) const;						// exact compare, no epsilon
	bool			Compare(const xPlaneExact &p, double epsilon) const;	// compare with epsilon
	bool			Compare(const xPlaneExact &p, double normalEps, double distEps) const;	// compare with epsilon
	bool			operator==(	const xPlaneExact &p) const;					// exact compare, no epsilon
	bool			operator!=(	const xPlaneExact &p) const;					// exact compare, no epsilon

	void			Zero();							// zero plane
	void			SetNormal(const xVec3d &normal);		// sets the normal
	const xVec3d &	Normal() const;					// reference to const normal
	xVec3d &	Normal();							// reference to normal
	double		Normalize(bool fixDegenerate = true);	// only normalizes the plane normal, does not adjust d
	bool			FixDegenerateNormal();			// fix degenerate normal
	bool			FixDegeneracies(double distEpsilon);	// fix degenerate normal and dist
	double		Dist() const;						// returns: -d
	void			SetDist(double dist);			// sets: d = -dist
	int				Type() const;						// returns plane type

	bool			FromPoints(const xVec3d &p1, const xVec3d &p2, const xVec3d &p3, bool fixDegenerate = true);
	bool			FromPoints(const xVec3 &p1, const xVec3 &p2, const xVec3 &p3, bool fixDegenerate = true);
	bool			FromVecs(const xVec3d &dir1, const xVec3d &dir2, const xVec3d &p, bool fixDegenerate = true);
	void			FitThroughPoint(const xVec3d &p);	// assumes normal is valid
  void			FitThroughPoint(const xVec3 &p);	// assumes normal is valid
	bool			HeightFit(const xVec3d *points, const int numPoints);
	xPlaneExact			Translate(const xVec3d &translation) const;
	xPlaneExact &		TranslateSelf(const xVec3d &translation);
	xPlaneExact			Rotate(const xVec3d &origin, const xMat3 &axis) const;
	xPlaneExact &		RotateSelf(const xVec3d &origin, const xMat3 &axis);

	double		Distance(const xVec3 &v) const;
	double		Distance(const xVec3d &v) const;
	int				Side(const xVec3 &v, double epsilon = 0.0f) const;
	int				Side(const xVec3d &v, double epsilon = 0.0f) const;

	bool			LineIntersection(const xVec3d &start, const xVec3d &end) const;
					// intersection point is start + dir * scale
	bool			RayIntersection(const xVec3d &start, const xVec3d &dir, double &scale) const;
	bool			PlaneIntersection(const xPlaneExact &plane, xVec3d &start, xVec3d &dir) const;

	int				Dimension() const;

	const xVec4 &	ToVec4() const;
	xVec4 &	ToVec4();

  const double *	ToFloatPtr() const;
	double *	ToFloatPtr();
	xString ToString(int precision = 2) const;

private:
	double	a;
	double	b;
	double	c;
	double	d;
};

extern const xPlaneExact plane_ex_origin;
#define plane_ex_zero plane_ex_origin

X_INLINE xPlaneExact::xPlaneExact() {
}

X_INLINE xPlaneExact::xPlaneExact(const xPlane& p)
{
	this->a = p[0];
	this->b = p[1];
	this->c = p[2];
	this->d = p[3];
}

X_INLINE xPlaneExact::xPlaneExact(double a, double b, double c, double d) {
	this->a = a;
	this->b = b;
	this->c = c;
	this->d = d;
}

X_INLINE xPlaneExact::xPlaneExact(const xVec3d &normal, double dist) {
	this->a = normal.x;
	this->b = normal.y;
	this->c = normal.z;
	this->d = -dist;
}

X_INLINE double xPlaneExact::operator[](int index) const {
	return (&a)[ index ];
}

X_INLINE double& xPlaneExact::operator[](int index) {
	return (&a)[ index ];
}

X_INLINE xPlaneExact xPlaneExact::operator-() const {
	return xPlaneExact(-a, -b, -c, -d);
}

X_INLINE xPlaneExact &xPlaneExact::operator=(const xVec3d &v) { 
	a = v.x;
	b = v.y;
	c = v.z;
	d = 0;
	return *this;
}

X_INLINE xPlaneExact xPlaneExact::operator+(const xPlaneExact &p) const {
	return xPlaneExact(a + p.a, b + p.b, c + p.c, d + p.d);
}

X_INLINE xPlaneExact xPlaneExact::operator-(const xPlaneExact &p) const {
	return xPlaneExact(a - p.a, b - p.b, c - p.c, d - p.d);
}

X_INLINE xPlaneExact &xPlaneExact::operator*=(const xMat3 &m) {
	Normal() *= m;
	return *this;
}

X_INLINE bool xPlaneExact::Compare(const xPlaneExact &p) const {
	return (a == p.a && b == p.b && c == p.c && d == p.d);
}

X_INLINE bool xPlaneExact::Compare(const xPlaneExact &p, double epsilon) const {
	if (fabs(a - p.a) > epsilon) {
		return false;
	}
			
	if (fabs(b - p.b) > epsilon) {
		return false;
	}

	if (fabs(c - p.c) > epsilon) {
		return false;
	}

	if (fabs(d - p.d) > epsilon) {
		return false;
	}

	return true;
}

X_INLINE bool xPlaneExact::Compare(const xPlaneExact &p, double normalEps, double distEps) const {
	if (fabs(d - p.d) > distEps) {
		return false;
	}
	if (!Normal().Compare(p.Normal(), normalEps)) {
		return false;
	}
	return true;
}

X_INLINE bool xPlaneExact::operator==(const xPlaneExact &p) const {
	return Compare(p);
}

X_INLINE bool xPlaneExact::operator!=(const xPlaneExact &p) const {
	return !Compare(p);
}

X_INLINE void xPlaneExact::Zero() {
	a = b = c = d = 0.0f;
}

X_INLINE void xPlaneExact::SetNormal(const xVec3d &normal) {
	a = normal.x;
	b = normal.y;
	c = normal.z;
}

X_INLINE const xVec3d &xPlaneExact::Normal() const {
	return *(const xVec3d*)&a;
}

X_INLINE xVec3d &xPlaneExact::Normal() {
	return *(xVec3d*)&a;
}

X_INLINE double xPlaneExact::Normalize(bool fixDegenerate) {
	double length = ((xVec3d*)&a)->Normalize();

	if (fixDegenerate) {
		FixDegenerateNormal();
	}
	return length;
}

X_INLINE bool xPlaneExact::FixDegenerateNormal() {
	return Normal().FixDegenerateNormal();
}

X_INLINE bool xPlaneExact::FixDegeneracies(double distEpsilon) {
	bool fixedNormal = FixDegenerateNormal();
	// only fix dist if the normal was degenerate
	if (fixedNormal) {
		if (fabs(d - xMath::Rint(d)) < distEpsilon) {
			d = xMath::Rint(d);
		}
	}
	return fixedNormal;
}

X_INLINE double xPlaneExact::Dist() const {
	return -d;
}

X_INLINE void xPlaneExact::SetDist(double dist) {
	d = -dist;
}

X_INLINE bool xPlaneExact::FromPoints(const xVec3d &p1, const xVec3d &p2, const xVec3d &p3, bool fixDegenerate) {
	Normal() = (p1 - p2).Cross(p3 - p2);
	if (Normalize(fixDegenerate) == 0.0f) {
		return false;
	}
	d = -(Normal() * p2);
	return true;
}

X_INLINE bool xPlaneExact::FromPoints(const xVec3 &p1, const xVec3 &p2, const xVec3 &p3, bool fixDegenerate) 
{
  return FromPoints(p1.ToVec3d(),p2.ToVec3d(),p3.ToVec3d(),fixDegenerate);
}

X_INLINE bool xPlaneExact::FromVecs(const xVec3d &dir1, const xVec3d &dir2, const xVec3d &p, bool fixDegenerate) {
	Normal() = dir1.Cross(dir2);
	if (Normalize(fixDegenerate) == 0.0f) {
		return false;
	}
	d = -(Normal() * p);
	return true;
}

X_INLINE void xPlaneExact::FitThroughPoint(const xVec3d &p) {
	d = -(Normal() * p);
}

X_INLINE void xPlaneExact::FitThroughPoint(const xVec3 &p) {
  d = -(Normal() * p.ToVec3d());
}

X_INLINE xPlaneExact xPlaneExact::Translate(const xVec3d &translation) const {
	return xPlaneExact(a, b, c, d - translation * Normal());
}

X_INLINE xPlaneExact &xPlaneExact::TranslateSelf(const xVec3d &translation) {
	d -= translation * Normal();
	return *this;
}

X_INLINE xPlaneExact xPlaneExact::Rotate(const xVec3d &origin, const xMat3 &axis) const {
	xPlaneExact p;
	p.Normal() = Normal() * axis;
	p.d = d + origin * Normal() - origin * p.Normal();
	return p;
}

X_INLINE xPlaneExact &xPlaneExact::RotateSelf(const xVec3d &origin, const xMat3 &axis) {
	d += origin * Normal();
	Normal() *= axis;
	d -= origin * Normal();
	return *this;
}

X_INLINE double xPlaneExact::Distance(const xVec3 &v) const {
	return (double)a * v.x + b * v.y + c * v.z + d;
}

X_INLINE double xPlaneExact::Distance(const xVec3d &v) const {
	return (double)a * v.x + b * v.y + c * v.z + d;
}

X_INLINE int xPlaneExact::Side(const xVec3 &v, double epsilon) const {
	double dist = Distance(v);
	if (dist > epsilon) {
		return SIDE_FRONT;
	}
	else if (dist < -epsilon) {
		return SIDE_BACK;
	}
	else {
		return SIDE_ON;
	}
}

X_INLINE int xPlaneExact::Side(const xVec3d &v, double epsilon) const {
	double dist = Distance(v);
	if (dist > epsilon) {
		return SIDE_FRONT;
	}
	else if (dist < -epsilon) {
		return SIDE_BACK;
	}
	else {
		return SIDE_ON;
	}
}

X_INLINE bool xPlaneExact::LineIntersection(const xVec3d &start, const xVec3d &end) const {
	double d1, d2, fraction;

	d1 = Normal() * start + d;
	d2 = Normal() * end + d;
	if (d1 == d2) {
		return false;
	}
	if (d1 > 0.0f && d2 > 0.0f) {
		return false;
	}
	if (d1 < 0.0f && d2 < 0.0f) {
		return false;
	}
	fraction = (d1 / (d1 - d2));
	return (fraction >= 0.0f && fraction <= 1.0f);
}

X_INLINE bool xPlaneExact::RayIntersection(const xVec3d &start, const xVec3d &dir, double &scale) const {
	double d1, d2;

	d1 = Normal() * start + d;
	d2 = Normal() * dir;
	if (d2 == 0.0f) {
		return false;
	}
	scale = -(d1 / d2);
	return true;
}

X_INLINE int xPlaneExact::Dimension() const {
	return 4;
}

X_INLINE const xVec4 &xPlaneExact::ToVec4() const {
	return *(const xVec4*)(&a);
}

X_INLINE xVec4 &xPlaneExact::ToVec4() {
	return *(xVec4*)&a;
}

X_INLINE const double *xPlaneExact::ToFloatPtr() const {
	return (const double*)&a;
}

X_INLINE double *xPlaneExact::ToFloatPtr() {
	return (double*)&a;
}

#endif /* !__X_PLANE_H__ */
