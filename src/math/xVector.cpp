#include <xForm.h>

const xVec2 vec2_origin(0.0f, 0.0f);
const xVec3 vec3_origin(0.0f, 0.0f, 0.0f);
const xVec3d vec3d_origin(0.0f, 0.0f, 0.0f);
const xVec4 vec4_origin(0.0f, 0.0f, 0.0f, 0.0f);
const xVec5 vec5_origin(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
const xVec6 vec6_origin(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
const xVec6 vec6_infinity(xMath::INFINITY, xMath::INFINITY, xMath::INFINITY, xMath::INFINITY, xMath::INFINITY, xMath::INFINITY);

const xVec3 vec3_forward; // (0.0f, 0.0f, 0.0f);
const xVec3 vec3_right; // (0.0f, 0.0f, 0.0f);
const xVec3 vec3_up; // (0.0f, 0.0f, 0.0f);

//===============================================================
//
//	xVec2
//
//===============================================================

/*
=============
xVec2::ToString
=============
*/
xString xVec2::ToString(int precision) const {
	return xString::FloatArrayToString(ToFloatPtr(), Dimension(), precision);
}

/*
=============
Lerp

Linearly inperpolates one vector to another.
=============
*/
void xVec2::Lerp(const xVec2 &v1, const xVec2 &v2, float l) {
	if (l <= 0.0f) {
		(*this) = v1;
	} else if (l >= 1.0f) {
		(*this) = v2;
	} else {
		(*this) = v1 + l * (v2 - v1);
	}
}


//===============================================================
//
//	xVec3
//
//===============================================================

/*
=============
xVec3::ToYaw
=============
*/
float xVec3::ToYaw() const {
	float yaw;
	
	if ((y == 0.0f) && (x == 0.0f)) {
		yaw = 0.0f;
	} else {
		yaw = RAD2DEG(atan2(y, x));
		if (yaw < 0.0f) {
			yaw += 360.0f;
		}
	}

	return yaw;
}

/*
=============
xVec3::ToPitch
=============
*/
float xVec3::ToPitch() const {
	float	forward;
	float	pitch;
	
	if ((x == 0.0f) && (y == 0.0f)) {
		if (z > 0.0f) {
			pitch = 90.0f;
		} else {
			pitch = 270.0f;
		}
	} else {
		forward = (float)xMath::Sqrt(x * x + y * y);
		pitch = RAD2DEG(atan2(z, forward));
		if (pitch < 0.0f) {
			pitch += 360.0f;
		}
	}

	return pitch;
}

/*
=============
xVec3::ToAngles
=============
*/
xAngles xVec3::ToAngles() const {
	float forward;
	float yaw;
	float pitch;
	
	if ((x == 0.0f) && (y == 0.0f)) {
		yaw = 0.0f;
		if (z > 0.0f) {
			pitch = 90.0f;
		} else {
			pitch = 270.0f;
		}
	} else {
		yaw = RAD2DEG(atan2(y, x));
		if (yaw < 0.0f) {
			yaw += 360.0f;
		}

		forward = (float)xMath::Sqrt(x * x + y * y);
		pitch = RAD2DEG(atan2(z, forward));
		if (pitch < 0.0f) {
			pitch += 360.0f;
		}
	}

	return xAngles(-pitch, yaw, 0.0f);
}

/*
=============
xVec3::ToPolar
=============
*/
xPolar3 xVec3::ToPolar() const {
	float forward;
	float yaw;
	float pitch;
	
	if ((x == 0.0f) && (y == 0.0f)) {
		yaw = 0.0f;
		if (z > 0.0f) {
			pitch = 90.0f;
		} else {
			pitch = 270.0f;
		}
	} else {
		yaw = RAD2DEG(atan2(y, x));
		if (yaw < 0.0f) {
			yaw += 360.0f;
		}

		forward = (float)xMath::Sqrt(x * x + y * y);
		pitch = RAD2DEG(atan2(z, forward));
		if (pitch < 0.0f) {
			pitch += 360.0f;
		}
	}
	return xPolar3(xMath::Sqrt(x * x + y * y + z * z), yaw, -pitch);
}

/*
=============
xVec3::ToMat3
=============
*/
xMat3 xVec3::ToMat3() const {
	xMat3	mat;
	float	d;

	mat[0] = *this;
	d = x * x + y * y;
	if (!d) {
		mat[1][0] = 1.0f;
		mat[1][1] = 0.0f;
		mat[1][2] = 0.0f;
	} else {
		d = xMath::InvSqrt(d);
		mat[1][0] = -y * d;
		mat[1][1] = x * d;
		mat[1][2] = 0.0f;
	}
	mat[2] = Cross(mat[1]);

	return mat;
}

xVec3d xVec3::ToVec3d() const
{
  return xVec3d(x,y,z);
}


/*
=============
xVec3::ToString
=============
*/
xString xVec3::ToString(int precision) const {
	return xString::FloatArrayToString(ToFloatPtr(), Dimension(), precision);
}

/*
=============
Lerp

Linearly inperpolates one vector to another.
=============
*/
void xVec3::Lerp(const xVec3 &v1, const xVec3 &v2, float l) {
	if (l <= 0.0f) {
		(*this) = v1;
	} else if (l >= 1.0f) {
		(*this) = v2;
	} else {
		(*this) = v1 + l * (v2 - v1);
	}
}

/*
=============
SLerp

Spherical linear interpolation from v1 to v2.
Vectors are expected to be normalized.
=============
*/
#define LERP_DELTA 1e-6

void xVec3::SLerp(const xVec3 &v1, const xVec3 &v2, float t) {
	float omega, cosom, sinom, scale0, scale1;

	if (t <= 0.0f) {
		(*this) = v1;
		return;
	} else if (t >= 1.0f) {
		(*this) = v2;
		return;
	}

	cosom = v1 * v2;
	if ((1.0f - cosom) > LERP_DELTA) {
		omega = acos(cosom);
		sinom = sin(omega);
		scale0 = sin((1.0f - t) * omega) / sinom;
		scale1 = sin(t * omega) / sinom;
	} else {
		scale0 = 1.0f - t;
		scale1 = t;
	}

	(*this) = (v1 * scale0 + v2 * scale1);
}

/*
=============
ProjectSelfOntoSphere

Projects the z component onto a sphere.
=============
*/
void xVec3::ProjectSelfOntoSphere(float radius) {
	float rsqr = radius * radius;
	float len = Length();
	if (len  < rsqr * 0.5f) {
		z = sqrt(rsqr - len);
	} else {
		z = rsqr / (2.0f * sqrt(len));
	}
}

//===============================================================
//
//	xVec3d
//
//===============================================================

/*
=============
xVec3d::ToYaw
=============
*/
float xVec3d::ToYaw() const {
	double yaw;
	
	if ((y == 0.0f) && (x == 0.0f)) {
		yaw = 0.0f;
	} else {
		yaw = RAD2DEG(atan2(y, x));
		if (yaw < 0.0f) {
			yaw += 360.0f;
		}
	}

	return yaw;
}

/*
=============
xVec3d::ToPitch
=============
*/
float xVec3d::ToPitch() const {
	double	forward;
	double	pitch;
	
	if ((x == 0.0f) && (y == 0.0f)) {
		if (z > 0.0f) {
			pitch = 90.0f;
		} else {
			pitch = 270.0f;
		}
	} else {
		forward = sqrt(x * x + y * y);
		pitch = RAD2DEG(atan2(z, forward));
		if (pitch < 0.0f) {
			pitch += 360.0f;
		}
	}

	return pitch;
}

/*
=============
xVec3d::ToAngles
=============
*/
xAngles xVec3d::ToAngles() const {
	double forward;
	double yaw;
	double pitch;
	
	if ((x == 0.0f) && (y == 0.0f)) {
		yaw = 0.0f;
		if (z > 0.0f) {
			pitch = 90.0f;
		} else {
			pitch = 270.0f;
		}
	} else {
		yaw = RAD2DEG(atan2(y, x));
		if (yaw < 0.0f) {
			yaw += 360.0f;
		}

		forward = sqrt(x * x + y * y);
		pitch = RAD2DEG(atan2(z, forward));
		if (pitch < 0.0f) {
			pitch += 360.0f;
		}
	}

	return xAngles(-pitch, yaw, 0.0f);
}

/*
=============
xVec3d::ToPolar
=============
*/
xPolar3 xVec3d::ToPolar() const {
	double forward;
	double yaw;
	double pitch;
	
	if ((x == 0.0f) && (y == 0.0f)) {
		yaw = 0.0f;
		if (z > 0.0f) {
			pitch = 90.0f;
		} else {
			pitch = 270.0f;
		}
	} else {
		yaw = RAD2DEG(atan2(y, x));
		if (yaw < 0.0f) {
			yaw += 360.0f;
		}

		forward = sqrt(x * x + y * y);
		pitch = RAD2DEG(atan2(z, forward));
		if (pitch < 0.0f) {
			pitch += 360.0f;
		}
	}
	return xPolar3(Length(), yaw, -pitch);
}

/*
=============
xVec3d::ToMat3
=============
*/
xMat3 xVec3d::ToMat3() const {
	xMat3	mat;
	double	d;

	mat[0] = ToVec3();
	d = x * x + y * y;
	if (!d) {
		mat[1][0] = 1.0f;
		mat[1][1] = 0.0f;
		mat[1][2] = 0.0f;
	} else {
		d = 1.0 / sqrt(d);
		mat[1][0] = -y * d;
		mat[1][1] = x * d;
		mat[1][2] = 0.0f;
	}
	mat[2] = mat[0].Cross(mat[1]);

	return mat;
}

/*
=============
Lerp

Linearly inperpolates one vector to another.
=============
*/
void xVec3d::Lerp(const xVec3d &v1, const xVec3d &v2, double l) {
	if (l <= 0.0f) {
		(*this) = v1;
	} else if (l >= 1.0f) {
		(*this) = v2;
	} else {
		(*this) = v1 + l * (v2 - v1);
	}
}

/*
=============
SLerp

Spherical linear interpolation from v1 to v2.
Vectors are expected to be normalized.
=============
*/
#define LERP_DELTA 1e-6

void xVec3d::SLerp(const xVec3d &v1, const xVec3d &v2, double t) {
	double omega, cosom, sinom, scale0, scale1;

	if (t <= 0.0f) {
		(*this) = v1;
		return;
	} else if (t >= 1.0f) {
		(*this) = v2;
		return;
	}

	cosom = v1 * v2;
	if ((1.0f - cosom) > LERP_DELTA) {
		omega = acos(cosom);
		sinom = sin(omega);
		scale0 = sin((1.0f - t) * omega) / sinom;
		scale1 = sin(t * omega) / sinom;
	} else {
		scale0 = 1.0f - t;
		scale1 = t;
	}

	(*this) = (v1 * scale0 + v2 * scale1);
}

/*
=============
ProjectSelfOntoSphere

Projects the z component onto a sphere.
=============
*/
void xVec3d::ProjectSelfOntoSphere(double radius) {
	double rsqr = radius * radius;
	double len = Length();
	if (len  < rsqr * 0.5f) {
		z = sqrt(rsqr - len);
	} else {
		z = rsqr / (2.0f * sqrt(len));
	}
}

//===============================================================
//
//	xVec4
//
//===============================================================

/*
=============
xVec4::ToString
=============
*/
xString xVec4::ToString(int precision) const {
	return xString::FloatArrayToString(ToFloatPtr(), Dimension(), precision);
}

/*
=============
Lerp

Linearly inperpolates one vector to another.
=============
*/
void xVec4::Lerp(const xVec4 &v1, const xVec4 &v2, float l) {
	if (l <= 0.0f) {
		(*this) = v1;
	} else if (l >= 1.0f) {
		(*this) = v2;
	} else {
		(*this) = v1 + l * (v2 - v1);
	}
}


//===============================================================
//
//	xVec5
//
//===============================================================

/*
=============
xVec5::ToString
=============
*/
xString xVec5::ToString(int precision) const {
	return xString::FloatArrayToString(ToFloatPtr(), Dimension(), precision);
}

/*
=============
xVec5::Lerp
=============
*/
void xVec5::Lerp(const xVec5 &v1, const xVec5 &v2, float l) {
	if (l <= 0.0f) {
		(*this) = v1;
	} else if (l >= 1.0f) {
		(*this) = v2;
	} else {
		x = v1.x + l * (v2.x - v1.x);
		y = v1.y + l * (v2.y - v1.y);
		z = v1.z + l * (v2.z - v1.z);
		s = v1.s + l * (v2.s - v1.s);
		t = v1.t + l * (v2.t - v1.t);
	}
}


//===============================================================
//
//	xVec6
//
//===============================================================

/*
=============
xVec6::ToString
=============
*/
xString xVec6::ToString(int precision) const {
	return xString::FloatArrayToString(ToFloatPtr(), Dimension(), precision);
}
