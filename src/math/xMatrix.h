#ifndef __X_MATRIX_H__
#define __X_MATRIX_H__

#pragma once

/*
===============================================================================

  Matrix classes, all matrices are row-major except xMat3

===============================================================================
*/

#define MATRIX_INVERSE_EPSILON  1e-14
#define MATRIX_EPSILON				  1e-6

class xAngles;
class xQuat;
class xCQuat;
class xRotation;
class xMat4;

/*

  Matrix * Vec = VectorIRotate
  Matrix::ProjectVector = VectorRotate

*/

//===============================================================
//
//	xMat2 - 2x2 matrix
//
//===============================================================

class xMat2 {
public:
	xMat2();
	explicit xMat2(const xVec2 &x, const xVec2 &y);
	explicit xMat2(float xx, float xy, float yx, float yy);
	explicit xMat2(float src[ 2 ][ 2 ]);

	const xVec2 &	operator[](int index) const;
	xVec2 &		operator[](int index);
	xMat2			operator-() const;
	xMat2			operator*(float a) const;
	xVec2			operator*(const xVec2 &vec) const;
	xMat2			operator*(const xMat2 &a) const;
	xMat2			operator+(const xMat2 &a) const;
	xMat2			operator-(const xMat2 &a) const;
	xMat2 &		operator*=(float a);
	xMat2 &		operator*=(const xMat2 &a);
	xMat2 &		operator+=(const xMat2 &a);
	xMat2 &		operator-=(const xMat2 &a);

	friend xMat2	operator*(float a, const xMat2 &mat);
	friend xVec2	operator*(const xVec2 &vec, const xMat2 &mat);
	friend xVec2 &	operator*=(xVec2 &vec, const xMat2 &mat);

	bool			Compare(const xMat2 &a) const;						// exact compare, no epsilon
	bool			Compare(const xMat2 &a, float epsilon) const;	// compare with epsilon
	bool			operator==(const xMat2 &a) const;					// exact compare, no epsilon
	bool			operator!=(const xMat2 &a) const;					// exact compare, no epsilon

	void			Zero();
	void			Identity();
	bool			IsIdentity(float epsilon = MATRIX_EPSILON) const;
	bool			IsSymmetric(float epsilon = MATRIX_EPSILON) const;
	bool			IsDiagonal(float epsilon = MATRIX_EPSILON) const;

	float			Trace() const;
	float			Determinant() const;
	xMat2			Transpose() const;	// returns transpose
	xMat2 &		TransposeSelf();
	xMat2			Inverse() const;		// returns the inverse (m * m.Inverse() = identity)
	bool			InverseSelf();		// returns false if determinant is zero
	xMat2			InverseFast() const;	// returns the inverse (m * m.Inverse() = identity)
	bool			InverseFastSelf();	// returns false if determinant is zero

	int				Dimension() const;

	const float *	ToFloatPtr() const;
	float *			ToFloatPtr();
	xString ToString(int precision = 2) const;

private:
	xVec2			mat[ 2 ];
};

extern const xMat2 mat2_zero;
extern const xMat2 mat2_identity;
#define mat2_default	mat2_identity

X_INLINE xMat2::xMat2() {
}

X_INLINE xMat2::xMat2(const xVec2 &x, const xVec2 &y) {
	mat[ 0 ].x = x.x; mat[ 0 ].y = x.y;
	mat[ 1 ].x = y.x; mat[ 1 ].y = y.y;
}

X_INLINE xMat2::xMat2(float xx, float xy, float yx, float yy) {
	mat[ 0 ].x = xx; mat[ 0 ].y = xy;
	mat[ 1 ].x = yx; mat[ 1 ].y = yy;
}

X_INLINE xMat2::xMat2(float src[ 2 ][ 2 ]) {
	memcpy(mat, src, 2 * 2 * sizeof(float));
}

X_INLINE const xVec2 &xMat2::operator[](int index) const {
	//assert((index >= 0) && (index < 2));
	return mat[ index ];
}

X_INLINE xVec2 &xMat2::operator[](int index) {
	//assert((index >= 0) && (index < 2));
	return mat[ index ];
}

X_INLINE xMat2 xMat2::operator-() const {
	return xMat2(	-mat[0][0], -mat[0][1],
					-mat[1][0], -mat[1][1]);
}

X_INLINE xVec2 xMat2::operator*(const xVec2 &vec) const {
	return xVec2(
		mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y,
		mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y);
}

X_INLINE xMat2 xMat2::operator*(const xMat2 &a) const {
	return xMat2(
		mat[0].x * a[0].x + mat[0].y * a[1].x,
		mat[0].x * a[0].y + mat[0].y * a[1].y,
		mat[1].x * a[0].x + mat[1].y * a[1].x,
		mat[1].x * a[0].y + mat[1].y * a[1].y);
}

X_INLINE xMat2 xMat2::operator*(float a) const {
	return xMat2(
		mat[0].x * a, mat[0].y * a, 
		mat[1].x * a, mat[1].y * a);
}

X_INLINE xMat2 xMat2::operator+(const xMat2 &a) const {
	return xMat2(
		mat[0].x + a[0].x, mat[0].y + a[0].y, 
		mat[1].x + a[1].x, mat[1].y + a[1].y);
}
    
X_INLINE xMat2 xMat2::operator-(const xMat2 &a) const {
	return xMat2(
		mat[0].x - a[0].x, mat[0].y - a[0].y,
		mat[1].x - a[1].x, mat[1].y - a[1].y);
}

X_INLINE xMat2 &xMat2::operator*=(float a) {
	mat[0].x *= a; mat[0].y *= a;
	mat[1].x *= a; mat[1].y *= a;

    return *this;
}

X_INLINE xMat2 &xMat2::operator*=(const xMat2 &a) {
	float x, y;
	x = mat[0].x; y = mat[0].y;
	mat[0].x = x * a[0].x + y * a[1].x;
	mat[0].y = x * a[0].y + y * a[1].y;
	x = mat[1].x; y = mat[1].y;
	mat[1].x = x * a[0].x + y * a[1].x;
	mat[1].y = x * a[0].y + y * a[1].y;
	return *this;
}

X_INLINE xMat2 &xMat2::operator+=(const xMat2 &a) {
	mat[0].x += a[0].x; mat[0].y += a[0].y;
	mat[1].x += a[1].x; mat[1].y += a[1].y;

    return *this;
}

X_INLINE xMat2 &xMat2::operator-=(const xMat2 &a) {
	mat[0].x -= a[0].x; mat[0].y -= a[0].y;
	mat[1].x -= a[1].x; mat[1].y -= a[1].y;

    return *this;
}

X_INLINE xVec2 operator*(const xVec2 &vec, const xMat2 &mat) {
	return mat * vec;
}

X_INLINE xMat2 operator*(float a, xMat2 const &mat) {
	return mat * a;
}

X_INLINE xVec2 &operator*=(xVec2 &vec, const xMat2 &mat) {
	vec = mat * vec;
	return vec;
}

X_INLINE bool xMat2::Compare(const xMat2 &a) const {
	if (mat[0].Compare(a[0]) &&
		mat[1].Compare(a[1])) {
		return true;
	}
	return false;
}

X_INLINE bool xMat2::Compare(const xMat2 &a, float epsilon) const {
	if (mat[0].Compare(a[0], epsilon) &&
		mat[1].Compare(a[1], epsilon)) {
		return true;
	}
	return false;
}

X_INLINE bool xMat2::operator==(const xMat2 &a) const {
	return Compare(a);
}

X_INLINE bool xMat2::operator!=(const xMat2 &a) const {
	return !Compare(a);
}

X_INLINE void xMat2::Zero() {
	mat[0].Zero();
	mat[1].Zero();
}

X_INLINE void xMat2::Identity() {
	*this = mat2_identity;
}

X_INLINE bool xMat2::IsIdentity(float epsilon) const {
	return Compare(mat2_identity, epsilon);
}

X_INLINE bool xMat2::IsSymmetric(float epsilon) const {
	return (xMath::Fabs(mat[0][1] - mat[1][0]) < epsilon);
}

X_INLINE bool xMat2::IsDiagonal(float epsilon) const {
	if (xMath::Fabs(mat[0][1]) > epsilon ||
		xMath::Fabs(mat[1][0]) > epsilon) {
		return false;
	}
	return true;
}

X_INLINE float xMat2::Trace() const {
	return (mat[0][0] + mat[1][1]);
}

X_INLINE float xMat2::Determinant() const {
	return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
}

X_INLINE xMat2 xMat2::Transpose() const {
	return xMat2(	mat[0][0], mat[1][0],
					mat[0][1], mat[1][1]);
}

X_INLINE xMat2 &xMat2::TransposeSelf() {
	float tmp;

	tmp = mat[0][1];
	mat[0][1] = mat[1][0];
	mat[1][0] = tmp;

	return *this;
}

X_INLINE xMat2 xMat2::Inverse() const {
	xMat2 invMat;

	invMat = *this;
	int r = invMat.InverseSelf();
	assert(r);
	return invMat;
}

X_INLINE xMat2 xMat2::InverseFast() const {
	xMat2 invMat;

	invMat = *this;
	int r = invMat.InverseFastSelf();
	assert(r);
	return invMat;
}

X_INLINE int xMat2::Dimension() const {
	return 4;
}

X_INLINE const float *xMat2::ToFloatPtr() const {
	return mat[0].ToFloatPtr();
}

X_INLINE float *xMat2::ToFloatPtr() {
	return mat[0].ToFloatPtr();
}


//===============================================================
//
//	xMat3 - 3x3 matrix
//
//	NOTE:	matrix is column-major
//
//===============================================================

class xMat3
{
public:
	xMat3();
	xMat3(const xVec3 &x, const xVec3 &y, const xVec3 &z);
	xMat3(float xx, float xy, float xz, float yx, float yy, float yz, float zx, float zy, float zz);
	xMat3(float src[ 3 ][ 3 ]);

	const xVec3 &	operator[](int index) const;
	xVec3 &		operator[](int index);
	xMat3			operator-() const;
	xMat3			operator*(float a) const;
	xVec3			operator*(const xVec3 &vec) const;
	xVec3d		operator*(const xVec3d &vec) const;
	xMat3			operator*(const xMat3 &a) const;
	xMat3			operator+(const xMat3 &a) const;
	xMat3			operator-(const xMat3 &a) const;
	xMat3 &		operator*=(float a);
	xMat3 &		operator*=(const xMat3 &a);
	xMat3 &		operator+=(const xMat3 &a);
	xMat3 &		operator-=(const xMat3 &a);

	friend xMat3	operator*(float a, const xMat3 &mat);
	friend xVec3	operator*(const xVec3 &vec, const xMat3 &mat);
	friend xVec3 &	operator*=(xVec3 &vec, const xMat3 &mat);
	friend xVec3d	operator*(const xVec3d &vec, const xMat3 &mat);
	friend xVec3d &	operator*=(xVec3d &vec, const xMat3 &mat);

	bool			Compare(const xMat3 &a) const;						// exact compare, no epsilon
	bool			Compare(const xMat3 &a, float epsilon) const;	// compare with epsilon
	bool			operator==(const xMat3 &a) const;					// exact compare, no epsilon
	bool			operator!=(const xMat3 &a) const;					// exact compare, no epsilon

	void			Zero();
	void			Identity();
	bool			IsIdentity(float epsilon = MATRIX_EPSILON) const;
	bool			IsSymmetric(float epsilon = MATRIX_EPSILON) const;
	bool			IsDiagonal(float epsilon = MATRIX_EPSILON) const;
	bool			IsRotated() const;

	xVec3     ProjectVector(const xVec3 &src) const;
	xVec3     UnprojectVector(const xVec3 &src) const;

  xVec3     Rotate(const xVec3& src) const;
  xVec3     InverseRotate(const xVec3& src) const;

	bool			FixDegeneracies();	// fix degenerate axial cases
	bool			FixDenormals();		// change tiny numbers to zero

	float			Trace() const;
	float			Determinant() const;
	xMat3			OrthoNormalize() const;
	xMat3 &		OrthoNormalizeSelf();
	xMat3			Transpose() const;	// returns transpose
	xMat3 &		TransposeSelf();
	xMat3			Inverse() const;		// returns the inverse (m * m.Inverse() = identity)
	bool			InverseSelf();		// returns false if determinant is zero
	xMat3			InverseFast() const;	// returns the inverse (m * m.Inverse() = identity)
	bool			InverseFastSelf();	// returns false if determinant is zero
	xMat3			TransposeMultiply(const xMat3 &b) const;

	int				Dimension() const;

	xMat3 &		SetVectors(const xVec3& forward, const xVec3& right, const xVec3& up);
	void			ToVectors(xVec3 *forward, xVec3 *right = NULL, xVec3 *up = NULL) const;

  xAngles		ToAngles() const;
	xQuat			ToQuat() const;
	xCQuat			ToCQuat() const;
	xRotation		ToRotation() const;
	xMat4			ToMat4() const;
	xVec3			ToAngularVelocity() const;
	const float *	ToFloatPtr() const;
	float *			ToFloatPtr();
	xString ToString(int precision = 2) const;

	friend void		TransposeMultiply(const xMat3 &inv, const xMat3 &b, xMat3 &dst);
	friend xMat3	SkewSymmetric(xVec3 const &src);

private:
	xVec3			mat[ 3 ];
};

extern const xMat3 mat3_zero;
extern const xMat3 mat3_identity;
#define mat3_default	mat3_identity

X_INLINE xMat3::xMat3() {
}

X_INLINE xMat3::xMat3(const xVec3 &x, const xVec3 &y, const xVec3 &z) {
	mat[ 0 ].x = x.x; mat[ 0 ].y = x.y; mat[ 0 ].z = x.z;
	mat[ 1 ].x = y.x; mat[ 1 ].y = y.y; mat[ 1 ].z = y.z;
	mat[ 2 ].x = z.x; mat[ 2 ].y = z.y; mat[ 2 ].z = z.z;
}

X_INLINE xMat3::xMat3(float xx, float xy, float xz, float yx, float yy, float yz, float zx, float zy, float zz) {
	mat[ 0 ].x = xx; mat[ 0 ].y = xy; mat[ 0 ].z = xz;
	mat[ 1 ].x = yx; mat[ 1 ].y = yy; mat[ 1 ].z = yz;
	mat[ 2 ].x = zx; mat[ 2 ].y = zy; mat[ 2 ].z = zz;
}

X_INLINE xMat3::xMat3(float src[ 3 ][ 3 ]) {
	memcpy(mat, src, 3 * 3 * sizeof(float));
}

X_INLINE const xVec3 &xMat3::operator[](int index) const {
	//assert((index >= 0) && (index < 3));
	return mat[ index ];
}

X_INLINE xVec3 &xMat3::operator[](int index) {
	//assert((index >= 0) && (index < 3));
	return mat[ index ];
}

X_INLINE xMat3 xMat3::operator-() const {
	return xMat3(	-mat[0][0], -mat[0][1], -mat[0][2],
					-mat[1][0], -mat[1][1], -mat[1][2],
					-mat[2][0], -mat[2][1], -mat[2][2]);
}

X_INLINE xVec3 xMat3::operator*(const xVec3 &vec) const {
	return xVec3(
		mat[ 0 ].x * vec.x + mat[ 1 ].x * vec.y + mat[ 2 ].x * vec.z,
		mat[ 0 ].y * vec.x + mat[ 1 ].y * vec.y + mat[ 2 ].y * vec.z,
		mat[ 0 ].z * vec.x + mat[ 1 ].z * vec.y + mat[ 2 ].z * vec.z);
}

X_INLINE xVec3d xMat3::operator*(const xVec3d &vec) const {
	return xVec3d(
		mat[ 0 ].x * vec.x + mat[ 1 ].x * vec.y + mat[ 2 ].x * vec.z,
		mat[ 0 ].y * vec.x + mat[ 1 ].y * vec.y + mat[ 2 ].y * vec.z,
		mat[ 0 ].z * vec.x + mat[ 1 ].z * vec.y + mat[ 2 ].z * vec.z);
}

X_INLINE xMat3 xMat3::operator*(const xMat3 &a) const {
	int i, j;
	float *m1Ptr, *m2Ptr;
	float *dstPtr;
	xMat3 dst;

	m1Ptr = (float*)this;
	m2Ptr = (float*)&a;
	dstPtr = (float*)&dst;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			*dstPtr = m1Ptr[0] * m2Ptr[ 0 * 3 + j ]
					+ m1Ptr[1] * m2Ptr[ 1 * 3 + j ]
					+ m1Ptr[2] * m2Ptr[ 2 * 3 + j ];
			dstPtr++;
		}
		m1Ptr += 3;
	}
	return dst;
}

X_INLINE xMat3 xMat3::operator*(float a) const {
	return xMat3(
		mat[0].x * a, mat[0].y * a, mat[0].z * a,
		mat[1].x * a, mat[1].y * a, mat[1].z * a,
		mat[2].x * a, mat[2].y * a, mat[2].z * a);
}

X_INLINE xMat3 xMat3::operator+(const xMat3 &a) const {
	return xMat3(
		mat[0].x + a[0].x, mat[0].y + a[0].y, mat[0].z + a[0].z,
		mat[1].x + a[1].x, mat[1].y + a[1].y, mat[1].z + a[1].z,
		mat[2].x + a[2].x, mat[2].y + a[2].y, mat[2].z + a[2].z);
}
    
X_INLINE xMat3 xMat3::operator-(const xMat3 &a) const {
	return xMat3(
		mat[0].x - a[0].x, mat[0].y - a[0].y, mat[0].z - a[0].z,
		mat[1].x - a[1].x, mat[1].y - a[1].y, mat[1].z - a[1].z,
		mat[2].x - a[2].x, mat[2].y - a[2].y, mat[2].z - a[2].z);
}

X_INLINE xMat3 &xMat3::operator*=(float a) {
	mat[0].x *= a; mat[0].y *= a; mat[0].z *= a;
	mat[1].x *= a; mat[1].y *= a; mat[1].z *= a; 
	mat[2].x *= a; mat[2].y *= a; mat[2].z *= a;

    return *this;
}

X_INLINE xMat3 &xMat3::operator*=(const xMat3 &a) {
	int i, j;
	float *m2Ptr;
	float *m1Ptr, dst[3];

	m1Ptr = (float*)this;
	m2Ptr = (float*)&a;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			dst[j]  = m1Ptr[0] * m2Ptr[ 0 * 3 + j ]
					+ m1Ptr[1] * m2Ptr[ 1 * 3 + j ]
					+ m1Ptr[2] * m2Ptr[ 2 * 3 + j ];
		}
		m1Ptr[0] = dst[0]; m1Ptr[1] = dst[1]; m1Ptr[2] = dst[2];
		m1Ptr += 3;
	}
	return *this;
}

X_INLINE xMat3 &xMat3::operator+=(const xMat3 &a) {
	mat[0].x += a[0].x; mat[0].y += a[0].y; mat[0].z += a[0].z;
	mat[1].x += a[1].x; mat[1].y += a[1].y; mat[1].z += a[1].z;
	mat[2].x += a[2].x; mat[2].y += a[2].y; mat[2].z += a[2].z;

    return *this;
}

X_INLINE xMat3 &xMat3::operator-=(const xMat3 &a) {
	mat[0].x -= a[0].x; mat[0].y -= a[0].y; mat[0].z -= a[0].z;
	mat[1].x -= a[1].x; mat[1].y -= a[1].y; mat[1].z -= a[1].z;
	mat[2].x -= a[2].x; mat[2].y -= a[2].y; mat[2].z -= a[2].z;

    return *this;
}

X_INLINE xVec3 operator*(const xVec3 &vec, const xMat3 &mat) {
	return mat * vec;
}

X_INLINE xVec3d operator*(const xVec3d &vec, const xMat3 &mat) {
	return mat * vec;
}

X_INLINE xMat3 operator*(float a, const xMat3 &mat) {
	return mat * a;
}

X_INLINE xVec3 &operator*=(xVec3 &vec, const xMat3 &mat) {
	float x = mat[ 0 ].x * vec.x + mat[ 1 ].x * vec.y + mat[ 2 ].x * vec.z;
	float y = mat[ 0 ].y * vec.x + mat[ 1 ].y * vec.y + mat[ 2 ].y * vec.z;
	vec.z = mat[ 0 ].z * vec.x + mat[ 1 ].z * vec.y + mat[ 2 ].z * vec.z;
	vec.x = x;
	vec.y = y;
	return vec;
}

X_INLINE xVec3d &operator*=(xVec3d &vec, const xMat3 &mat) {
	double x = mat[ 0 ].x * vec.x + mat[ 1 ].x * vec.y + mat[ 2 ].x * vec.z;
	double y = mat[ 0 ].y * vec.x + mat[ 1 ].y * vec.y + mat[ 2 ].y * vec.z;
	vec.z = mat[ 0 ].z * vec.x + mat[ 1 ].z * vec.y + mat[ 2 ].z * vec.z;
	vec.x = x;
	vec.y = y;
	return vec;
}

X_INLINE bool xMat3::Compare(const xMat3 &a) const {
	if (mat[0].Compare(a[0]) &&
		mat[1].Compare(a[1]) &&
		mat[2].Compare(a[2])) {
		return true;
	}
	return false;
}

X_INLINE bool xMat3::Compare(const xMat3 &a, float epsilon) const {
	if (mat[0].Compare(a[0], epsilon) &&
		mat[1].Compare(a[1], epsilon) &&
		mat[2].Compare(a[2], epsilon)) {
		return true;
	}
	return false;
}

X_INLINE bool xMat3::operator==(const xMat3 &a) const {
	return Compare(a);
}

X_INLINE bool xMat3::operator!=(const xMat3 &a) const {
	return !Compare(a);
}

X_INLINE void xMat3::Zero() {
	memset(mat, 0, sizeof(xMat3));
}

X_INLINE void xMat3::Identity() {
	*this = mat3_identity;
}

X_INLINE bool xMat3::IsIdentity(float epsilon) const {
	return Compare(mat3_identity, epsilon);
}

X_INLINE bool xMat3::IsSymmetric(float epsilon) const {
	if (xMath::Fabs(mat[0][1] - mat[1][0]) > epsilon) {
		return false;
	}
	if (xMath::Fabs(mat[0][2] - mat[2][0]) > epsilon) {
		return false;
	}
	if (xMath::Fabs(mat[1][2] - mat[2][1]) > epsilon) {
		return false;
	}
	return true;
}

X_INLINE bool xMat3::IsDiagonal(float epsilon) const {
	if (xMath::Fabs(mat[0][1]) > epsilon ||
		xMath::Fabs(mat[0][2]) > epsilon ||
		xMath::Fabs(mat[1][0]) > epsilon ||
		xMath::Fabs(mat[1][2]) > epsilon ||
		xMath::Fabs(mat[2][0]) > epsilon ||
		xMath::Fabs(mat[2][1]) > epsilon) {
		return false;
	}
	return true;
}

X_INLINE bool xMat3::IsRotated() const {
	return !Compare(mat3_identity);
}

X_INLINE xVec3 xMat3::ProjectVector(const xVec3 &src) const 
{
  return xVec3(src * mat[ 0 ], src * mat[ 1 ], src * mat[ 2 ]);
}

X_INLINE xVec3 xMat3::UnprojectVector(const xVec3 &src) const 
{
	return mat[ 0 ] * src.x + mat[ 1 ] * src.y + mat[ 2 ] * src.z;
}

X_INLINE xVec3 xMat3::Rotate(const xVec3 &src) const
{
  return xVec3(
	    src.x * mat[0].x + src.y * mat[1].x + src.z * mat[2].x,
	    src.x * mat[0].y + src.y * mat[1].y + src.z * mat[2].y,
	    src.x * mat[0].z + src.y * mat[1].z + src.z * mat[2].z);
}

X_INLINE xVec3 xMat3::InverseRotate(const xVec3 &src) const 
{
  return xVec3(
			mat[ 0 ].x * src.x + mat[ 0 ].y * src.y + mat[ 0 ].z * src.z,
			mat[ 1 ].x * src.x + mat[ 1 ].y * src.y + mat[ 1 ].z * src.z,
			mat[ 2 ].x * src.x + mat[ 2 ].y * src.y + mat[ 2 ].z * src.z);
}

X_INLINE bool xMat3::FixDegeneracies() {
	bool r = mat[0].FixDegenerateNormal();
	r |= mat[1].FixDegenerateNormal();
	r |= mat[2].FixDegenerateNormal();
	return r;
}

X_INLINE bool xMat3::FixDenormals() {
	bool r = mat[0].FixDenormals();
	r |= mat[1].FixDenormals();
	r |= mat[2].FixDenormals();
	return r;
}

X_INLINE float xMat3::Trace() const {
	return (mat[0][0] + mat[1][1] + mat[2][2]);
}

X_INLINE xMat3 xMat3::OrthoNormalize() const {
	xMat3 ortho;

	ortho = *this;
	ortho[ 0 ].Normalize();
	ortho[ 2 ].Cross(mat[ 0 ], mat[ 1 ]);
	ortho[ 2 ].Normalize();
	ortho[ 1 ].Cross(mat[ 2 ], mat[ 0 ]);
	ortho[ 1 ].Normalize();
	return ortho;
}

X_INLINE xMat3 &xMat3::OrthoNormalizeSelf() {
	mat[ 0 ].Normalize();
	mat[ 2 ].Cross(mat[ 0 ], mat[ 1 ]);
	mat[ 2 ].Normalize();
	mat[ 1 ].Cross(mat[ 2 ], mat[ 0 ]);
	mat[ 1 ].Normalize();
	return *this;
}

X_INLINE xMat3 xMat3::Transpose() const {
	return xMat3(	mat[0][0], mat[1][0], mat[2][0],
					mat[0][1], mat[1][1], mat[2][1],
					mat[0][2], mat[1][2], mat[2][2]);
}

X_INLINE xMat3 &xMat3::TransposeSelf() {
	float tmp0, tmp1, tmp2;

	tmp0 = mat[0][1];
	mat[0][1] = mat[1][0];
	mat[1][0] = tmp0;
	tmp1 = mat[0][2];
	mat[0][2] = mat[2][0];
	mat[2][0] = tmp1;
	tmp2 = mat[1][2];
	mat[1][2] = mat[2][1];
	mat[2][1] = tmp2;

	return *this;
}

X_INLINE xMat3 xMat3::Inverse() const {
	xMat3 invMat;

	invMat = *this;
	int r = invMat.InverseSelf();
	assert(r);
	return invMat;
}

X_INLINE xMat3 xMat3::InverseFast() const {
	xMat3 invMat;

	invMat = *this;
	int r = invMat.InverseFastSelf();
	assert(r);
	return invMat;
}

X_INLINE xMat3 xMat3::TransposeMultiply(const xMat3 &b) const {
	return xMat3(	mat[0].x * b[0].x + mat[1].x * b[1].x + mat[2].x * b[2].x,
					mat[0].x * b[0].y + mat[1].x * b[1].y + mat[2].x * b[2].y,
					mat[0].x * b[0].z + mat[1].x * b[1].z + mat[2].x * b[2].z,
					mat[0].y * b[0].x + mat[1].y * b[1].x + mat[2].y * b[2].x,
					mat[0].y * b[0].y + mat[1].y * b[1].y + mat[2].y * b[2].y,
					mat[0].y * b[0].z + mat[1].y * b[1].z + mat[2].y * b[2].z,
					mat[0].z * b[0].x + mat[1].z * b[1].x + mat[2].z * b[2].x,
					mat[0].z * b[0].y + mat[1].z * b[1].y + mat[2].z * b[2].y,
					mat[0].z * b[0].z + mat[1].z * b[1].z + mat[2].z * b[2].z);
}

X_INLINE void TransposeMultiply(const xMat3 &transpose, const xMat3 &b, xMat3 &dst) {
	dst[0].x = transpose[0].x * b[0].x + transpose[1].x * b[1].x + transpose[2].x * b[2].x;
	dst[0].y = transpose[0].x * b[0].y + transpose[1].x * b[1].y + transpose[2].x * b[2].y;
	dst[0].z = transpose[0].x * b[0].z + transpose[1].x * b[1].z + transpose[2].x * b[2].z;
	dst[1].x = transpose[0].y * b[0].x + transpose[1].y * b[1].x + transpose[2].y * b[2].x;
	dst[1].y = transpose[0].y * b[0].y + transpose[1].y * b[1].y + transpose[2].y * b[2].y;
	dst[1].z = transpose[0].y * b[0].z + transpose[1].y * b[1].z + transpose[2].y * b[2].z;
	dst[2].x = transpose[0].z * b[0].x + transpose[1].z * b[1].x + transpose[2].z * b[2].x;
	dst[2].y = transpose[0].z * b[0].y + transpose[1].z * b[1].y + transpose[2].z * b[2].y;
	dst[2].z = transpose[0].z * b[0].z + transpose[1].z * b[1].z + transpose[2].z * b[2].z;
}

X_INLINE xMat3 SkewSymmetric(xVec3 const &src) {
	return xMat3(0.0f, -src.z,  src.y, src.z,   0.0f, -src.x, -src.y,  src.x,   0.0f);
}

X_INLINE int xMat3::Dimension() const {
	return 9;
}

X_INLINE const float *xMat3::ToFloatPtr() const {
	return mat[0].ToFloatPtr();
}

X_INLINE float *xMat3::ToFloatPtr() {
	return mat[0].ToFloatPtr();
}


//===============================================================
//
//	xMat4 - 4x4 matrix
//
//===============================================================

class xMat4
{
public:
					xMat4();
					explicit xMat4(const xVec4 &x, const xVec4 &y, const xVec4 &z, const xVec4 &w);
					explicit xMat4(float xx, float xy, float xz, float xw,
									float yx, float yy, float yz, float yw,
									float zx, float zy, float zz, float zw,
									float wx, float wy, float wz, float ww);
					explicit xMat4(const xMat3 &rotation, const xVec3 &translation);
					explicit xMat4(float src[ 4 ][ 4 ]);

	const xVec4 &	operator[](int index) const;
	xVec4 &		operator[](int index);
	xMat4			operator*(float a) const;
	xVec4			operator*(const xVec4 &vec) const;
	xVec3			operator*(const xVec3 &vec) const;
	xMat4			operator*(const xMat4 &a) const;
	xMat4			operator+(const xMat4 &a) const;
	xMat4			operator-(const xMat4 &a) const;
	xMat4 &		operator*=(float a);
	xMat4 &		operator*=(const xMat4 &a);
	xMat4 &		operator+=(const xMat4 &a);
	xMat4 &		operator-=(const xMat4 &a);

	friend xMat4	operator*(float a, const xMat4 &mat);
	friend xVec4	operator*(const xVec4 &vec, const xMat4 &mat);
	friend xVec3	operator*(const xVec3 &vec, const xMat4 &mat);
	friend xVec4 &	operator*=(xVec4 &vec, const xMat4 &mat);
	friend xVec3 &	operator*=(xVec3 &vec, const xMat4 &mat);

	bool			Compare(const xMat4 &a) const;						// exact compare, no epsilon
	bool			Compare(const xMat4 &a, float epsilon) const;	// compare with epsilon
	bool			operator==(const xMat4 &a) const;					// exact compare, no epsilon
	bool			operator!=(const xMat4 &a) const;					// exact compare, no epsilon

	void			Zero();
	void			Identity();
	bool			IsIdentity(float epsilon = MATRIX_EPSILON) const;
	bool			IsSymmetric(float epsilon = MATRIX_EPSILON) const;
	bool			IsDiagonal(float epsilon = MATRIX_EPSILON) const;
	bool			IsRotated() const;

	xVec4			ProjectVector(const xVec4 &src) const;
	xVec4			UnprojectVector(const xVec4 &src) const;

	xVec3     ProjectVector(const xVec3 &src) const;
	xVec3     UnprojectVector(const xVec3 &src) const;
  
  xVec3     TransformFast(const xVec3& src) const;
  xVec3     InverseTransformFast(const xVec3& src) const;

  xVec3     RotateFast(const xVec3& src) const;
  xVec3     InverseRotateFast(const xVec3& src) const;

	float			Trace() const;
	float			Determinant() const;
	xMat4			Transpose() const;	// returns transpose
	xMat4 &		TransposeSelf();
	xMat4			Inverse() const;		// returns the inverse (m * m.Inverse() = identity)
	bool			InverseSelf();		// returns false if determinant is zero
	xMat4			InverseFast() const;	// returns the inverse (m * m.Inverse() = identity)
	bool			InverseFastSelf();	// returns false if determinant is zero
	xMat4			TransposeMultiply(const xMat4 &b) const;

	int				Dimension() const;

	xMat4 &		SetVectors(const xVec3& forward, const xVec3& right, const xVec3& up, const xVec3& translation);
  void      ToVectors(xVec3 *forward, xVec3 *right = NULL, xVec3 *up = NULL, xVec3 *translation = NULL) const;

  xMat3			ToMat3() const;

	const float *	ToFloatPtr() const;
	float *			ToFloatPtr();
	xString ToString(int precision = 2) const;

private:
	xVec4			mat[ 4 ];
};

extern const xMat4 mat4_zero;
extern const xMat4 mat4_identity;
#define mat4_default	mat4_identity

X_INLINE xMat4::xMat4() {
}

X_INLINE xMat4::xMat4(const xVec4 &x, const xVec4 &y, const xVec4 &z, const xVec4 &w) {
	mat[ 0 ] = x;
	mat[ 1 ] = y;
	mat[ 2 ] = z;
	mat[ 3 ] = w;
}

X_INLINE xMat4::xMat4(float xx, float xy, float xz, float xw,
							float yx, float yy, float yz, float yw,
							float zx, float zy, float zz, float zw,
							float wx, float wy, float wz, float ww) {
	mat[0][0] = xx; mat[0][1] = xy; mat[0][2] = xz; mat[0][3] = xw;
	mat[1][0] = yx; mat[1][1] = yy; mat[1][2] = yz; mat[1][3] = yw;
	mat[2][0] = zx; mat[2][1] = zy; mat[2][2] = zz; mat[2][3] = zw;
	mat[3][0] = wx; mat[3][1] = wy; mat[3][2] = wz; mat[3][3] = ww;
}

X_INLINE xMat4::xMat4(const xMat3 &rotation, const xVec3 &translation) {
	// NOTE: xMat3 is transposed because it is column-major
	mat[ 0 ][ 0 ] = rotation[0][0];
	mat[ 0 ][ 1 ] = rotation[1][0];
	mat[ 0 ][ 2 ] = rotation[2][0];
	mat[ 0 ][ 3 ] = translation[0];
	mat[ 1 ][ 0 ] = rotation[0][1];
	mat[ 1 ][ 1 ] = rotation[1][1];
	mat[ 1 ][ 2 ] = rotation[2][1];
	mat[ 1 ][ 3 ] = translation[1];
	mat[ 2 ][ 0 ] = rotation[0][2];
	mat[ 2 ][ 1 ] = rotation[1][2];
	mat[ 2 ][ 2 ] = rotation[2][2];
	mat[ 2 ][ 3 ] = translation[2];
	mat[ 3 ][ 0 ] = 0.0f;
	mat[ 3 ][ 1 ] = 0.0f;
	mat[ 3 ][ 2 ] = 0.0f;
	mat[ 3 ][ 3 ] = 1.0f;
}

X_INLINE xMat4::xMat4(float src[ 4 ][ 4 ]) {
	memcpy(mat, src, 4 * 4 * sizeof(float));
}

X_INLINE const xVec4 &xMat4::operator[](int index) const {
	//assert((index >= 0) && (index < 4));
	return mat[ index ];
}

X_INLINE xVec4 &xMat4::operator[](int index) {
	//assert((index >= 0) && (index < 4));
	return mat[ index ];
}

X_INLINE xMat4 xMat4::operator*(float a) const {
	return xMat4(
		mat[0].x * a, mat[0].y * a, mat[0].z * a, mat[0].w * a,
		mat[1].x * a, mat[1].y * a, mat[1].z * a, mat[1].w * a,
		mat[2].x * a, mat[2].y * a, mat[2].z * a, mat[2].w * a,
		mat[3].x * a, mat[3].y * a, mat[3].z * a, mat[3].w * a);
}

X_INLINE xVec4 xMat4::operator*(const xVec4 &vec) const {
	return xVec4(
		mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y + mat[ 0 ].z * vec.z + mat[ 0 ].w * vec.w,
		mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y + mat[ 1 ].z * vec.z + mat[ 1 ].w * vec.w,
		mat[ 2 ].x * vec.x + mat[ 2 ].y * vec.y + mat[ 2 ].z * vec.z + mat[ 2 ].w * vec.w,
		mat[ 3 ].x * vec.x + mat[ 3 ].y * vec.y + mat[ 3 ].z * vec.z + mat[ 3 ].w * vec.w);
}

X_INLINE xVec3 xMat4::operator*(const xVec3 &vec) const {
	float s = mat[ 3 ].x * vec.x + mat[ 3 ].y * vec.y + mat[ 3 ].z * vec.z + mat[ 3 ].w;
	if (s == 0.0f) {
		return xVec3(0.0f, 0.0f, 0.0f);
	}
	if (s == 1.0f) {
		return xVec3(
			mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y + mat[ 0 ].z * vec.z + mat[ 0 ].w,
			mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y + mat[ 1 ].z * vec.z + mat[ 1 ].w,
			mat[ 2 ].x * vec.x + mat[ 2 ].y * vec.y + mat[ 2 ].z * vec.z + mat[ 2 ].w);
	}
	else {
		float invS = 1.0f / s;
		return xVec3(
			(mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y + mat[ 0 ].z * vec.z + mat[ 0 ].w) * invS,
			(mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y + mat[ 1 ].z * vec.z + mat[ 1 ].w) * invS,
			(mat[ 2 ].x * vec.x + mat[ 2 ].y * vec.y + mat[ 2 ].z * vec.z + mat[ 2 ].w) * invS);
	}
}

X_INLINE xMat4 xMat4::operator*(const xMat4 &a) const {
	int i, j;
	float *m1Ptr, *m2Ptr;
	float *dstPtr;
	xMat4 dst;

	m1Ptr = (float*)this;
	m2Ptr = (float*)&a;
	dstPtr = (float*)&dst;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			*dstPtr = m1Ptr[0] * m2Ptr[ 0 * 4 + j ]
					+ m1Ptr[1] * m2Ptr[ 1 * 4 + j ]
					+ m1Ptr[2] * m2Ptr[ 2 * 4 + j ]
					+ m1Ptr[3] * m2Ptr[ 3 * 4 + j ];
			dstPtr++;
		}
		m1Ptr += 4;
	}
	return dst;
}

X_INLINE xMat4 xMat4::operator+(const xMat4 &a) const {
	return xMat4(
		mat[0].x + a[0].x, mat[0].y + a[0].y, mat[0].z + a[0].z, mat[0].w + a[0].w,
		mat[1].x + a[1].x, mat[1].y + a[1].y, mat[1].z + a[1].z, mat[1].w + a[1].w,
		mat[2].x + a[2].x, mat[2].y + a[2].y, mat[2].z + a[2].z, mat[2].w + a[2].w,
		mat[3].x + a[3].x, mat[3].y + a[3].y, mat[3].z + a[3].z, mat[3].w + a[3].w);
}
    
X_INLINE xMat4 xMat4::operator-(const xMat4 &a) const {
	return xMat4(
		mat[0].x - a[0].x, mat[0].y - a[0].y, mat[0].z - a[0].z, mat[0].w - a[0].w,
		mat[1].x - a[1].x, mat[1].y - a[1].y, mat[1].z - a[1].z, mat[1].w - a[1].w,
		mat[2].x - a[2].x, mat[2].y - a[2].y, mat[2].z - a[2].z, mat[2].w - a[2].w,
		mat[3].x - a[3].x, mat[3].y - a[3].y, mat[3].z - a[3].z, mat[3].w - a[3].w);
}

X_INLINE xMat4 &xMat4::operator*=(float a) {
	mat[0].x *= a; mat[0].y *= a; mat[0].z *= a; mat[0].w *= a;
	mat[1].x *= a; mat[1].y *= a; mat[1].z *= a; mat[1].w *= a;
	mat[2].x *= a; mat[2].y *= a; mat[2].z *= a; mat[2].w *= a;
	mat[3].x *= a; mat[3].y *= a; mat[3].z *= a; mat[3].w *= a;
    return *this;
}

X_INLINE xMat4 &xMat4::operator*=(const xMat4 &a) {
	*this = (*this) * a;
	return *this;
}

X_INLINE xMat4 &xMat4::operator+=(const xMat4 &a) {
	mat[0].x += a[0].x; mat[0].y += a[0].y; mat[0].z += a[0].z; mat[0].w += a[0].w;
	mat[1].x += a[1].x; mat[1].y += a[1].y; mat[1].z += a[1].z; mat[1].w += a[1].w;
	mat[2].x += a[2].x; mat[2].y += a[2].y; mat[2].z += a[2].z; mat[2].w += a[2].w;
	mat[3].x += a[3].x; mat[3].y += a[3].y; mat[3].z += a[3].z; mat[3].w += a[3].w;
    return *this;
}

X_INLINE xMat4 &xMat4::operator-=(const xMat4 &a) {
	mat[0].x -= a[0].x; mat[0].y -= a[0].y; mat[0].z -= a[0].z; mat[0].w -= a[0].w;
	mat[1].x -= a[1].x; mat[1].y -= a[1].y; mat[1].z -= a[1].z; mat[1].w -= a[1].w;
	mat[2].x -= a[2].x; mat[2].y -= a[2].y; mat[2].z -= a[2].z; mat[2].w -= a[2].w;
	mat[3].x -= a[3].x; mat[3].y -= a[3].y; mat[3].z -= a[3].z; mat[3].w -= a[3].w;
    return *this;
}

X_INLINE xMat4 operator*(float a, const xMat4 &mat) {
	return mat * a;
}

X_INLINE xVec4 operator*(const xVec4 &vec, const xMat4 &mat) {
	return mat * vec;
}

X_INLINE xVec3 operator*(const xVec3 &vec, const xMat4 &mat) {
	return mat * vec;
}

X_INLINE xVec4 &operator*=(xVec4 &vec, const xMat4 &mat) {
	vec = mat * vec;
	return vec;
}

X_INLINE xVec3 &operator*=(xVec3 &vec, const xMat4 &mat) {
	vec = mat * vec;
	return vec;
}

X_INLINE bool xMat4::Compare(const xMat4 &a) const {
	dword i;
	float *ptr1, *ptr2;

	ptr1 = (float*)mat;
	ptr2 = (float*)a.mat;
	for (i = 0; i < 4*4; i++) {
		if (ptr1[i] != ptr2[i]) {
			return false;
		}
	}
	return true;
}

X_INLINE bool xMat4::Compare(const xMat4 &a, float epsilon) const {
	dword i;
	float *ptr1, *ptr2;

	ptr1 = (float*)mat;
	ptr2 = (float*)a.mat;
	for (i = 0; i < 4*4; i++) {
		if (xMath::Fabs(ptr1[i] - ptr2[i]) > epsilon) {
			return false;
		}
	}
	return true;
}

X_INLINE bool xMat4::operator==(const xMat4 &a) const {
	return Compare(a);
}

X_INLINE bool xMat4::operator!=(const xMat4 &a) const {
	return !Compare(a);
}

X_INLINE void xMat4::Zero() {
	memset(mat, 0, sizeof(xMat4));
}

X_INLINE void xMat4::Identity() {
	*this = mat4_identity;
}

X_INLINE bool xMat4::IsIdentity(float epsilon) const {
	return Compare(mat4_identity, epsilon);
}

X_INLINE bool xMat4::IsSymmetric(float epsilon) const {
	for (int i = 1; i < 4; i++) {
		for (int j = 0; j < i; j++) {
			if (xMath::Fabs(mat[i][j] - mat[j][i]) > epsilon) {
				return false;
			}
		}
	}
	return true;
}

X_INLINE bool xMat4::IsDiagonal(float epsilon) const {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i != j && xMath::Fabs(mat[i][j]) > epsilon) {
				return false;
			}
		}
	}
	return true;
}

X_INLINE bool xMat4::IsRotated() const {
	if (!mat[ 0 ][ 1 ] && !mat[ 0 ][ 2 ] &&
		!mat[ 1 ][ 0 ] && !mat[ 1 ][ 2 ] &&
		!mat[ 2 ][ 0 ] && !mat[ 2 ][ 1 ]) {
		return false;
	}
	return true;
}

X_INLINE xVec4 xMat4::ProjectVector(const xVec4 &src) const
{
  return xVec4(src * mat[ 0 ], src * mat[ 1 ], src * mat[ 2 ], src * mat[ 3 ]);
}

X_INLINE xVec4 xMat4::UnprojectVector(const xVec4 &src) const
{
	return mat[ 0 ] * src.x + mat[ 1 ] * src.y + mat[ 2 ] * src.z + mat[ 3 ] * src.w;
}

X_INLINE xVec3 xMat4::ProjectVector(const xVec3 &src) const
{
  xVec4 p = ProjectVector(xVec4(src.x, src.y, src.z, 1));
  float invW = 1.0f / p.w;
  return xVec3(p.x * invW, p.y * invW, p.z * invW);
}
X_INLINE xVec3 xMat4::UnprojectVector(const xVec3 &src) const
{
  xVec4 p = UnprojectVector(xVec4(src.x, src.y, src.z, 1));
  float invW = 1.0f / p.w;
  return xVec3(p.x * invW, p.y * invW, p.z * invW);
}

X_INLINE xVec3 xMat4::TransformFast(const xVec3& src) const
{
	assert(mat[ 3 ].x * src.x + mat[ 3 ].y * src.y + mat[ 3 ].z * src.z + mat[ 3 ].w == 1.0f);
  return xVec3(
			mat[ 0 ].x * src.x + mat[ 0 ].y * src.y + mat[ 0 ].z * src.z + mat[ 0 ].w,
			mat[ 1 ].x * src.x + mat[ 1 ].y * src.y + mat[ 1 ].z * src.z + mat[ 1 ].w,
			mat[ 2 ].x * src.x + mat[ 2 ].y * src.y + mat[ 2 ].z * src.z + mat[ 2 ].w);
}

X_INLINE xVec3 xMat4::InverseTransformFast(const xVec3& src) const
{
	assert(mat[ 3 ].x * src.x + mat[ 3 ].y * src.y + mat[ 3 ].z * src.z + mat[ 3 ].w == 1.0f);

  float t[3];
	t[0] = src.x - mat[0].w;
	t[1] = src.y - mat[1].w;
	t[2] = src.z - mat[2].w;

  return xVec3(
	    t[0] * mat[0].x + t[1] * mat[1].x + t[2] * mat[2].x,
	    t[0] * mat[0].y + t[1] * mat[1].y + t[2] * mat[2].y,
	    t[0] * mat[0].z + t[1] * mat[1].z + t[2] * mat[2].z);
}

X_INLINE xVec3 xMat4::RotateFast(const xVec3& src) const
{
	assert(mat[ 3 ].x * src.x + mat[ 3 ].y * src.y + mat[ 3 ].z * src.z + mat[ 3 ].w == 1.0f);
  return xVec3(
			mat[ 0 ].x * src.x + mat[ 0 ].y * src.y + mat[ 0 ].z * src.z,
			mat[ 1 ].x * src.x + mat[ 1 ].y * src.y + mat[ 1 ].z * src.z,
			mat[ 2 ].x * src.x + mat[ 2 ].y * src.y + mat[ 2 ].z * src.z);
}

X_INLINE xVec3 xMat4::InverseRotateFast(const xVec3& src) const
{
	assert(mat[ 3 ].x * src.x + mat[ 3 ].y * src.y + mat[ 3 ].z * src.z + mat[ 3 ].w == 1.0f);
  return xVec3(
	    src.x * mat[0].x + src.y * mat[1].x + src.z * mat[2].x,
	    src.x * mat[0].y + src.y * mat[1].y + src.z * mat[2].y,
	    src.x * mat[0].z + src.y * mat[1].z + src.z * mat[2].z);
}


X_INLINE float xMat4::Trace() const {
	return (mat[0][0] + mat[1][1] + mat[2][2] + mat[3][3]);
}

X_INLINE xMat4 xMat4::Inverse() const {
	xMat4 invMat;

	invMat = *this;
	int r = invMat.InverseSelf();
	assert(r);
	return invMat;
}

X_INLINE xMat4 xMat4::InverseFast() const {
	xMat4 invMat;

	invMat = *this;
	int r = invMat.InverseFastSelf();
	assert(r);
	return invMat;
}

X_INLINE xMat4 xMat3::ToMat4() const {
	// NOTE: xMat3 is transposed because it is column-major
	return xMat4(	mat[0][0],	mat[1][0],	mat[2][0],	0.0f,
					mat[0][1],	mat[1][1],	mat[2][1],	0.0f,
					mat[0][2],	mat[1][2],	mat[2][2],	0.0f,
					0.0f,		0.0f,		0.0f,		1.0f);
}

X_INLINE xMat3 xMat4::ToMat3() const {
	// NOTE: xMat3 is transposed because it is column-major
	return xMat3(	mat[0][0],	mat[1][0],	mat[2][0],
					mat[0][1],	mat[1][1],	mat[2][1],
					mat[0][2],	mat[1][2],	mat[2][2]);
}

X_INLINE int xMat4::Dimension() const {
	return 16;
}

X_INLINE const float *xMat4::ToFloatPtr() const {
	return mat[0].ToFloatPtr();
}

X_INLINE float *xMat4::ToFloatPtr() {
	return mat[0].ToFloatPtr();
}


//===============================================================
//
//	xMat5 - 5x5 matrix
//
//===============================================================

class xMat5 {
public:
					xMat5();
					explicit xMat5(const xVec5 &v0, const xVec5 &v1, const xVec5 &v2, const xVec5 &v3, const xVec5 &v4);
					explicit xMat5(float src[ 5 ][ 5 ]);

	const xVec5 &	operator[](int index) const;
	xVec5 &		operator[](int index);
	xMat5			operator*(float a) const;
	xVec5			operator*(const xVec5 &vec) const;
	xMat5			operator*(const xMat5 &a) const;
	xMat5			operator+(const xMat5 &a) const;
	xMat5			operator-(const xMat5 &a) const;
	xMat5 &		operator*=(float a);
	xMat5 &		operator*=(const xMat5 &a);
	xMat5 &		operator+=(const xMat5 &a);
	xMat5 &		operator-=(const xMat5 &a);

	friend xMat5	operator*(float a, const xMat5 &mat);
	friend xVec5	operator*(const xVec5 &vec, const xMat5 &mat);
	friend xVec5 &	operator*=(xVec5 &vec, const xMat5 &mat);

	bool			Compare(const xMat5 &a) const;						// exact compare, no epsilon
	bool			Compare(const xMat5 &a, float epsilon) const;	// compare with epsilon
	bool			operator==(const xMat5 &a) const;					// exact compare, no epsilon
	bool			operator!=(const xMat5 &a) const;					// exact compare, no epsilon

	void			Zero();
	void			Identity();
	bool			IsIdentity(float epsilon = MATRIX_EPSILON) const;
	bool			IsSymmetric(float epsilon = MATRIX_EPSILON) const;
	bool			IsDiagonal(float epsilon = MATRIX_EPSILON) const;

	float			Trace() const;
	float			Determinant() const;
	xMat5			Transpose() const;	// returns transpose
	xMat5 &		TransposeSelf();
	xMat5			Inverse() const;		// returns the inverse (m * m.Inverse() = identity)
	bool			InverseSelf();		// returns false if determinant is zero
	xMat5			InverseFast() const;	// returns the inverse (m * m.Inverse() = identity)
	bool			InverseFastSelf();	// returns false if determinant is zero

	int				Dimension() const;

	const float *	ToFloatPtr() const;
	float *			ToFloatPtr();
	xString ToString(int precision = 2) const;

private:
	xVec5			mat[ 5 ];
};

extern const xMat5 mat5_zero;
extern const xMat5 mat5_identity;
#define mat5_default	mat5_identity

X_INLINE xMat5::xMat5() {
}

X_INLINE xMat5::xMat5(float src[ 5 ][ 5 ]) {
	memcpy(mat, src, 5 * 5 * sizeof(float));
}

X_INLINE xMat5::xMat5(const xVec5 &v0, const xVec5 &v1, const xVec5 &v2, const xVec5 &v3, const xVec5 &v4) {
	mat[0] = v0;
	mat[1] = v1;
	mat[2] = v2;
	mat[3] = v3;
	mat[4] = v4;
}

X_INLINE const xVec5 &xMat5::operator[](int index) const {
	//assert((index >= 0) && (index < 5));
	return mat[ index ];
}

X_INLINE xVec5 &xMat5::operator[](int index) {
	//assert((index >= 0) && (index < 5));
	return mat[ index ];
}

X_INLINE xMat5 xMat5::operator*(const xMat5 &a) const {
	int i, j;
	float *m1Ptr, *m2Ptr;
	float *dstPtr;
	xMat5 dst;

	m1Ptr = (float*)this;
	m2Ptr = (float*)&a;
	dstPtr = (float*)&dst;

	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			*dstPtr = m1Ptr[0] * m2Ptr[ 0 * 5 + j ]
					+ m1Ptr[1] * m2Ptr[ 1 * 5 + j ]
					+ m1Ptr[2] * m2Ptr[ 2 * 5 + j ]
					+ m1Ptr[3] * m2Ptr[ 3 * 5 + j ]
					+ m1Ptr[4] * m2Ptr[ 4 * 5 + j ];
			dstPtr++;
		}
		m1Ptr += 5;
	}
	return dst;
}

X_INLINE xMat5 xMat5::operator*(float a) const {
	return xMat5(
		xVec5(mat[0][0] * a, mat[0][1] * a, mat[0][2] * a, mat[0][3] * a, mat[0][4] * a),
		xVec5(mat[1][0] * a, mat[1][1] * a, mat[1][2] * a, mat[1][3] * a, mat[1][4] * a),
		xVec5(mat[2][0] * a, mat[2][1] * a, mat[2][2] * a, mat[2][3] * a, mat[2][4] * a),
		xVec5(mat[3][0] * a, mat[3][1] * a, mat[3][2] * a, mat[3][3] * a, mat[3][4] * a),
		xVec5(mat[4][0] * a, mat[4][1] * a, mat[4][2] * a, mat[4][3] * a, mat[4][4] * a));
}

X_INLINE xVec5 xMat5::operator*(const xVec5 &vec) const {
	return xVec5(
		mat[0][0] * vec[0] + mat[0][1] * vec[1] + mat[0][2] * vec[2] + mat[0][3] * vec[3] + mat[0][4] * vec[4],
		mat[1][0] * vec[0] + mat[1][1] * vec[1] + mat[1][2] * vec[2] + mat[1][3] * vec[3] + mat[1][4] * vec[4],
		mat[2][0] * vec[0] + mat[2][1] * vec[1] + mat[2][2] * vec[2] + mat[2][3] * vec[3] + mat[2][4] * vec[4],
		mat[3][0] * vec[0] + mat[3][1] * vec[1] + mat[3][2] * vec[2] + mat[3][3] * vec[3] + mat[3][4] * vec[4],
		mat[4][0] * vec[0] + mat[4][1] * vec[1] + mat[4][2] * vec[2] + mat[4][3] * vec[3] + mat[4][4] * vec[4]);
}

X_INLINE xMat5 xMat5::operator+(const xMat5 &a) const {
	return xMat5(
		xVec5(mat[0][0] + a[0][0], mat[0][1] + a[0][1], mat[0][2] + a[0][2], mat[0][3] + a[0][3], mat[0][4] + a[0][4]),
		xVec5(mat[1][0] + a[1][0], mat[1][1] + a[1][1], mat[1][2] + a[1][2], mat[1][3] + a[1][3], mat[1][4] + a[1][4]),
		xVec5(mat[2][0] + a[2][0], mat[2][1] + a[2][1], mat[2][2] + a[2][2], mat[2][3] + a[2][3], mat[2][4] + a[2][4]),
		xVec5(mat[3][0] + a[3][0], mat[3][1] + a[3][1], mat[3][2] + a[3][2], mat[3][3] + a[3][3], mat[3][4] + a[3][4]),
		xVec5(mat[4][0] + a[4][0], mat[4][1] + a[4][1], mat[4][2] + a[4][2], mat[4][3] + a[4][3], mat[4][4] + a[4][4]));
}

X_INLINE xMat5 xMat5::operator-(const xMat5 &a) const {
	return xMat5(
		xVec5(mat[0][0] - a[0][0], mat[0][1] - a[0][1], mat[0][2] - a[0][2], mat[0][3] - a[0][3], mat[0][4] - a[0][4]),
		xVec5(mat[1][0] - a[1][0], mat[1][1] - a[1][1], mat[1][2] - a[1][2], mat[1][3] - a[1][3], mat[1][4] - a[1][4]),
		xVec5(mat[2][0] - a[2][0], mat[2][1] - a[2][1], mat[2][2] - a[2][2], mat[2][3] - a[2][3], mat[2][4] - a[2][4]),
		xVec5(mat[3][0] - a[3][0], mat[3][1] - a[3][1], mat[3][2] - a[3][2], mat[3][3] - a[3][3], mat[3][4] - a[3][4]),
		xVec5(mat[4][0] - a[4][0], mat[4][1] - a[4][1], mat[4][2] - a[4][2], mat[4][3] - a[4][3], mat[4][4] - a[4][4]));
}

X_INLINE xMat5 &xMat5::operator*=(float a) {
	mat[0][0] *= a; mat[0][1] *= a; mat[0][2] *= a; mat[0][3] *= a; mat[0][4] *= a;
	mat[1][0] *= a; mat[1][1] *= a; mat[1][2] *= a; mat[1][3] *= a; mat[1][4] *= a;
	mat[2][0] *= a; mat[2][1] *= a; mat[2][2] *= a; mat[2][3] *= a; mat[2][4] *= a;
	mat[3][0] *= a; mat[3][1] *= a; mat[3][2] *= a; mat[3][3] *= a; mat[3][4] *= a;
	mat[4][0] *= a; mat[4][1] *= a; mat[4][2] *= a; mat[4][3] *= a; mat[4][4] *= a;
	return *this;
}

X_INLINE xMat5 &xMat5::operator*=(const xMat5 &a) {
	*this = *this * a;
	return *this;
}

X_INLINE xMat5 &xMat5::operator+=(const xMat5 &a) {
	mat[0][0] += a[0][0]; mat[0][1] += a[0][1]; mat[0][2] += a[0][2]; mat[0][3] += a[0][3]; mat[0][4] += a[0][4];
	mat[1][0] += a[1][0]; mat[1][1] += a[1][1]; mat[1][2] += a[1][2]; mat[1][3] += a[1][3]; mat[1][4] += a[1][4];
	mat[2][0] += a[2][0]; mat[2][1] += a[2][1]; mat[2][2] += a[2][2]; mat[2][3] += a[2][3]; mat[2][4] += a[2][4];
	mat[3][0] += a[3][0]; mat[3][1] += a[3][1]; mat[3][2] += a[3][2]; mat[3][3] += a[3][3]; mat[3][4] += a[3][4];
	mat[4][0] += a[4][0]; mat[4][1] += a[4][1]; mat[4][2] += a[4][2]; mat[4][3] += a[4][3]; mat[4][4] += a[4][4];
	return *this;
}

X_INLINE xMat5 &xMat5::operator-=(const xMat5 &a) {
	mat[0][0] -= a[0][0]; mat[0][1] -= a[0][1]; mat[0][2] -= a[0][2]; mat[0][3] -= a[0][3]; mat[0][4] -= a[0][4];
	mat[1][0] -= a[1][0]; mat[1][1] -= a[1][1]; mat[1][2] -= a[1][2]; mat[1][3] -= a[1][3]; mat[1][4] -= a[1][4];
	mat[2][0] -= a[2][0]; mat[2][1] -= a[2][1]; mat[2][2] -= a[2][2]; mat[2][3] -= a[2][3]; mat[2][4] -= a[2][4];
	mat[3][0] -= a[3][0]; mat[3][1] -= a[3][1]; mat[3][2] -= a[3][2]; mat[3][3] -= a[3][3]; mat[3][4] -= a[3][4];
	mat[4][0] -= a[4][0]; mat[4][1] -= a[4][1]; mat[4][2] -= a[4][2]; mat[4][3] -= a[4][3]; mat[4][4] -= a[4][4];
	return *this;
}

X_INLINE xVec5 operator*(const xVec5 &vec, const xMat5 &mat) {
	return mat * vec;
}

X_INLINE xMat5 operator*(float a, xMat5 const &mat) {
	return mat * a;
}

X_INLINE xVec5 &operator*=(xVec5 &vec, const xMat5 &mat) {
	vec = mat * vec;
	return vec;
}

X_INLINE bool xMat5::Compare(const xMat5 &a) const {
	dword i;
	float *ptr1, *ptr2;

	ptr1 = (float*)mat;
	ptr2 = (float*)a.mat;
	for (i = 0; i < 5*5; i++) {
		if (ptr1[i] != ptr2[i]) {
			return false;
		}
	}
	return true;
}

X_INLINE bool xMat5::Compare(const xMat5 &a, float epsilon) const {
	dword i;
	float *ptr1, *ptr2;

	ptr1 = (float*)mat;
	ptr2 = (float*)a.mat;
	for (i = 0; i < 5*5; i++) {
		if (xMath::Fabs(ptr1[i] - ptr2[i]) > epsilon) {
			return false;
		}
	}
	return true;
}

X_INLINE bool xMat5::operator==(const xMat5 &a) const {
	return Compare(a);
}

X_INLINE bool xMat5::operator!=(const xMat5 &a) const {
	return !Compare(a);
}

X_INLINE void xMat5::Zero() {
	memset(mat, 0, sizeof(xMat5));
}

X_INLINE void xMat5::Identity() {
	*this = mat5_identity;
}

X_INLINE bool xMat5::IsIdentity(float epsilon) const {
	return Compare(mat5_identity, epsilon);
}

X_INLINE bool xMat5::IsSymmetric(float epsilon) const {
	for (int i = 1; i < 5; i++) {
		for (int j = 0; j < i; j++) {
			if (xMath::Fabs(mat[i][j] - mat[j][i]) > epsilon) {
				return false;
			}
		}
	}
	return true;
}

X_INLINE bool xMat5::IsDiagonal(float epsilon) const {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			if (i != j && xMath::Fabs(mat[i][j]) > epsilon) {
				return false;
			}
		}
	}
	return true;
}

X_INLINE float xMat5::Trace() const {
	return (mat[0][0] + mat[1][1] + mat[2][2] + mat[3][3] + mat[4][4]);
}

X_INLINE xMat5 xMat5::Inverse() const {
	xMat5 invMat;

	invMat = *this;
	int r = invMat.InverseSelf();
	assert(r);
	return invMat;
}

X_INLINE xMat5 xMat5::InverseFast() const {
	xMat5 invMat;

	invMat = *this;
	int r = invMat.InverseFastSelf();
	assert(r);
	return invMat;
}

X_INLINE int xMat5::Dimension() const {
	return 25;
}

X_INLINE const float *xMat5::ToFloatPtr() const {
	return mat[0].ToFloatPtr();
}

X_INLINE float *xMat5::ToFloatPtr() {
	return mat[0].ToFloatPtr();
}


//===============================================================
//
//	xMat6 - 6x6 matrix
//
//===============================================================

class xMat6 {
public:
					xMat6();
					explicit xMat6(const xVec6 &v0, const xVec6 &v1, const xVec6 &v2, const xVec6 &v3, const xVec6 &v4, const xVec6 &v5);
					explicit xMat6(const xMat3 &m0, const xMat3 &m1, const xMat3 &m2, const xMat3 &m3);
					explicit xMat6(float src[ 6 ][ 6 ]);

	const xVec6 &	operator[](int index) const;
	xVec6 &		operator[](int index);
	xMat6			operator*(float a) const;
	xVec6			operator*(const xVec6 &vec) const;
	xMat6			operator*(const xMat6 &a) const;
	xMat6			operator+(const xMat6 &a) const;
	xMat6			operator-(const xMat6 &a) const;
	xMat6 &		operator*=(float a);
	xMat6 &		operator*=(const xMat6 &a);
	xMat6 &		operator+=(const xMat6 &a);
	xMat6 &		operator-=(const xMat6 &a);

	friend xMat6	operator*(float a, const xMat6 &mat);
	friend xVec6	operator*(const xVec6 &vec, const xMat6 &mat);
	friend xVec6 &	operator*=(xVec6 &vec, const xMat6 &mat);

	bool			Compare(const xMat6 &a) const;						// exact compare, no epsilon
	bool			Compare(const xMat6 &a, float epsilon) const;	// compare with epsilon
	bool			operator==(const xMat6 &a) const;					// exact compare, no epsilon
	bool			operator!=(const xMat6 &a) const;					// exact compare, no epsilon

	void			Zero();
	void			Identity();
	bool			IsIdentity(float epsilon = MATRIX_EPSILON) const;
	bool			IsSymmetric(float epsilon = MATRIX_EPSILON) const;
	bool			IsDiagonal(float epsilon = MATRIX_EPSILON) const;

	xMat3			SubMat3(int n) const;
	float			Trace() const;
	float			Determinant() const;
	xMat6			Transpose() const;	// returns transpose
	xMat6 &		TransposeSelf();
	xMat6			Inverse() const;		// returns the inverse (m * m.Inverse() = identity)
	bool			InverseSelf();		// returns false if determinant is zero
	xMat6			InverseFast() const;	// returns the inverse (m * m.Inverse() = identity)
	bool			InverseFastSelf();	// returns false if determinant is zero

	int				Dimension() const;

	const float *	ToFloatPtr() const;
	float *			ToFloatPtr();
	xString ToString(int precision = 2) const;

private:
	xVec6			mat[ 6 ];
};

extern const xMat6 mat6_zero;
extern const xMat6 mat6_identity;
#define mat6_default	mat6_identity

X_INLINE xMat6::xMat6() {
}

X_INLINE xMat6::xMat6(const xMat3 &m0, const xMat3 &m1, const xMat3 &m2, const xMat3 &m3) {
	mat[0] = xVec6(m0[0][0], m0[0][1], m0[0][2], m1[0][0], m1[0][1], m1[0][2]);
	mat[1] = xVec6(m0[1][0], m0[1][1], m0[1][2], m1[1][0], m1[1][1], m1[1][2]);
	mat[2] = xVec6(m0[2][0], m0[2][1], m0[2][2], m1[2][0], m1[2][1], m1[2][2]);
	mat[3] = xVec6(m2[0][0], m2[0][1], m2[0][2], m3[0][0], m3[0][1], m3[0][2]);
	mat[4] = xVec6(m2[1][0], m2[1][1], m2[1][2], m3[1][0], m3[1][1], m3[1][2]);
	mat[5] = xVec6(m2[2][0], m2[2][1], m2[2][2], m3[2][0], m3[2][1], m3[2][2]);
}

X_INLINE xMat6::xMat6(const xVec6 &v0, const xVec6 &v1, const xVec6 &v2, const xVec6 &v3, const xVec6 &v4, const xVec6 &v5) {
	mat[0] = v0;
	mat[1] = v1;
	mat[2] = v2;
	mat[3] = v3;
	mat[4] = v4;
	mat[5] = v5;
}

X_INLINE xMat6::xMat6(float src[ 6 ][ 6 ]) {
	memcpy(mat, src, 6 * 6 * sizeof(float));
}

X_INLINE const xVec6 &xMat6::operator[](int index) const {
	//assert((index >= 0) && (index < 6));
	return mat[ index ];
}

X_INLINE xVec6 &xMat6::operator[](int index) {
	//assert((index >= 0) && (index < 6));
	return mat[ index ];
}

X_INLINE xMat6 xMat6::operator*(const xMat6 &a) const {
	int i, j;
	float *m1Ptr, *m2Ptr;
	float *dstPtr;
	xMat6 dst;

	m1Ptr = (float*)this;
	m2Ptr = (float*)&a;
	dstPtr = (float*)&dst;

	for (i = 0; i < 6; i++) {
		for (j = 0; j < 6; j++) {
			*dstPtr = m1Ptr[0] * m2Ptr[ 0 * 6 + j ]
					+ m1Ptr[1] * m2Ptr[ 1 * 6 + j ]
					+ m1Ptr[2] * m2Ptr[ 2 * 6 + j ]
					+ m1Ptr[3] * m2Ptr[ 3 * 6 + j ]
					+ m1Ptr[4] * m2Ptr[ 4 * 6 + j ]
					+ m1Ptr[5] * m2Ptr[ 5 * 6 + j ];
			dstPtr++;
		}
		m1Ptr += 6;
	}
	return dst;
}

X_INLINE xMat6 xMat6::operator*(float a) const {
	return xMat6(
		xVec6(mat[0][0] * a, mat[0][1] * a, mat[0][2] * a, mat[0][3] * a, mat[0][4] * a, mat[0][5] * a),
		xVec6(mat[1][0] * a, mat[1][1] * a, mat[1][2] * a, mat[1][3] * a, mat[1][4] * a, mat[1][5] * a),
		xVec6(mat[2][0] * a, mat[2][1] * a, mat[2][2] * a, mat[2][3] * a, mat[2][4] * a, mat[2][5] * a),
		xVec6(mat[3][0] * a, mat[3][1] * a, mat[3][2] * a, mat[3][3] * a, mat[3][4] * a, mat[3][5] * a),
		xVec6(mat[4][0] * a, mat[4][1] * a, mat[4][2] * a, mat[4][3] * a, mat[4][4] * a, mat[4][5] * a),
		xVec6(mat[5][0] * a, mat[5][1] * a, mat[5][2] * a, mat[5][3] * a, mat[5][4] * a, mat[5][5] * a));
}

X_INLINE xVec6 xMat6::operator*(const xVec6 &vec) const {
	return xVec6(
		mat[0][0] * vec[0] + mat[0][1] * vec[1] + mat[0][2] * vec[2] + mat[0][3] * vec[3] + mat[0][4] * vec[4] + mat[0][5] * vec[5],
		mat[1][0] * vec[0] + mat[1][1] * vec[1] + mat[1][2] * vec[2] + mat[1][3] * vec[3] + mat[1][4] * vec[4] + mat[1][5] * vec[5],
		mat[2][0] * vec[0] + mat[2][1] * vec[1] + mat[2][2] * vec[2] + mat[2][3] * vec[3] + mat[2][4] * vec[4] + mat[2][5] * vec[5],
		mat[3][0] * vec[0] + mat[3][1] * vec[1] + mat[3][2] * vec[2] + mat[3][3] * vec[3] + mat[3][4] * vec[4] + mat[3][5] * vec[5],
		mat[4][0] * vec[0] + mat[4][1] * vec[1] + mat[4][2] * vec[2] + mat[4][3] * vec[3] + mat[4][4] * vec[4] + mat[4][5] * vec[5],
		mat[5][0] * vec[0] + mat[5][1] * vec[1] + mat[5][2] * vec[2] + mat[5][3] * vec[3] + mat[5][4] * vec[4] + mat[5][5] * vec[5]);
}

X_INLINE xMat6 xMat6::operator+(const xMat6 &a) const {
	return xMat6(
		xVec6(mat[0][0] + a[0][0], mat[0][1] + a[0][1], mat[0][2] + a[0][2], mat[0][3] + a[0][3], mat[0][4] + a[0][4], mat[0][5] + a[0][5]),
		xVec6(mat[1][0] + a[1][0], mat[1][1] + a[1][1], mat[1][2] + a[1][2], mat[1][3] + a[1][3], mat[1][4] + a[1][4], mat[1][5] + a[1][5]),
		xVec6(mat[2][0] + a[2][0], mat[2][1] + a[2][1], mat[2][2] + a[2][2], mat[2][3] + a[2][3], mat[2][4] + a[2][4], mat[2][5] + a[2][5]),
		xVec6(mat[3][0] + a[3][0], mat[3][1] + a[3][1], mat[3][2] + a[3][2], mat[3][3] + a[3][3], mat[3][4] + a[3][4], mat[3][5] + a[3][5]),
		xVec6(mat[4][0] + a[4][0], mat[4][1] + a[4][1], mat[4][2] + a[4][2], mat[4][3] + a[4][3], mat[4][4] + a[4][4], mat[4][5] + a[4][5]),
		xVec6(mat[5][0] + a[5][0], mat[5][1] + a[5][1], mat[5][2] + a[5][2], mat[5][3] + a[5][3], mat[5][4] + a[5][4], mat[5][5] + a[5][5]));
}

X_INLINE xMat6 xMat6::operator-(const xMat6 &a) const {
	return xMat6(
		xVec6(mat[0][0] - a[0][0], mat[0][1] - a[0][1], mat[0][2] - a[0][2], mat[0][3] - a[0][3], mat[0][4] - a[0][4], mat[0][5] - a[0][5]),
		xVec6(mat[1][0] - a[1][0], mat[1][1] - a[1][1], mat[1][2] - a[1][2], mat[1][3] - a[1][3], mat[1][4] - a[1][4], mat[1][5] - a[1][5]),
		xVec6(mat[2][0] - a[2][0], mat[2][1] - a[2][1], mat[2][2] - a[2][2], mat[2][3] - a[2][3], mat[2][4] - a[2][4], mat[2][5] - a[2][5]),
		xVec6(mat[3][0] - a[3][0], mat[3][1] - a[3][1], mat[3][2] - a[3][2], mat[3][3] - a[3][3], mat[3][4] - a[3][4], mat[3][5] - a[3][5]),
		xVec6(mat[4][0] - a[4][0], mat[4][1] - a[4][1], mat[4][2] - a[4][2], mat[4][3] - a[4][3], mat[4][4] - a[4][4], mat[4][5] - a[4][5]),
		xVec6(mat[5][0] - a[5][0], mat[5][1] - a[5][1], mat[5][2] - a[5][2], mat[5][3] - a[5][3], mat[5][4] - a[5][4], mat[5][5] - a[5][5]));
}

X_INLINE xMat6 &xMat6::operator*=(float a) {
	mat[0][0] *= a; mat[0][1] *= a; mat[0][2] *= a; mat[0][3] *= a; mat[0][4] *= a; mat[0][5] *= a;
	mat[1][0] *= a; mat[1][1] *= a; mat[1][2] *= a; mat[1][3] *= a; mat[1][4] *= a; mat[1][5] *= a;
	mat[2][0] *= a; mat[2][1] *= a; mat[2][2] *= a; mat[2][3] *= a; mat[2][4] *= a; mat[2][5] *= a;
	mat[3][0] *= a; mat[3][1] *= a; mat[3][2] *= a; mat[3][3] *= a; mat[3][4] *= a; mat[3][5] *= a;
	mat[4][0] *= a; mat[4][1] *= a; mat[4][2] *= a; mat[4][3] *= a; mat[4][4] *= a; mat[4][5] *= a;
	mat[5][0] *= a; mat[5][1] *= a; mat[5][2] *= a; mat[5][3] *= a; mat[5][4] *= a; mat[5][5] *= a;
	return *this;
}

X_INLINE xMat6 &xMat6::operator*=(const xMat6 &a) {
	*this = *this * a;
	return *this;
}

X_INLINE xMat6 &xMat6::operator+=(const xMat6 &a) {
	mat[0][0] += a[0][0]; mat[0][1] += a[0][1]; mat[0][2] += a[0][2]; mat[0][3] += a[0][3]; mat[0][4] += a[0][4]; mat[0][5] += a[0][5];
	mat[1][0] += a[1][0]; mat[1][1] += a[1][1]; mat[1][2] += a[1][2]; mat[1][3] += a[1][3]; mat[1][4] += a[1][4]; mat[1][5] += a[1][5];
	mat[2][0] += a[2][0]; mat[2][1] += a[2][1]; mat[2][2] += a[2][2]; mat[2][3] += a[2][3]; mat[2][4] += a[2][4]; mat[2][5] += a[2][5];
	mat[3][0] += a[3][0]; mat[3][1] += a[3][1]; mat[3][2] += a[3][2]; mat[3][3] += a[3][3]; mat[3][4] += a[3][4]; mat[3][5] += a[3][5];
	mat[4][0] += a[4][0]; mat[4][1] += a[4][1]; mat[4][2] += a[4][2]; mat[4][3] += a[4][3]; mat[4][4] += a[4][4]; mat[4][5] += a[4][5];
	mat[5][0] += a[5][0]; mat[5][1] += a[5][1]; mat[5][2] += a[5][2]; mat[5][3] += a[5][3]; mat[5][4] += a[5][4]; mat[5][5] += a[5][5];
	return *this;
}

X_INLINE xMat6 &xMat6::operator-=(const xMat6 &a) {
	mat[0][0] -= a[0][0]; mat[0][1] -= a[0][1]; mat[0][2] -= a[0][2]; mat[0][3] -= a[0][3]; mat[0][4] -= a[0][4]; mat[0][5] -= a[0][5];
	mat[1][0] -= a[1][0]; mat[1][1] -= a[1][1]; mat[1][2] -= a[1][2]; mat[1][3] -= a[1][3]; mat[1][4] -= a[1][4]; mat[1][5] -= a[1][5];
	mat[2][0] -= a[2][0]; mat[2][1] -= a[2][1]; mat[2][2] -= a[2][2]; mat[2][3] -= a[2][3]; mat[2][4] -= a[2][4]; mat[2][5] -= a[2][5];
	mat[3][0] -= a[3][0]; mat[3][1] -= a[3][1]; mat[3][2] -= a[3][2]; mat[3][3] -= a[3][3]; mat[3][4] -= a[3][4]; mat[3][5] -= a[3][5];
	mat[4][0] -= a[4][0]; mat[4][1] -= a[4][1]; mat[4][2] -= a[4][2]; mat[4][3] -= a[4][3]; mat[4][4] -= a[4][4]; mat[4][5] -= a[4][5];
	mat[5][0] -= a[5][0]; mat[5][1] -= a[5][1]; mat[5][2] -= a[5][2]; mat[5][3] -= a[5][3]; mat[5][4] -= a[5][4]; mat[5][5] -= a[5][5];
	return *this;
}

X_INLINE xVec6 operator*(const xVec6 &vec, const xMat6 &mat) {
	return mat * vec;
}

X_INLINE xMat6 operator*(float a, xMat6 const &mat) {
	return mat * a;
}

X_INLINE xVec6 &operator*=(xVec6 &vec, const xMat6 &mat) {
	vec = mat * vec;
	return vec;
}

X_INLINE bool xMat6::Compare(const xMat6 &a) const {
	dword i;
	float *ptr1, *ptr2;

	ptr1 = (float*)mat;
	ptr2 = (float*)a.mat;
	for (i = 0; i < 6*6; i++) {
		if (ptr1[i] != ptr2[i]) {
			return false;
		}
	}
	return true;
}

X_INLINE bool xMat6::Compare(const xMat6 &a, float epsilon) const {
	dword i;
	float *ptr1, *ptr2;

	ptr1 = (float*)mat;
	ptr2 = (float*)a.mat;
	for (i = 0; i < 6*6; i++) {
		if (xMath::Fabs(ptr1[i] - ptr2[i]) > epsilon) {
			return false;
		}
	}
	return true;
}

X_INLINE bool xMat6::operator==(const xMat6 &a) const {
	return Compare(a);
}

X_INLINE bool xMat6::operator!=(const xMat6 &a) const {
	return !Compare(a);
}

X_INLINE void xMat6::Zero() {
	memset(mat, 0, sizeof(xMat6));
}

X_INLINE void xMat6::Identity() {
	*this = mat6_identity;
}

X_INLINE bool xMat6::IsIdentity(float epsilon) const {
	return Compare(mat6_identity, epsilon);
}

X_INLINE bool xMat6::IsSymmetric(float epsilon) const {
	for (int i = 1; i < 6; i++) {
		for (int j = 0; j < i; j++) {
			if (xMath::Fabs(mat[i][j] - mat[j][i]) > epsilon) {
				return false;
			}
		}
	}
	return true;
}

X_INLINE bool xMat6::IsDiagonal(float epsilon) const {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			if (i != j && xMath::Fabs(mat[i][j]) > epsilon) {
				return false;
			}
		}
	}
	return true;
}

X_INLINE xMat3 xMat6::SubMat3(int n) const {
	assert(n >= 0 && n < 4);
	int b0 = ((n & 2) >> 1) * 3;
	int b1 = (n & 1) * 3;
	return xMat3(
		mat[b0 + 0][b1 + 0], mat[b0 + 0][b1 + 1], mat[b0 + 0][b1 + 2],
		mat[b0 + 1][b1 + 0], mat[b0 + 1][b1 + 1], mat[b0 + 1][b1 + 2],
		mat[b0 + 2][b1 + 0], mat[b0 + 2][b1 + 1], mat[b0 + 2][b1 + 2]);
}

X_INLINE float xMat6::Trace() const {
	return (mat[0][0] + mat[1][1] + mat[2][2] + mat[3][3] + mat[4][4] + mat[5][5]);
}

X_INLINE xMat6 xMat6::Inverse() const {
	xMat6 invMat;

	invMat = *this;
	int r = invMat.InverseSelf();
	assert(r);
	return invMat;
}

X_INLINE xMat6 xMat6::InverseFast() const {
	xMat6 invMat;

	invMat = *this;
	int r = invMat.InverseFastSelf();
	assert(r);
	return invMat;
}

X_INLINE int xMat6::Dimension() const {
	return 36;
}

X_INLINE const float *xMat6::ToFloatPtr() const {
	return mat[0].ToFloatPtr();
}

X_INLINE float *xMat6::ToFloatPtr() {
	return mat[0].ToFloatPtr();
}

#endif /* !__X_MATRIX_H__ */
