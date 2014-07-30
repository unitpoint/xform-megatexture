#include <xForm.h>

/*
=====================
xQuat::ToAngles
=====================
*/
xAngles xQuat::ToAngles() const {
	return ToMat3().ToAngles();
}

/*
=====================
xQuat::ToRotation
=====================
*/
xRotation xQuat::ToRotation() const {
	xVec3 vec;
	float angle;

	vec.x = x;
	vec.y = y;
	vec.z = z;
	angle = xMath::ACos(w);
	if (angle == 0.0f) {
		vec.Set(0.0f, 0.0f, 1.0f);
	} else {
		//vec *= (1.0f / sin(angle));
		vec.Normalize();
		vec.FixDegenerateNormal();
		angle *= 2.0f * xMath::M_RAD2DEG;
	}
	return xRotation(vec3_origin, vec, angle);
}

/*
=====================
xQuat::ToMat3
=====================
*/
xMat3 xQuat::ToMat3() const {
	xMat3	mat;
	float	wx, wy, wz;
	float	xx, yy, yz;
	float	xy, xz, zz;
	float	x2, y2, z2;

	x2 = x + x;
	y2 = y + y;
	z2 = z + z;

	xx = x * x2;
	xy = x * y2;
	xz = x * z2;

	yy = y * y2;
	yz = y * z2;
	zz = z * z2;

	wx = w * x2;
	wy = w * y2;
	wz = w * z2;

	mat[ 0 ][ 0 ] = 1.0f - (yy + zz);
	mat[ 0 ][ 1 ] = xy - wz;
	mat[ 0 ][ 2 ] = xz + wy;

	mat[ 1 ][ 0 ] = xy + wz;
	mat[ 1 ][ 1 ] = 1.0f - (xx + zz);
	mat[ 1 ][ 2 ] = yz - wx;

	mat[ 2 ][ 0 ] = xz - wy;
	mat[ 2 ][ 1 ] = yz + wx;
	mat[ 2 ][ 2 ] = 1.0f - (xx + yy);

	return mat;
}

/*
=====================
xQuat::ToMat4
=====================
*/
xMat4 xQuat::ToMat4() const {
	return ToMat3().ToMat4();
}

/*
=====================
xQuat::ToCQuat
=====================
*/
xCQuat xQuat::ToCQuat() const {
	if (w < 0.0f) {
		return xCQuat(-x, -y, -z);
	}
	return xCQuat(x, y, z);
}

/*
============
xQuat::ToAngularVelocity
============
*/
xVec3 xQuat::ToAngularVelocity() const {
	xVec3 vec;

	vec.x = x;
	vec.y = y;
	vec.z = z;
	vec.Normalize();
	return vec * xMath::ACos(w);
}

/*
=============
xQuat::ToString
=============
*/
xString xQuat::ToString(int precision) const {
	return xString::FloatArrayToString(ToFloatPtr(), Dimension(), precision);
}

/*
=====================
xQuat::SlerpTo

Spherical linear interpolation between two quaternions.
=====================
*/
xQuat xQuat::SlerpTo(const xQuat &to, float t) const
{
	xQuat	temp;
  const xQuat& from = *this;
	float	omega, cosom, sinom, scale0, scale1;

	if(t <= 0.0f)
		return from;

	if(t >= 1.0f || from == to)
		return to;

	cosom = from.x * to.x + from.y * to.y + from.z * to.z + from.w * to.w;
	if (cosom < 0.0f) {
		temp = -to;
		cosom = -cosom;
	} else {
		temp = to;
	}

	if ((1.0f - cosom) > 1e-6f) {
#if 0
		omega = acos(cosom);
		sinom = 1.0f / sin(omega);
		scale0 = sin((1.0f - t) * omega) * sinom;
		scale1 = sin(t * omega) * sinom;
#else
		scale0 = 1.0f - cosom * cosom;
		sinom = xMath::InvSqrt(scale0);
		omega = xMath::ATan16(scale0 * sinom, cosom);
		scale0 = xMath::Sin16((1.0f - t) * omega) * sinom;
		scale1 = xMath::Sin16(t * omega) * sinom;
#endif
	} else {
		scale0 = 1.0f - t;
		scale1 = t;
	}

	return (scale0 * from) + (scale1 * temp);
}

/*
=====================
xQuat::Slerp

Spherical linear interpolation between two quaternions.
=====================
*/
xQuat &xQuat::SlerpFrom(const xQuat &from, const xQuat &to, float t)
{
  *this = from.SlerpTo(to, t);
	return *this;
}

xQuat xQuat::Slerp(const xQuat &from, const xQuat &to, float t)
{
  return from.SlerpTo(to, t);
}

/*
=============
xCQuat::ToAngles
=============
*/
xAngles xCQuat::ToAngles() const {
	return ToQuat().ToAngles();
}

/*
=============
xCQuat::ToRotation
=============
*/
xRotation xCQuat::ToRotation() const {
	return ToQuat().ToRotation();
}

/*
=============
xCQuat::ToMat3
=============
*/
xMat3 xCQuat::ToMat3() const {
	return ToQuat().ToMat3();
}

/*
=============
xCQuat::ToMat4
=============
*/
xMat4 xCQuat::ToMat4() const {
	return ToQuat().ToMat4();
}

/*
=============
xCQuat::ToString
=============
*/
xString xCQuat::ToString(int precision) const {
	return xString::FloatArrayToString(ToFloatPtr(), Dimension(), precision);
}
