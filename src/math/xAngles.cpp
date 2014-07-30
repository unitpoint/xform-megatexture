#include <xForm.h>

const xAngles ang_zero(0.0f, 0.0f, 0.0f);

xAngles::xAngles(const xVec3& forwardDir, const xVec3& upDir)
{
  xVec3 r = forwardDir.Cross(xMath::Fabs(forwardDir * upDir) < 0.999f ? upDir : vec3_forward).Norm(); //vec3_right);
  xVec3 u = r.Cross(forwardDir).Norm();
  xMat3 m;
  *this = m.SetVectors(forwardDir, r, u).ToAngles();
}

float xAngles::Norm360(float a)
{
  if(a >= 0.0f && a < 360.0f)
    return a;
  
  a -= floor(a / 360.0f) * 360.0f;
  if(a >= 360.0f)
    return a - 360.0f;

  if(a < 0.0f)
    return a + 360.0f;

  return a;
}

float xAngles::Norm180(float a)
{
  a = Norm360(a);
  return a > 180.0f ? a - 360.0f : a;
}

/*
=================
xAngles::Normalize360

returns angles normalized to the range [0 <= angle < 360]
=================
*/
xAngles& xAngles::Normalize360()
{
  pitch = Norm360(pitch);
  yaw = Norm360(yaw);
  roll = Norm360(roll);
	return *this;
}

/*
=================
xAngles::Normalize180

returns angles normalized to the range [-180 < angle <= 180]
=================
*/
xAngles& xAngles::Normalize180()
{
  pitch = Norm180(pitch);
  yaw = Norm180(yaw);
  roll = Norm180(roll);
	return *this;
}

xAngles xAngles::Norm360()
{
  return xAngles(Norm360(pitch), Norm360(yaw), Norm360(roll));
}

xAngles xAngles::Norm180()
{
  return xAngles(Norm180(pitch), Norm180(yaw), Norm180(roll));
}

/*
=================
xAngles::ToVectors
=================
*/
void xAngles::ToVectors(xVec3 *forward, xVec3 *right, xVec3 *up) const {
	float sr, sp, sy, cr, cp, cy;
	
	xMath::SinCos(DEG2RAD(yaw), sy, cy);
	xMath::SinCos(DEG2RAD(pitch), sp, cp);
	xMath::SinCos(DEG2RAD(roll), sr, cr);

	if (forward) {
		forward->Set(cp * cy, cp * sy, -sp);
	}

	if (right) {
		right->Set(-sr * sp * cy + cr * sy, -sr * sp * sy + -cr * cy, -sr * cp);
	}

	if (up) {
		up->Set(cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp);
	}
}

/*
=================
xAngles::ToTexMaping
=================
*/
void xAngles::ToTexMaping(xVec3& xVec, xVec3& yVec, xVec3& up) const
{
  ToVectors(&yVec, &xVec, &up);
  up.FixDegenerateNormal();
  yVec.FixDegenerateNormal();
  xVec.FixDegenerateNormal();
  yVec = -yVec;
}

/*
=================
xAngles::ToForward
=================
*/
xVec3 xAngles::ToForward() const {
	float sp, sy, cp, cy;
	
	xMath::SinCos(DEG2RAD(yaw), sy, cy);
	xMath::SinCos(DEG2RAD(pitch), sp, cp);

	return xVec3(cp * cy, cp * sy, -sp);
}

/*
=================
xAngles::ToQuat
=================
*/
xQuat xAngles::ToQuat() const {
	float sx, cx, sy, cy, sz, cz;
	float sxcy, cxcy, sxsy, cxsy;

	xMath::SinCos(DEG2RAD(yaw) * 0.5f, sz, cz);
	xMath::SinCos(DEG2RAD(pitch) * 0.5f, sy, cy);
	xMath::SinCos(DEG2RAD(roll) * 0.5f, sx, cx);

	sxcy = sx * cy;
	cxcy = cx * cy;
	sxsy = sx * sy;
	cxsy = cx * sy;

	return xQuat(cxsy*sz - sxcy*cz, -cxsy*cz - sxcy*sz, sxsy*cz - cxcy*sz, cxcy*cz + sxsy*sz);
}

/*
=================
xAngles::ToRotation
=================
*/
xRotation xAngles::ToRotation() const {
	xVec3 vec;
	float angle, w;
	float sx, cx, sy, cy, sz, cz;
	float sxcy, cxcy, sxsy, cxsy;

	if (pitch == 0.0f) {
		if (yaw == 0.0f) {
			return xRotation(vec3_origin, xVec3(-1.0f, 0.0f, 0.0f), roll);
		}
		if (roll == 0.0f) {
			return xRotation(vec3_origin, xVec3(0.0f, 0.0f, -1.0f), yaw);
		}
	} else if (yaw == 0.0f && roll == 0.0f) {
		return xRotation(vec3_origin, xVec3(0.0f, -1.0f, 0.0f), pitch);
	}

	xMath::SinCos(DEG2RAD(yaw) * 0.5f, sz, cz);
	xMath::SinCos(DEG2RAD(pitch) * 0.5f, sy, cy);
	xMath::SinCos(DEG2RAD(roll) * 0.5f, sx, cx);

	sxcy = sx * cy;
	cxcy = cx * cy;
	sxsy = sx * sy;
	cxsy = cx * sy;

	vec.x =  cxsy * sz - sxcy * cz;
	vec.y = -cxsy * cz - sxcy * sz;
	vec.z =  sxsy * cz - cxcy * sz;
	w =		 cxcy * cz + sxsy * sz;
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
=================
xAngles::ToMat3
=================
*/
xMat3 xAngles::ToMat3() const {
	xMat3 mat;
	float sr, sp, sy, cr, cp, cy;

	xMath::SinCos(DEG2RAD(yaw), sy, cy);
	xMath::SinCos(DEG2RAD(pitch), sp, cp);
	xMath::SinCos(DEG2RAD(roll), sr, cr);

	mat[ 0 ].Set(cp * cy, cp * sy, -sp);
	mat[ 1 ].Set(sr * sp * cy + cr * -sy, sr * sp * sy + cr * cy, sr * cp);
	mat[ 2 ].Set(cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp);

	return mat;
}

/*
=================
xAngles::ToMat4
=================
*/
xMat4 xAngles::ToMat4() const {
	return ToMat3().ToMat4();
}

/*
=================
xAngles::ToAngularVelocity
=================
*/
xVec3 xAngles::ToAngularVelocity() const {
	xRotation rotation = xAngles::ToRotation();
	return rotation.Vec() * DEG2RAD(rotation.Angle());
}

/*
=============
xAngles::ToString
=============
*/
xString xAngles::ToString(int precision) const {
	return xString::FloatArrayToString(ToFloatPtr(), Dimension(), precision);
}
