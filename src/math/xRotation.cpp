#include <xForm.h>

/*
============
xRotation::ToAngles
============
*/
xAngles xRotation::ToAngles() const {
	return ToMat3().ToAngles();
}

/*
============
xRotation::ToQuat
============
*/
xQuat xRotation::ToQuat() const {
	float a, s, c;

	a = angle * (xMath::M_DEG2RAD * 0.5f);
	xMath::SinCos(a, s, c);
	return xQuat(vec.x * s, vec.y * s, vec.z * s, c);
}

/*
============
xRotation::toMat3
============
*/
const xMat3 &xRotation::ToMat3() const {
	float wx, wy, wz;
	float xx, yy, yz;
	float xy, xz, zz;
	float x2, y2, z2;
	float a, c, s, x, y, z;

	if (axisValid) {
		return axis;
	}

	a = angle * (xMath::M_DEG2RAD * 0.5f);
	xMath::SinCos(a, s, c);

	x = vec[0] * s;
	y = vec[1] * s;
	z = vec[2] * s;

	x2 = x + x;
	y2 = y + y;
	z2 = z + z;

	xx = x * x2;
	xy = x * y2;
	xz = x * z2;

	yy = y * y2;
	yz = y * z2;
	zz = z * z2;

	wx = c * x2;
	wy = c * y2;
	wz = c * z2;

	axis[ 0 ][ 0 ] = 1.0f - (yy + zz);
	axis[ 0 ][ 1 ] = xy - wz;
	axis[ 0 ][ 2 ] = xz + wy;

	axis[ 1 ][ 0 ] = xy + wz;
	axis[ 1 ][ 1 ] = 1.0f - (xx + zz);
	axis[ 1 ][ 2 ] = yz - wx;

	axis[ 2 ][ 0 ] = xz - wy;
	axis[ 2 ][ 1 ] = yz + wx;
	axis[ 2 ][ 2 ] = 1.0f - (xx + yy);

	axisValid = true;

	return axis;
}

/*
============
xRotation::ToMat4
============
*/
xMat4 xRotation::ToMat4() const {
	return ToMat3().ToMat4();
}

/*
============
xRotation::ToAngularVelocity
============
*/
xVec3 xRotation::ToAngularVelocity() const {
	return vec * DEG2RAD(angle);
}

/*
============
xRotation::Normalize180
============
*/
void xRotation::Normalize180() {
	angle -= floor(angle / 360.0f) * 360.0f;
	if (angle > 180.0f) {
		angle -= 360.0f;
	}
	else if (angle < -180.0f) {
		angle += 360.0f;
	}
}

/*
============
xRotation::Normalize360
============
*/
void xRotation::Normalize360() {
	angle -= floor(angle / 360.0f) * 360.0f;
	if (angle > 360.0f) {
		angle -= 360.0f;
	}
	else if (angle < 0.0f) {
		angle += 360.0f;
	}
}
